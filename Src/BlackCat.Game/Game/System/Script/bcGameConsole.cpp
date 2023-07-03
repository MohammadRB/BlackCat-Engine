 // [22/10/2016 MRB]

#include "Game/GamePCH.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Platform/bcEvent.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Script/bcGameConsole.h"
#include "Game/System/Script/bcIGameConsoleImp.h"
#include "Game/System/Script/bcScriptSystem.h"

namespace black_cat
{
	namespace game
	{
		bc_game_console::bc_game_console(bc_script_system& p_script_system)
			: m_script_system(&p_script_system),
			m_imp(),
			m_log_types({true, true, true, true, true}),
			m_bound_context(nullptr),
			m_bound_console()
		{
			auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();

			m_key_event_handle = l_event_manager->register_event_listener<platform::bc_app_event_key>
			(
				core::bc_event_manager::delegate_type(*this, &bc_game_console::_on_key)
			);
		}

		bc_game_console::bc_game_console(bc_game_console&& p_other) noexcept
			: m_script_system(p_other.m_script_system),
			m_imp(p_other.m_imp),
			m_log_types(p_other.m_log_types),
			m_scripts(std::move(p_other.m_scripts)),
			m_bound_context(p_other.m_bound_context),
			m_bound_console(std::move(p_other.m_bound_console))
		{
		}

		bc_game_console::~bc_game_console()
		{
			if (m_bound_console->is_valid())
			{
				{
					platform::bc_script_context::scope l_scope(*m_bound_context);
					m_bound_console.reset();
				}
			}
			
			core::bc_get_service<core::bc_logger>()->unregister_listener(this);
		}

		bc_game_console& bc_game_console::operator=(bc_game_console&& p_other) noexcept
		{
			m_script_system = p_other.m_script_system;
			m_imp = p_other.m_imp;
			m_log_types = p_other.m_log_types;
			m_scripts = std::move(p_other.m_scripts);
			m_bound_context = p_other.m_bound_context;
			m_bound_console = std::move(p_other.m_bound_console);

			return *this;
		}

		void bc_game_console::set_implementation(bci_game_console_imp* p_imp)
		{
			const bool l_register_listener = !m_imp;
			
			m_imp = p_imp;
			
			if(l_register_listener)
			{
				auto* l_logger = core::bc_get_service<core::bc_logger>();
				l_logger->register_listener(core::bc_log_type::all, this);
			}
		}

		void bc_game_console::disable_output(bc_console_output_type p_output) noexcept
		{
			m_log_types[std::log2(static_cast<bcSIZE>(p_output))] = false;
		}

		void bc_game_console::enable_output(bc_console_output_type p_output) noexcept
		{
			m_log_types[std::log2(static_cast<bcSIZE>(p_output))] = true;
		}

		void bc_game_console::clear_output()
		{
			if (m_imp)
			{
				m_imp->clear_output();
			}
		}

		void bc_game_console::run_script(const bcWCHAR* p_script, bool p_output_to_console)
		{
			{
				platform::bc_mutex_guard l_guard(m_scripts_mutex);

				_bc_script_queue_entry l_item
				{
					p_output_to_console,
					core::bc_wstring(p_script),
					platform::bc_promise<platform::bc_script_variable>()
				};

				m_scripts.push_back(std::move(l_item));
			}
		}

		void bc_game_console::run_script(const bcWCHAR* p_script, platform::bc_future<platform::bc_script_variable>* p_result, bool p_output_to_console)
		{
			BC_ASSERT(p_result);

			{
				platform::bc_mutex_guard l_guard(m_scripts_mutex);

				_bc_script_queue_entry l_item
				{
					p_output_to_console,
					core::bc_wstring(p_script),
					platform::bc_promise<platform::bc_script_variable>()
				};
				*p_result = l_item.m_promise.get_future();

				m_scripts.push_back(std::move(l_item));
			}
		}

		void bc_game_console::update(const platform::bc_clock::update_param& p_clock)
		{
			if (m_imp)
			{
				m_imp->update(p_clock);
			}

			platform::bc_script_variable l_result;
			core::bc_estring_frame l_result_string;

			{
				platform::bc_mutex_guard l_guard(m_scripts_mutex);

				if (!m_scripts.empty())
				{
					for (_bc_script_queue_entry& l_item : m_scripts)
					{
						l_result = m_script_system->run_script(bc_script_context::app, l_item.m_script.c_str());
						l_item.m_promise.set_value(l_result);

						if(l_item.m_output_to_console)
						{
							auto l_string = m_script_system->stringify(l_result);
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

				auto l_console_builder = p_context.create_prototype_builder<bc_game_console_script>();
				
				l_console_builder
					.constant(L"outputInfo", static_cast<platform::bc_script_int>(bc_console_output_type::info))
					.constant(L"outputDebug", static_cast<platform::bc_script_int>(bc_console_output_type::debug))
					.constant(L"outputWarning", static_cast<platform::bc_script_int>(bc_console_output_type::warning))
					.constant(L"outputError", static_cast<platform::bc_script_int>(bc_console_output_type::error))
					.constant(L"outputScript", static_cast<platform::bc_script_int>(bc_console_output_type::script))
					.function(L"enableOutput", &bc_game_console_script::enable_output)
					.function(L"disableOutput", &bc_game_console_script::disable_output)
					.function(L"clear", &bc_game_console_script::clear_output);

				auto l_console_prototype = p_context.create_prototype(l_console_builder);
				auto l_console_object = platform::bc_script_object_ref(p_context.create_object(l_console_prototype, bc_game_console_script(p_instance)));

				p_instance.m_bound_context = &p_context;
				p_instance.m_bound_console = std::move(l_console_object);

				platform::bc_script_property_descriptor<platform::bc_script_object> l_console_descriptor(&p_instance.m_bound_console.get(), false);
				p_global_prototype.property(L"console", l_console_descriptor);
			}
		}
		
		void bc_game_console::on_log(core::bc_log_type p_type, const bcECHAR* p_log)
		{
			auto l_type = static_cast<bc_console_output_type>(p_type);

			if (m_imp)
			{
				const bool l_log_enabled = m_log_types[std::log2(static_cast<bcSIZE>(l_type))];
				if(l_log_enabled)
				{
					m_imp->write_output(l_type, p_log);
				}
			}
		}

		void bc_game_console::_on_key(core::bci_event& p_event)
		{
			if(!m_imp)
			{
				return;
			}

			const auto l_key_event = static_cast<platform::bc_app_event_key&>(p_event);

			if (l_key_event.get_key() == platform::bc_key::kb_grave && l_key_event.get_key_state() == platform::bc_key_state::pressing)
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
		}
	}
}