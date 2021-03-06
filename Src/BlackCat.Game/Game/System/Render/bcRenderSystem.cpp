// [05/10/2016 MRB]

#include "Game/GamePCH.h"

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatformImp/Utility/bcHardwareInfo.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Utility/bcLogger.h"
#include "Core/bcEvent.h"
#include "Platform/bcEvent.h"
#include "Graphic/bcEvent.h"
#include "GraphicImp/Device/bcDevice.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Render/bcRenderTask.h"
#include "Game/System/Render/bcRenderThreadManager.h"
#include "Game/System/Render/bcFrameRenderer.h"
#include "Game/System/Render/Light/bcLightManager.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Mesh/bcMeshUtility.h"
#include "Game/Object/Animation/bcAnimationManager.h"

namespace black_cat
{
	namespace game
	{
		extern graphic::bc_pipeline_stage _convert_shader_type_to_pipeline_stage(graphic::bc_shader_type p_shader_types);

		bc_render_system::bc_render_system()
			: m_content_stream(nullptr)
		{
		}

		bc_render_system::bc_render_system(bc_render_system&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_render_system::~bc_render_system()
		{
			if(m_initialized)
			{
				destroy();
			}
		}

		bc_render_system& bc_render_system::operator=(bc_render_system&& p_other) noexcept
		{
			m_device = std::move(p_other.m_device);
			m_content_stream = p_other.m_content_stream;
			m_thread_manager = std::move(p_other.m_thread_manager);
			m_material_manager = std::move(p_other.m_material_manager);
			m_render_pass_manager = std::move(p_other.m_render_pass_manager);
			m_animation_manager = std::move(p_other.m_animation_manager);
			m_shape_drawer = std::move(p_other.m_shape_drawer);
			m_frame_renderer = std::move(p_other.m_frame_renderer);

			m_window_resize_handle = std::move(p_other.m_window_resize_handle);
			m_app_active_handle = std::move(p_other.m_app_active_handle);
			m_device_reset_handle = std::move(p_other.m_device_reset_handle);
			m_frame_render_finish_handle = std::move(p_other.m_frame_render_finish_handle);

			m_render_pass_states = std::move(p_other.m_render_pass_states);
			m_render_states = std::move(p_other.m_render_states);
			m_compute_states = std::move(p_other.m_compute_states);

			m_window_resize_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_render_system::_event_handler));
			m_app_active_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_render_system::_event_handler));
			m_device_reset_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_render_system::_event_handler));
			m_frame_render_finish_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_render_system::_event_handler));
			
			return *this;
		}

		void bc_render_system::update(const update_context& p_update_params)
		{
			m_frame_renderer->update(bc_frame_renderer_update_context(p_update_params.m_clock, bc_camera_instance(p_update_params.m_camera)));
		}

		void bc_render_system::render(const render_context& p_render_param)
		{
			m_frame_renderer->render(bc_frame_renderer_render_context(p_render_param.m_clock, p_render_param.m_query_manager, *this));

			m_device.present();
		}

		void bc_render_system::swap(const swap_context& p_swap_context)
		{
			if(!m_device_reset_event.has_value())
			{
				return;
			}
			
			m_render_pass_manager->before_reset(bc_render_pass_reset_context
			(
				*this,
				*m_device_reset_event->m_device,
				m_device_reset_event->m_old_parameters,
				m_device_reset_event->m_new_parameters
			));

			m_device.resize_back_buffer(m_device_reset_event->m_new_parameters.m_width, m_device_reset_event->m_new_parameters.m_height);

			m_render_pass_manager->after_reset(bc_render_pass_reset_context
			(
				*this,
				*m_device_reset_event->m_device,
				m_device_reset_event->m_old_parameters,
				m_device_reset_event->m_new_parameters
			));

			m_device_reset_event.reset();
		}

		void bc_render_system::add_render_task(bci_render_task& p_task)
		{
			core::bc_task<void> l_cpu_task = core::bc_concurrency::start_task<void>([this, &p_task]()
			{
				const auto l_render_thread_guard = m_thread_manager->get_available_thread_wait();
				auto& l_render_thread = *l_render_thread_guard.get_thread();

				p_task.execute(*this, l_render_thread);
			});

			p_task._set_cpu_task(std::move(l_cpu_task));
		}

		graphic::bc_device_pipeline_state_ref bc_render_system::create_device_pipeline_state(const bcCHAR* p_vertex_shader_name,
			const bcCHAR* p_hull_shader_name,
			const bcCHAR* p_domain_shader_name,
			const bcCHAR* p_geometry_shader_name,
			const bcCHAR* p_pixel_shader_name,
			bc_vertex_type p_vertex_layout, 
			bc_blend_type p_blend,
			bc_depth_stencil_type p_depth_stencil, 
			bc_rasterizer_type p_rasterizer, 
			bcUINT p_sample_mask, 
			std::initializer_list< graphic::bc_format > p_render_target_formats, 
			graphic::bc_format p_depth_stencil_format, 
			bc_multi_sample_type p_ms_config)
		{
			BC_ASSERT(p_render_target_formats.size() < graphic::bc_render_api_info::number_of_om_render_target_slots());

			graphic::bc_vertex_shader_content_ptr l_vertex_shader;
			graphic::bc_hull_shader_content_ptr l_hull_shader;
			graphic::bc_domain_shader_content_ptr l_domain_shader;
			graphic::bc_geometry_shader_content_ptr l_geometry_shader;
			graphic::bc_pixel_shader_content_ptr l_pixel_shader;

			if(p_vertex_shader_name)
			{
				l_vertex_shader = m_content_stream->find_content_throw< graphic::bc_vertex_shader_content >(p_vertex_shader_name);
			}
			if (p_hull_shader_name)
			{
				l_hull_shader = m_content_stream->find_content_throw< graphic::bc_hull_shader_content >(p_hull_shader_name);
			}
			if (p_domain_shader_name)
			{
				l_domain_shader = m_content_stream->find_content_throw< graphic::bc_domain_shader_content >(p_domain_shader_name);
			}
			if (p_geometry_shader_name)
			{
				l_geometry_shader = m_content_stream->find_content_throw< graphic::bc_geometry_shader_content >(p_geometry_shader_name);
			}
			if (p_pixel_shader_name)
			{
				l_pixel_shader = m_content_stream->find_content_throw< graphic::bc_pixel_shader_content >(p_pixel_shader_name);
			}

			const auto l_input_layout_config = bc_graphic_state_configs::bc_input_layout_config(p_vertex_layout);
			const auto l_blend_state_config = bc_graphic_state_configs::bc_blend_config(p_blend);
			const auto l_depth_stencil_state_config = bc_graphic_state_configs::bc_depth_stencil_config(p_depth_stencil);
			const auto l_rasterizer_state_config = bc_graphic_state_configs::bc_rasterizer_config(p_rasterizer);
			const auto l_sample_state_config = bc_graphic_state_configs::bc_texture_multi_sample(p_ms_config);

			graphic::bc_device_pipeline_state_config l_pipeline_config;

			l_pipeline_config.m_vertex_shader = l_vertex_shader ? l_vertex_shader.get()->get_resource() : graphic::bc_vertex_shader();
			l_pipeline_config.m_hull_shader = l_hull_shader ? l_hull_shader.get()->get_resource() : graphic::bc_hull_shader();
			l_pipeline_config.m_domain_shader = l_domain_shader ? l_domain_shader.get()->get_resource() : graphic::bc_domain_shader();
			l_pipeline_config.m_geometry_shader = l_geometry_shader ? l_geometry_shader.get()->get_resource() : graphic::bc_geometry_shader();
			l_pipeline_config.m_pixel_shader = l_pixel_shader ? l_pixel_shader.get()->get_resource() : graphic::bc_pixel_shader();
			l_pipeline_config.m_blend_state_config = l_blend_state_config;
			l_pipeline_config.m_sample_mask = p_sample_mask;
			l_pipeline_config.m_depth_stencil_state_config = l_depth_stencil_state_config;
			l_pipeline_config.m_rasterizer_state_config = l_rasterizer_state_config;
			l_pipeline_config.m_input_layout_config = l_input_layout_config;
			l_pipeline_config.m_num_render_target = p_render_target_formats.size();
			std::copy(std::begin(p_render_target_formats), std::end(p_render_target_formats), std::begin(l_pipeline_config.m_render_target_format));
			l_pipeline_config.m_depth_stencil_format = p_depth_stencil_format;
			l_pipeline_config.m_sample_config = l_sample_state_config;
			
			return m_device.create_pipeline_state(l_pipeline_config);
		}

		graphic::bc_device_compute_state_ref bc_render_system::create_device_compute_state(const bcCHAR* p_compute_shader_name)
		{
			graphic::bc_compute_shader_content_ptr l_compute_shader = m_content_stream->find_content_throw< graphic::bc_compute_shader_content >(p_compute_shader_name);

			graphic::bc_device_compute_state_config l_compute_config;
			l_compute_config.m_compute_shader = l_compute_shader->get_resource();

			return m_device.create_compute_state(l_compute_config);
		}

		bc_render_pass_state_ptr bc_render_system::create_render_pass_state(graphic::bc_device_pipeline_state p_pipeline_state,
			graphic::bc_viewport p_viewport,
			bc_render_pass_state_render_target_view_array p_shader_targets,
			graphic::bc_depth_stencil_view p_shader_depth,
			bc_render_pass_state_sampler_array p_shader_samplers,
			bc_render_pass_state_resource_view_array p_resource_views,
			bc_render_pass_state_unordered_view_array p_unordered_views,
			bc_render_pass_state_constant_buffer_array p_shader_buffers)
		{
			bc_render_pass_state l_render_pass_state
			(
				p_pipeline_state,
				p_viewport,
				std::move(p_shader_targets),
				p_shader_depth,
				std::move(p_shader_samplers),
				std::move(p_resource_views),
				std::move(p_unordered_views),
				std::move(p_shader_buffers)
			);

			bcSIZE l_parameter_register = g_render_pass_state_sampler_min_index;

			for (auto& l_sampler_parameter : l_render_pass_state.m_shader_samplers)
			{
				l_sampler_parameter.set_register_index(l_parameter_register++);
			}

			l_parameter_register = g_render_pass_state_shader_view_min_index;

			for (auto& l_shader_parameter : l_render_pass_state.m_resource_views)
			{
				l_shader_parameter.set_register_index(l_parameter_register++);
			}

			l_parameter_register = g_render_pass_state_constant_buffer_min_index;

			for (auto& l_cbuffer_parameter : l_render_pass_state.m_shader_cbuffers)
			{
				l_cbuffer_parameter.set_register_index(l_parameter_register++);
			}

			auto* l_render_pass_state_ptr = m_render_pass_states.alloc(std::move(l_render_pass_state));
			
			return bc_render_pass_state_ptr(l_render_pass_state_ptr, _bc_render_pass_state_handle_deleter(this));
		}
		
		bc_render_state_ptr bc_render_system::create_render_state(graphic::bc_primitive p_primitive,
			graphic::bc_buffer p_vertex_buffer, 
			bcUINT32 p_vertex_buffer_stride,
			bcUINT32 p_vertex_buffer_offset, 
			graphic::bc_buffer p_index_buffer, 
			bc_index_type p_index_type,
			bcUINT32 p_index_count, 
			bcUINT32 p_index_buffer_offset, 
			bc_render_state_resource_view_array p_resource_views,
			bc_render_state_constant_buffer_array p_shader_buffers)
		{
			bc_render_state l_render_state
			(
				p_primitive,
				p_vertex_buffer,
				p_vertex_buffer_stride,
				p_vertex_buffer_offset,
				p_index_buffer,
				p_index_type,
				p_index_count,
				p_index_buffer_offset,
				std::move(p_resource_views),
				std::move(p_shader_buffers)
			);

			bcSIZE l_parameter_register = g_render_state_shader_view_min_index;

			for (auto& l_shader_parameter : l_render_state.m_resource_views)
			{
				l_shader_parameter.set_register_index(l_parameter_register++);
			}

			l_parameter_register = g_render_state_constant_buffer_min_index;

			for (auto& l_cbuffer_parameter : l_render_state.m_shader_cbuffers)
			{
				l_cbuffer_parameter.set_register_index(l_parameter_register++);
			}

			auto* l_render_state_ptr = m_render_states.alloc(std::move(l_render_state));
			
			return bc_render_state_ptr(l_render_state_ptr, _bc_render_state_handle_deleter(this));
		}

		bc_compute_state_ptr bc_render_system::create_compute_state(graphic::bc_device_compute_state p_compute_state,
			bc_compute_state_sampler_array p_samplers,
			bc_compute_state_resource_view_array p_resource_views,
			bc_compute_state_unordered_view_array p_unordered_views,
			bc_compute_state_constant_buffer_array p_cbuffers)
		{
			bc_compute_state l_compute_state
			(
				p_compute_state,
				std::move(p_samplers),
				std::move(p_resource_views),
				std::move(p_unordered_views),
				std::move(p_cbuffers)
			);

			bcSIZE l_parameter_register = g_compute_state_sampler_min_index;

			for (auto& l_sampler_parameter : l_compute_state.m_samplers)
			{
				l_sampler_parameter.set_shader_types(graphic::bc_shader_type::compute);
				l_sampler_parameter.set_register_index(l_parameter_register++);
			}

			l_parameter_register = g_compute_state_shader_view_min_index;

			for (auto& l_shader_parameter : l_compute_state.m_resource_views)
			{
				l_shader_parameter.set_shader_types(graphic::bc_shader_type::compute);
				l_shader_parameter.set_register_index(l_parameter_register++);
			}

			l_parameter_register = g_compute_state_unordered_view_min_index;

			for (auto& l_unordered_parameter : l_compute_state.m_unordered_views)
			{
				l_unordered_parameter.set_shader_types(graphic::bc_shader_type::compute);
				l_unordered_parameter.set_register_index(l_parameter_register++);
			}

			l_parameter_register = g_compute_state_constant_buffer_min_index;

			for (auto& l_cbuffer_parameter : l_compute_state.m_cbuffers)
			{
				l_cbuffer_parameter.set_shader_types(graphic::bc_shader_type::compute);
				l_cbuffer_parameter.set_register_index(l_parameter_register++);
			}

			auto* l_compute_state_ptr = m_compute_states.alloc(std::move(l_compute_state));

			return bc_compute_state_ptr(l_compute_state_ptr, _bc_compute_state_handle_deleter(this));
		}

		void bc_render_system::destroy_render_passes()
		{
			m_render_pass_manager->pass_destroy(*this);
			// Delete all render passes to clear queries and references which they may hold
			m_render_pass_manager->pass_clear();
		}

		void bc_render_system::_initialize(core::bc_content_stream_manager& p_content_stream, bc_physics_system& p_physics_system, bc_render_system_parameter p_parameter)
		{
			m_device.initialize
			(
				p_parameter.m_device_backbuffer_width,
				p_parameter.m_device_backbuffer_height,
				p_parameter.m_device_backbuffer_format,
				std::move(p_parameter.m_render_output)
			);
			
			const auto l_alloc_type = m_device.set_allocator_alloc_type(core::bc_alloc_type::program);
			
			core_platform::bc_basic_hardware_info l_hw_info;
			core_platform::bc_hardware_info::get_basic_info(&l_hw_info);

			m_render_pass_states.initialize(20, core::bc_alloc_type::program);
			m_render_states.initialize(500, core::bc_alloc_type::program);
			m_compute_states.initialize(20, core::bc_alloc_type::program);
			
			m_content_stream = &p_content_stream;
			m_thread_manager = core::bc_make_unique< bc_render_thread_manager >(core::bc_alloc_type::program , *this, std::max(1U, l_hw_info.proccessor_count / 2));
			m_material_manager = core::bc_make_unique< bc_material_manager >(core::bc_alloc_type::program, *m_content_stream, *this, p_physics_system);
			m_render_pass_manager = core::bc_make_unique< bc_render_pass_manager >(core::bc_alloc_type::program);
			m_animation_manager = core::bc_make_unique< bc_animation_manager >(core::bc_alloc_type::program);
			m_shape_drawer = core::bc_make_unique< bc_shape_drawer >(core::bc_alloc_type::program);
			m_frame_renderer = core::bc_make_unique< bc_frame_renderer >(core::bc_alloc_type::program, m_device, *m_thread_manager, *m_render_pass_manager);

			m_device.set_allocator_alloc_type(l_alloc_type);
			
			auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();

			m_window_resize_handle = l_event_manager->register_event_listener<platform::bc_app_event_window_resize>
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_system::_event_handler)
			);
			m_app_active_handle = l_event_manager->register_event_listener<platform::bc_app_event_pause_state>
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_system::_event_handler)
			);
			m_device_reset_handle = l_event_manager->register_event_listener<graphic::bc_app_event_device_reset>
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_system::_event_handler)
			);
			m_frame_render_finish_handle = l_event_manager->register_event_listener<core::bc_event_frame_swap>
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_system::_event_handler)
			);

			bc_particle_manager::init_emitter_states();
		}

		void bc_render_system::_destroy()
		{
			bc_mesh_utility::clear_mesh_render_states_cache();
			bc_particle_manager::clear_emitter_states();
			
			destroy_render_passes();
			
			m_device_reset_handle.reset();
			m_app_active_handle.reset();
			m_window_resize_handle.reset();
			m_frame_render_finish_handle.reset();

			m_frame_renderer.reset();
			m_shape_drawer.reset();
			m_material_manager.reset();
			m_thread_manager.reset();
			
			m_render_pass_states.destroy();
			m_render_states.destroy();
			m_compute_states.destroy();
			
			m_device.destroy();
		}
		
		bool bc_render_system::_event_handler(core::bci_event& p_event)
		{
			auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();

			auto* l_window_resize_event = core::bci_message::as<platform::bc_app_event_window_resize>(p_event);
			if(l_window_resize_event)
			{
				// If nothing has change do not continue
				if (l_window_resize_event->width() != m_device.get_back_buffer_width() ||
					l_window_resize_event->height() != m_device.get_back_buffer_height())
				{
					const auto l_device_back_buffer = m_device.get_back_buffer_texture();

					graphic::bc_device_parameters l_old_parameters
					(
						l_device_back_buffer.get_width(),
						l_device_back_buffer.get_height(),
						l_device_back_buffer.get_format(),
						l_device_back_buffer.get_sample_count()
					);
					graphic::bc_device_parameters l_new_parameters
					(
						l_window_resize_event->width(),
						l_window_resize_event->height(),
						l_device_back_buffer.get_format(),
						l_device_back_buffer.get_sample_count()
					);

					// Put device reset event in render event queue
					l_event_manager->queue_event(graphic::bc_app_event_device_reset(m_device, l_old_parameters, l_new_parameters, true), 0);
				}

				return true;
			}

			auto* l_device_reset_event = core::bci_message::as<graphic::bc_app_event_device_reset>(p_event);
			if(l_device_reset_event)
			{
				m_device_reset_event.reset(*l_device_reset_event);
				
				return true;
			}

			auto* l_frame_swap_event = core::bci_message::as<core::bc_event_frame_swap>(p_event);
			if (l_frame_swap_event)
			{
				m_shape_drawer->clear_swap_buffers();

				return true;
			}
			
			return false;
		}
		
		void bc_render_system::_destroy_render_pass_state(bc_render_pass_state* p_render_pass_state)
		{
			BC_ASSERT(m_initialized);
			m_render_pass_states.free(p_render_pass_state);
		}

		void bc_render_system::_destroy_render_state(bc_render_state* p_render_state)
		{
			BC_ASSERT(m_initialized);
			m_render_states.free(p_render_state);
		}

		void bc_render_system::_destroy_compute_state(bc_compute_state* p_compute_state)
		{
			BC_ASSERT(m_initialized);
			m_compute_states.free(p_compute_state);
		}
	}
}