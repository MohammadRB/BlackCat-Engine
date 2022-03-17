// [12/20/2018 MRB]

#pragma once

#include "Core/Utility/bcDelegate.h"
#include "PlatformImp/Application/bcApplication.h"
#include "PhysicsImp/Fundation/bcSceneBuilder.h"
#include "Game/Application/bcIRenderApplicationOutputWindow.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphNode.h"

namespace black_cat
{
	namespace game
	{
		struct bc_engine_component_parameter
		{
			bc_engine_component_parameter(bcSIZE p_memmng_fsa_start_size,
				bcSIZE p_memmng_fsa_count,
				bcSIZE p_memmng_fsa_step_size,
				bcSIZE p_memmng_fsa_allocation_count,
				bcSIZE p_memmng_program_stack_size,
				bcSIZE p_memmng_frame_stack_size,
				bcSIZE p_memmng_super_heap_size,
				bcSIZE p_thread_manager_thread_count,
				bcSIZE p_thread_manager_reserved_thread_count)
				: m_memmng_fsa_start_size(p_memmng_fsa_start_size),
				m_memmng_fsa_count(p_memmng_fsa_count),
				m_memmng_fsa_step_size(p_memmng_fsa_step_size),
				m_memmng_fsa_allocation_count(p_memmng_fsa_allocation_count),
				m_memmng_program_stack_size(p_memmng_program_stack_size),
				m_memmng_frame_stack_size(p_memmng_frame_stack_size),
				m_memmng_super_heap_size(p_memmng_super_heap_size),
				m_thread_manager_thread_count(p_thread_manager_thread_count),
				m_thread_manager_reserved_thread_count(p_thread_manager_reserved_thread_count)
			{
			}

			bcSIZE m_memmng_fsa_start_size;
			bcSIZE m_memmng_fsa_count;
			bcSIZE m_memmng_fsa_step_size;
			bcSIZE m_memmng_fsa_allocation_count;
			bcSIZE m_memmng_program_stack_size;
			bcSIZE m_memmng_frame_stack_size;
			bcSIZE m_memmng_super_heap_size;
			bcSIZE m_thread_manager_thread_count;
			bcSIZE m_thread_manager_reserved_thread_count;
		};

		struct bc_scene_graph_node_factory_parameter
		{
			core::bc_vector3f m_center;
			core::bc_vector3f m_half_extends;
		};

		using bc_render_output_window_factory = core::bc_delegate<bci_render_application_output_window*()>;
		using bc_px_scene_builder_factory = core::bc_delegate<physics::bc_scene_builder()>;
		using bc_scene_graph_node_factory = core::bc_delegate<core::bc_unique_ptr<bci_scene_graph_node>(bc_scene_graph_node_factory_parameter)>;
		
		struct bc_render_application_parameter : public platform::bc_application_parameter
		{
			bc_render_application_parameter(const platform::bc_application_parameter& p_app_parameters, 
				bc_render_output_window_factory p_output_window_factory, 
				bc_scene_graph_node_factory p_scene_graph_factory)
				: platform::bc_application_parameter(p_app_parameters),
				m_output_window_factory(std::move(p_output_window_factory)),
				m_scene_graph_factory(std::move(p_scene_graph_factory))
			{
			}

			bc_render_application_parameter(bc_render_application_parameter&&) = default;

			~bc_render_application_parameter() = default;

			bc_render_application_parameter& operator=(bc_render_application_parameter&&) = default;

			bc_render_output_window_factory m_output_window_factory;
			bc_scene_graph_node_factory m_scene_graph_factory;
		};

		struct bc_engine_application_parameter
		{
			bc_engine_application_parameter(const bc_engine_component_parameter& p_engine_parameters, bc_render_application_parameter p_app_parameters)
				: m_engine_parameters(p_engine_parameters),
				m_app_parameters(std::move(p_app_parameters))
			{
			}

			bc_engine_component_parameter m_engine_parameters;
			bc_render_application_parameter m_app_parameters;
		};
	}
}