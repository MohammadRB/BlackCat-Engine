 // [10/21/2016 MRB]

#include "Game/GamePCH.h"
#include "Core/File/bcFileStream.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		bc_script_system::bc_script_system()
		{
		}

		bc_script_system::bc_script_system(bc_script_system&& p_other) noexcept
			: core::bc_initializable<bool>(std::move(p_other)),
			m_script_runtime(std::move(p_other.m_script_runtime)),
			m_app_context(std::move(p_other.m_app_context)),
			m_stringify_function(std::move(p_other.m_stringify_function))
		{

		}

		bc_script_system::~bc_script_system()
		{
			destroy();
		}

		bc_script_system& bc_script_system::operator=(bc_script_system&& p_other) noexcept
		{
			core::bc_initializable<bool>::operator=(std::move(p_other));
			m_script_runtime = std::move(p_other.m_script_runtime);
			m_app_context = std::move(p_other.m_app_context);
			m_stringify_function = std::move(p_other.m_stringify_function);

			return *this;
		}

		bc_script_binding bc_script_system::get_script_binder() const noexcept
		{
			return bc_script_binding(const_cast<bc_script_system&>(*this));
		}

		void bc_script_system::set_script_binder(bc_script_binding&& p_bindings)
		{
			for(auto& l_item : p_bindings.m_contexts)
			{
				l_item.first.get().register_global_prototype(std::move(l_item.second));
			}
		}

		platform::bc_script_bytecode bc_script_system::load_script(bc_script_context p_context, const bcECHAR* p_script_file)
		{
			auto* l_game_system = core::bc_get_service<bc_game_system>();
			core::bc_file_stream l_script_file;

			const auto l_script_file_path = l_game_system->get_file_system().get_content_script_path(p_script_file);
			l_script_file.open_read(l_script_file_path.c_str());

			core::bc_wstring_frame l_script_file_content;
			core::bc_wstring_frame l_line;
			
			while (core::bc_read_line(l_script_file, l_line))
			{
				l_script_file_content.append(l_line);
			}

			{
				platform::bc_script_context::scope l_context_scope(_get_context(p_context));
				platform::bc_script_bytecode l_byte_code = m_script_runtime->compile_script(l_script_file_content.c_str());

				if (l_context_scope.get_context().has_exception())
				{
					const auto l_error = l_context_scope.get_context().get_clear_exception();
					const auto l_error_message = core::bc_to_string_frame(l_error.error_message());
					const auto l_error_file = core::bc_to_exclusive_string(p_script_file);

					throw bc_script_compile_exception(l_error_file.c_str(), 0, 0, l_error_message.c_str());
				}

				return l_byte_code;
			}
		}

		platform::bc_script_variable bc_script_system::run_script(bc_script_context p_context, const bcWCHAR* p_script)
		{
			{
				platform::bc_script_context::scope l_context_scope(_get_context(p_context));
				
				platform::bc_script_bytecode l_byte_code = m_script_runtime->compile_script(p_script);
				
				if (l_context_scope.get_context().has_exception())
				{
					platform::bc_script_error l_error = l_context_scope.get_context().get_clear_exception();
				
					return l_context_scope.get_context().create_variable(l_error);
				}

				return run_script(p_context, l_byte_code);
			}
		}

		platform::bc_script_variable bc_script_system::run_script(bc_script_context p_context, platform::bc_script_bytecode& p_script)
		{
			{
				const platform::bc_script_context::scope l_context_scope(_get_context(p_context));

				platform::bc_script_variable l_result = m_script_runtime->run_script(p_script);

				if (l_context_scope.get_context().has_exception())
				{
					auto l_error = l_context_scope.get_context().get_clear_exception();
					return l_context_scope.get_context().create_variable(l_error);
				}

				return l_result;
			}
		}

		platform::bc_script_variable bc_script_system::run_script_throw(bc_script_context p_context, const bcWCHAR* p_script)
		{
			{
				const platform::bc_script_context::scope l_context_scope(_get_context(p_context));

				platform::bc_script_bytecode l_byte_code = m_script_runtime->compile_script(p_script);

				if (l_context_scope.get_context().has_exception())
				{
					const auto l_error = l_context_scope.get_context().get_clear_exception();
					const auto l_error_message = core::bc_to_string_frame(l_error.error_message());
					const auto l_error_script = core::bc_to_exclusive_string(p_script);

					throw bc_script_compile_exception(l_error_script.c_str(), 0, 0, l_error_message.c_str());
				}

				return run_script_throw(p_context, l_byte_code);
			}
		}

		platform::bc_script_variable bc_script_system::run_script_throw(bc_script_context p_context, platform::bc_script_bytecode& p_script)
		{
			platform::bc_script_variable l_result = run_script(p_context, p_script);

			if (l_result.is_error())
			{
				const auto l_error = l_result.as_error();
				const auto l_error_message = core::bc_to_string_frame(l_error.error_message());

				throw bc_script_execute_exception(l_error_message.c_str());
			}

			return l_result;
		}

		core::bc_wstring bc_script_system::stringify(platform::bc_script_variable& p_value)
		{
			core::bc_wstring l_result_string;

			{
				platform::bc_script_context::scope l_context_scope(_get_context(bc_script_context::app));

				if (p_value.is_valid())
				{
					switch (p_value.get_type())
					{
					case platform::bc_script_value_type::function:
					{
						l_result_string = bcL("function");
						break;
					}
					case platform::bc_script_value_type::error:
					{
						l_result_string = p_value.as_error().error_message();
						break;
					}
					case platform::bc_script_value_type::null:
					{
						l_result_string = bcL("null");
						break;
					}
					case platform::bc_script_value_type::undefined:
					{
						l_result_string = bcL("undefined");
						break;
					}
					default:
					{
						auto l_global_obj = l_context_scope.get_context().get_global();
						auto l_arg = l_context_scope.get_context().create_variable(l_global_obj);
						const auto l_json = (*m_stringify_function)(l_arg,p_value);
						l_result_string = l_json.data();
						break;
					}
					}
				}
			}

			return l_result_string;
		}

		void bc_script_system::interrupt_script_execution()
		{
			m_script_runtime->interrupt_script_execution();
		}

		void bc_script_system::collect_garbage()
		{
			m_script_runtime->collect_garbage();
		}

		bcSIZE bc_script_system::memory_usage() const
		{
			return m_script_runtime->memory_usage();
		}

		void bc_script_system::update(platform::bc_clock::update_param p_clock_update_param)
		{
		}

		void bc_script_system::_initialize(bool)
		{
			m_script_runtime = core::bc_make_unique<platform::bc_script_runtime>(core::bc_alloc_type::program);
			m_app_context.reset(m_script_runtime->create_context());

			{
				platform::bc_script_context::scope l_context_scope(_get_context(bc_script_context::app));

				m_stringify_function = l_context_scope.get_context()
					.get_global()
					.get_property(L"JSON")
					.as_object()
					.get_property(L"stringify")
					.as_function();
			}
		}

		void bc_script_system::_destroy()
		{
			if (m_stringify_function->is_valid())
			{
				{
					platform::bc_script_context::scope l_context_scope(_get_context(bc_script_context::app));
					m_stringify_function.reset();
				}
			}

			m_app_context.reset();
			m_script_runtime.reset();
		}
	}
}