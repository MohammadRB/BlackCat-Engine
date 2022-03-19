// [11/22/2018 MRB]

#include "Editor/EditorPCH.h"

#include "Game/Object/Scene/SceneGraph/bcOctalTreeSceneGraphNode.h"
#include "Editor/Application/bcEditorRenderApplicationThread.h"
#include "Editor/Application/bcEditorRenderApplication.h"

namespace black_cat
{
	namespace editor
	{
		void bc_editor_render_app_thread::start(HINSTANCE p_instance, 
			const char* p_cmd_line, 
			bc_editor_render_app_factory p_render_app_factory, 
			game::bci_render_application_output_window* p_output_window)
		{
			m_instance = p_instance;
			m_cmd_line = p_cmd_line;
			m_render_app_factory = std::move(p_render_app_factory);
			m_output_window = p_output_window;
			m_result_code = s_default_result_code;

			QThread::start();
		}

		void bc_editor_render_app_thread::wait_for_initialization() const
		{
			while (m_initialized.load() == 0) {}
		}

		std::int32_t bc_editor_render_app_thread::is_still_running() const noexcept
		{
			return m_result_code.load() == s_default_result_code;
		}

		std::int32_t bc_editor_render_app_thread::get_result_code() const noexcept
		{
			return m_result_code.load();
		}

		void bc_editor_render_app_thread::run()
		{
			platform::bc_basic_hardware_info l_hardware_info{};
			platform::bc_hardware_info::get_basic_info(l_hardware_info);

			platform::bc_application_parameter l_app_parameters
			(
				m_instance,
				bcL("BlackCatEditor"),
				m_cmd_line
			);
			game::bc_render_application_parameter l_render_app_parameters
			(
				l_app_parameters,
				[=]() 
				{
					return m_output_window;
				},
				[](game::bc_scene_graph_node_factory_parameter p_parameter)
				{
					return core::bc_make_unique<game::bc_octal_tree_graph_node>
					(
						physics::bc_bound_box(p_parameter.m_center, p_parameter.m_half_extends),
						10,
						128
					);
				}
			);
			game::bc_engine_component_parameter l_engine_component_parameters
			(
				32,
				32,
				16,
				128,
				static_cast<bcUINT32>(core::bc_mem_size::mb) * 32,
				static_cast<bcUINT32>(core::bc_mem_size::mb) * 32,
				static_cast<bcUINT32>(core::bc_mem_size::mb) * 128,
				std::max(4U, l_hardware_info.m_processor_count),
				std::max(4U, l_hardware_info.m_processor_count)
			);
			game::bc_engine_application_parameter l_engine_app_parameters
			(
				l_engine_component_parameters,
				std::move(l_render_app_parameters)
			);

			auto* l_app = m_render_app_factory();
			l_app->initialize(l_engine_app_parameters);
			l_app->set_fps(60);

			m_initialized.store(1);
			m_result_code.store(l_app->run());

			l_app->destroy();
		}
	}
}