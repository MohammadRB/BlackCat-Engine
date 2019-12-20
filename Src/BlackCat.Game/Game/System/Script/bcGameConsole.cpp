 // [10/22/2016 MRB]

#include "Game/GamePCH.h"
#include "Core/Event/bcEventManager.h"
#include "Platform/bcEvent.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Script/bcGameConsole.h"
#include "Game/System/Script/bcIGameConsoleImp.h"

namespace black_cat
{
	namespace game
	{
		// == bc_game_console_bind ===========================================================================

		bc_game_console_bind::bc_game_console_bind(bc_game_console& p_game_console)
			: m_game_console(p_game_console)
		{
		}

		bc_game_console_bind::bc_game_console_bind(const bc_game_console_bind& p_other)
			: m_game_console(p_other.m_game_console)
		{
		}

		bc_game_console_bind::~bc_game_console_bind()
		{
		}

		bc_game_console_bind& bc_game_console_bind::operator=(const bc_game_console_bind& p_other)
		{
			return *this;
		}

		void bc_game_console_bind::disable_output(const platform::bc_script_int& p_output)
		{
			m_game_console.disable_output(static_cast< bc_console_output_type >(p_output));
		}

		void bc_game_console_bind::enable_output(const platform::bc_script_int& p_output)
		{
			m_game_console.enable_output(static_cast< bc_console_output_type >(p_output));
		}

		void bc_game_console_bind::clear_output()
		{
			m_game_console.clear_output();
		}

		// == bc_game_console ==================================================================================

		bc_game_console::bc_game_console(bc_script_system& p_script_system)
			: m_script_system(p_script_system),
			m_imp(),
			m_log_types({true, true, true, true}),
			m_logs(),
			m_bound_context(nullptr),
			m_bound_console()
		{
			core::bc_get_service< core::bc_logger >()->register_listener
			(
				core::bc_enum::or({core::bc_log_type::info, core::bc_log_type::debug, core::bc_log_type::error}),
				this
			);
			m_key_event_handle = core::bc_get_service< core::bc_event_manager >()->register_event_listener< platform::bc_app_event_key >
			(
				core::bc_event_manager::delegate_type(this, &bc_game_console::_on_key)
			);
		}

		bc_game_console::bc_game_console(bc_game_console&& p_other) noexcept
			: m_script_system(p_other.m_script_system),
			m_imp(p_other.m_imp),
			m_log_types(p_other.m_log_types),
			m_logs(std::move(p_other.m_logs)),
			m_scripts(std::move(p_other.m_scripts)),
			m_bound_context(p_other.m_bound_context),
			m_bound_console(std::move(p_other.m_bound_console))
		{
		}

		bc_game_console::~bc_game_console()
		{
			if(m_bound_console->is_valid())
			{
				{
					platform::bc_script_context::scope l_scope(*m_bound_context);
					m_bound_console.reset();
				}
			}

			core::bc_get_service< core::bc_logger >()->unregister_listener(this);
		}

		bc_game_console& bc_game_console::operator=(bc_game_console&& p_other) noexcept
		{
			m_imp = p_other.m_imp;
			m_log_types = p_other.m_log_types;
			m_logs = std::move(p_other.m_logs);
			m_scripts = std::move(p_other.m_scripts);
			m_bound_console = std::move(p_other.m_bound_console);

			return *this;
		}

		void bc_game_console::set_implementation(bc_igame_console_imp* p_imp)
		{
			m_imp = p_imp;
		}

		void bc_game_console::disable_output(bc_console_output_type p_output) noexcept
		{
			m_log_types[std::log2(static_cast<bcSIZE>(p_output))] = false;

			if(m_imp)
			{
				m_imp->clear_output();
				_write_logs();
			}
		}

		void bc_game_console::enable_output(bc_console_output_type p_output) noexcept
		{
			m_log_types[std::log2(static_cast<bcSIZE>(p_output))] = true;

			if (m_imp)
			{
				m_imp->clear_output();
				_write_logs();
			}
		}

		void bc_game_console::clear_output()
		{
			if (m_imp)
			{
				m_imp->clear_output();
			}
			m_logs.clear();
		}

		void bc_game_console::run_script(const bcWCHAR* p_script, bool p_output_to_console)
		{
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_scripts_mutex);

				_bc_script_queue_entry l_item
				{
					p_output_to_console,
					core::bc_wstring(p_script),
					core_platform::bc_promise< platform::bc_script_variable >()
				};

				m_scripts.push_back(std::move(l_item));
			}
		}

		void bc_game_console::run_script(const bcWCHAR* p_script, core_platform::bc_future<platform::bc_script_variable>* p_result, bool p_output_to_console)
		{
			bcAssert(p_result);

			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_scripts_mutex);

				_bc_script_queue_entry l_item
				{
					p_output_to_console,
					core::bc_wstring(p_script),
					core_platform::bc_promise< platform::bc_script_variable >()
				};
				*p_result = l_item.m_promise.get_future();

				m_scripts.push_back(std::move(l_item));
			}
		}

		void bc_game_console::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			if (m_imp)
			{
				m_imp->update(p_clock_update_param);
			}

			platform::bc_script_variable l_result;
			core::bc_estring_frame l_result_string;

			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_scripts_mutex);

				if (!m_scripts.empty())
				{
					for (_bc_script_queue_entry& l_item : m_scripts)
					{
						l_result = m_script_system.run_script(bc_script_context::ui, l_item.m_script.c_str());
						l_item.m_promise.set_value(l_result);

						if(l_item.m_output_to_console)
						{
							auto l_string = m_script_system.stringify(l_result);
							on_log(static_cast<core::bc_log_type>(bc_console_output_type::script), l_string.c_str());
						}
					}

					m_scripts.clear();
				}
			}
		}

		void bc_game_console::bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bc_game_console& p_instance)
		{
			{
				platform::bc_script_context::scope l_scope(p_context);

				auto l_console_builder = p_context.create_prototype_builder< bc_game_console_bind >();

				l_console_builder
					.constant(L"outputInfo", static_cast< platform::bc_script_int >(bc_console_output_type::info))
					.constant(L"outputDebug", static_cast< platform::bc_script_int >(bc_console_output_type::debug))
					.constant(L"outputError", static_cast< platform::bc_script_int >(bc_console_output_type::error))
					.constant(L"outputScript", static_cast< platform::bc_script_int >(bc_console_output_type::script))
					.function(L"enableOutput", &bc_game_console_bind::enable_output)
					.function(L"disableOutput", &bc_game_console_bind::disable_output)
					.function(L"clear", &bc_game_console_bind::clear_output);

				auto l_console_prototype = p_context.create_prototype(l_console_builder);
				auto l_console_object = platform::bc_script_object_ref(p_context.create_object(l_console_prototype, bc_game_console_bind(p_instance)));

				platform::bc_script_property_descriptor< platform::bc_script_object > l_console_descriptor(&l_console_object.get(), false);

				p_global_prototype.property(L"console", l_console_descriptor);

				p_instance.m_bound_context = &p_context;
				p_instance.m_bound_console = std::move(l_console_object);
			}
		}

		void bc_game_console::on_log(core::bc_log_type p_type, const bcECHAR* p_log)
		{
			auto l_type = static_cast<bc_console_output_type>(p_type);
			auto l_log = core::bc_estring(p_log);

			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_logs_mutex);

				m_logs.push_back(std::make_pair(l_type, std::move(l_log)));
			}

			if (m_imp)
			{
				bool l_log_enabled = m_log_types[std::log2(static_cast<bcSIZE>(l_type))];

				if(l_log_enabled)
				{
					m_imp->write_output(l_type, m_logs.rbegin()->second);
				}
			}
		}

		bool bc_game_console::_on_key(core::bc_ievent& p_event)
		{
			bcAssert(core::bc_ievent::event_is< platform::bc_app_event_key >(p_event), "");

			if(!m_imp)
			{
				return false;
			}

			auto l_key_event = static_cast< platform::bc_app_event_key& >(p_event);

			if (l_key_event.get_key() == platform::bc_key::kb_grave &&
				l_key_event.get_key_state() == platform::bc_key_state::pressing)
			{
				if(m_imp->is_visible())
				{
					m_imp->hide();
				}
				else
				{
					m_imp->show();
				}
			}

			return true;
		}

		void bc_game_console::_write_logs()
		{
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_logs_mutex);

				for (auto& l_log : m_logs)
				{
					bool l_log_enabled = m_log_types[std::log2(static_cast<bcSIZE>(l_log.first))];

					if (l_log_enabled)
					{
						m_imp->write_output(l_log.first, l_log.second);
					}
				}
			}
		}
	}
}