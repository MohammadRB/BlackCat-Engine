// [04/03/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcEnumOperand.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderThreadManager.h"
#include "Game/System/Render/Pass/bcRenderPassManager.h"
#include "Game/System/Render/bcFrameRenderer.h"
#include "Game/System/Render/bcRenderStateBuffer.h"

namespace black_cat
{
	namespace game
	{
		struct _bc_render_system_global_state_cbuffer
		{
			BC_CBUFFER_ALIGN
			core::bc_matrix4f m_view;
			BC_CBUFFER_ALIGN
			core::bc_matrix4f m_view_inv;
			BC_CBUFFER_ALIGN
			core::bc_matrix4f m_projection;
			BC_CBUFFER_ALIGN
			core::bc_matrix4f m_view_projection;
			BC_CBUFFER_ALIGN
			core::bc_matrix4f m_view_projection_inv;
			BC_CBUFFER_ALIGN
			bcUINT32 m_screen_width;
			bcUINT32 m_screen_height;
			bcFLOAT m_near_plan;
			bcFLOAT m_far_plan;
			BC_CBUFFER_ALIGN
			core::bc_vector3f m_camera_position;
			BC_CBUFFER_ALIGN
			bcDOUBLE m_total_elapsed;
			bcDOUBLE m_total_elapsed_second;
			BC_CBUFFER_ALIGN
			bcFLOAT m_elapsed;
			bcFLOAT m_elapsed_second;
			BC_CBUFFER_ALIGN
			bcFLOAT m_global_scale;
			core::bc_vector3f m_global_light_direction;
			BC_CBUFFER_ALIGN
			core::bc_vector3f m_global_light_color;
			bcFLOAT m_global_light_intensity;
			BC_CBUFFER_ALIGN
			core::bc_vector3f m_global_light_ambient_color;
			bcFLOAT m_global_light_ambient_intensity;
			BC_CBUFFER_ALIGN
			core::bc_vector3f m_global_wind_direction;
			bcFLOAT m_global_wind_power;
		};
		
		static constexpr bcSIZE g_max_skinned_transform = 90;

		struct _bc_render_system_per_object_cbuffer
		{
			BC_CBUFFER_ALIGN
			core::bc_matrix4f m_world_view_projection;
			BC_CBUFFER_ALIGN
			core::bc_matrix4f m_world;
			BC_CBUFFER_ALIGN
			core::bc_matrix4f m_transforms[g_max_skinned_transform];
		};

		bc_frame_renderer::bc_frame_renderer(graphic::bc_device& p_device, bc_render_thread_manager& p_thread_manager, bc_render_pass_manager& p_render_pass_manager) noexcept
			: m_thread_manager(&p_thread_manager),
			m_render_pass_manager(&p_render_pass_manager),
			m_update_camera(),
			m_render_camera(),
			m_update_camera_instance(nullptr),
			m_render_camera_instance(nullptr),
			m_render_thread_update_camera(),
			m_render_thread_render_camera()
		{
			const auto l_global_cbuffer_config = graphic::bc_graphic_resource_builder()
			                                     .as_resource()
			                                     .as_buffer
			                                     (
				                                     1,
				                                     sizeof(_bc_render_system_global_state_cbuffer),
				                                     graphic::bc_resource_usage::gpu_rw,
				                                     graphic::bc_resource_view_type::none
			                                     )
			                                     .as_constant_buffer();
			const auto l_per_object_cbuffer_config = graphic::bc_graphic_resource_builder()
			                                         .as_resource()
			                                         .as_buffer
			                                         (
				                                         1,
				                                         sizeof(_bc_render_system_per_object_cbuffer),
				                                         graphic::bc_resource_usage::gpu_rw,
				                                         graphic::bc_resource_view_type::none
			                                         )
			                                         .as_constant_buffer();

			m_global_cbuffer = p_device.create_buffer(l_global_cbuffer_config, nullptr);
			m_per_object_cbuffer = p_device.create_buffer(l_per_object_cbuffer_config, nullptr);

			m_global_cbuffer->set_debug_name("global_cbuffer");
			m_per_object_cbuffer->set_debug_name("object_cbuffer");

			m_global_cbuffer_parameter = graphic::bc_constant_buffer_parameter
			(
				g_render_pass_state_constant_buffer_min_index,
				core::bc_enum::mask_or
				(
					{
						graphic::bc_shader_type::vertex,
						graphic::bc_shader_type::hull,
						graphic::bc_shader_type::domain,
						graphic::bc_shader_type::geometry,
						graphic::bc_shader_type::pixel,
						graphic::bc_shader_type::compute
					}
				),
				m_global_cbuffer.get()
			);
			m_per_object_cbuffer_parameter = graphic::bc_constant_buffer_parameter
			(
				g_render_state_constant_buffer_min_index,
				core::bc_enum::mask_or
				({ graphic::bc_shader_type::vertex, graphic::bc_shader_type::hull, graphic::bc_shader_type::domain }),
				m_per_object_cbuffer.get()
			);
		}

		bc_frame_renderer::bc_frame_renderer(bc_frame_renderer&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_frame_renderer::~bc_frame_renderer() = default;

		bc_frame_renderer& bc_frame_renderer::operator=(bc_frame_renderer&& p_other) noexcept
		{
			m_thread_manager = p_other.m_thread_manager;
			m_render_pass_manager = p_other.m_render_pass_manager;

			m_global_cbuffer = std::move(p_other.m_global_cbuffer);
			m_per_object_cbuffer = std::move(p_other.m_per_object_cbuffer);
			m_global_cbuffer_parameter = std::move(p_other.m_global_cbuffer_parameter);
			m_per_object_cbuffer_parameter = std::move(p_other.m_per_object_cbuffer_parameter);

			m_render_camera = p_other.m_render_camera;
			m_update_camera = p_other.m_update_camera;
			m_render_camera_instance.store(p_other.m_render_camera_instance.load());
			m_update_camera_instance.store(p_other.m_update_camera_instance.load());
			
			return *this;
		}

		void bc_frame_renderer::update_global_cbuffer(bc_render_thread& p_render_thread,
			const platform::bc_clock::update_param& p_clock,
			const bc_camera_instance& p_camera)
		{
			_bc_render_system_global_state_cbuffer l_global_cbuffer_data;
			l_global_cbuffer_data.m_view = p_camera.get_view();
			l_global_cbuffer_data.m_view_inv = p_camera.get_view().inverse();
			l_global_cbuffer_data.m_projection = p_camera.get_projection();
			l_global_cbuffer_data.m_view_projection = p_camera.get_view() * p_camera.get_projection();
			l_global_cbuffer_data.m_view_projection_inv = l_global_cbuffer_data.m_view_projection.inverse();
			l_global_cbuffer_data.m_screen_width = p_camera.get_screen_width();
			l_global_cbuffer_data.m_screen_height = p_camera.get_screen_height();
			l_global_cbuffer_data.m_near_plan = p_camera.get_near_clip();
			l_global_cbuffer_data.m_far_plan = p_camera.get_far_clip();
			l_global_cbuffer_data.m_camera_position = p_camera.get_position();
			l_global_cbuffer_data.m_total_elapsed = p_clock.m_total_elapsed;
			l_global_cbuffer_data.m_total_elapsed_second = p_clock.m_total_elapsed_second;
			l_global_cbuffer_data.m_elapsed = p_clock.m_elapsed;
			l_global_cbuffer_data.m_elapsed_second = p_clock.m_elapsed_second;
			l_global_cbuffer_data.m_global_scale = bc_get_global_config().get_scene_global_scale();
			l_global_cbuffer_data.m_global_light_direction = m_global_cbuffer_data.m_global_light.get_direction();
			l_global_cbuffer_data.m_global_light_color = m_global_cbuffer_data.m_global_light.get_color();
			l_global_cbuffer_data.m_global_light_intensity = m_global_cbuffer_data.m_global_light.get_intensity();
			l_global_cbuffer_data.m_global_light_ambient_color = m_global_cbuffer_data.m_global_light.get_ambient_color();
			l_global_cbuffer_data.m_global_light_ambient_intensity = m_global_cbuffer_data.m_global_light.get_ambient_intensity();
			l_global_cbuffer_data.m_global_wind_direction = m_global_cbuffer_data.m_global_wind.m_direction;
			l_global_cbuffer_data.m_global_wind_power = m_global_cbuffer_data.m_global_wind.m_power;

			if constexpr (need_matrix_transpose())
			{
				l_global_cbuffer_data.m_view.make_transpose();
				l_global_cbuffer_data.m_view_inv.make_transpose();
				l_global_cbuffer_data.m_projection.make_transpose();
				l_global_cbuffer_data.m_view_projection.make_transpose();
				l_global_cbuffer_data.m_view_projection_inv.make_transpose();
			}

			graphic::bc_buffer l_buffer = m_global_cbuffer_parameter.get_buffer();
			p_render_thread.update_subresource(l_buffer, 0, &l_global_cbuffer_data, 0, 0);
		}

		void bc_frame_renderer::update_global_cbuffer(bc_render_thread& p_render_thread,
			const platform::bc_clock::update_param& p_clock,
			const bc_camera_instance& p_camera,
			const bc_direct_light& p_global_light,
			const bc_direct_wind& p_global_wind)
		{
			m_global_cbuffer_data.m_global_light = p_global_light;
			m_global_cbuffer_data.m_global_wind = p_global_wind;

			_bc_render_system_global_state_cbuffer l_global_cbuffer_data;
			l_global_cbuffer_data.m_view = p_camera.get_view();
			l_global_cbuffer_data.m_view_inv = p_camera.get_view().inverse();
			l_global_cbuffer_data.m_projection = p_camera.get_projection();
			l_global_cbuffer_data.m_view_projection = p_camera.get_view() * p_camera.get_projection();
			l_global_cbuffer_data.m_view_projection_inv = l_global_cbuffer_data.m_view_projection.inverse();
			l_global_cbuffer_data.m_screen_width = p_camera.get_screen_width();
			l_global_cbuffer_data.m_screen_height = p_camera.get_screen_height();
			l_global_cbuffer_data.m_near_plan = p_camera.get_near_clip();
			l_global_cbuffer_data.m_far_plan = p_camera.get_far_clip();
			l_global_cbuffer_data.m_camera_position = p_camera.get_position();
			l_global_cbuffer_data.m_total_elapsed = p_clock.m_total_elapsed;
			l_global_cbuffer_data.m_total_elapsed_second = p_clock.m_total_elapsed_second;
			l_global_cbuffer_data.m_elapsed = p_clock.m_elapsed;
			l_global_cbuffer_data.m_elapsed_second = p_clock.m_elapsed_second;
			l_global_cbuffer_data.m_global_scale = bc_get_global_config().get_scene_global_scale();
			l_global_cbuffer_data.m_global_light_direction = m_global_cbuffer_data.m_global_light.get_direction();
			l_global_cbuffer_data.m_global_light_color = m_global_cbuffer_data.m_global_light.get_color();
			l_global_cbuffer_data.m_global_light_intensity = m_global_cbuffer_data.m_global_light.get_intensity();
			l_global_cbuffer_data.m_global_light_ambient_color = m_global_cbuffer_data.m_global_light.get_ambient_color();
			l_global_cbuffer_data.m_global_light_ambient_intensity = m_global_cbuffer_data.m_global_light.get_ambient_intensity();
			l_global_cbuffer_data.m_global_wind_direction = m_global_cbuffer_data.m_global_wind.m_direction;
			l_global_cbuffer_data.m_global_wind_power = m_global_cbuffer_data.m_global_wind.m_power;

			if constexpr (need_matrix_transpose())
			{
				l_global_cbuffer_data.m_view.make_transpose();
				l_global_cbuffer_data.m_view_inv.make_transpose();
				l_global_cbuffer_data.m_projection.make_transpose();
				l_global_cbuffer_data.m_view_projection_inv.make_transpose();
			}
			
			graphic::bc_buffer l_buffer = m_global_cbuffer_parameter.get_buffer();
			p_render_thread.update_subresource(l_buffer, 0, &l_global_cbuffer_data, 0, 0);
		}

		void bc_frame_renderer::update_per_object_cbuffer(bc_render_thread& p_render_thread, const bc_camera_instance& p_camera, const bc_render_instance& p_instance)
		{
			_bc_render_system_per_object_cbuffer l_per_object_cbuffer;
			l_per_object_cbuffer.m_world_view_projection = p_instance.get_transform() * p_camera.get_view() * p_camera.get_projection();
			l_per_object_cbuffer.m_world = p_instance.get_transform();

			if constexpr (need_matrix_transpose())
			{
				l_per_object_cbuffer.m_world_view_projection.make_transpose();
				l_per_object_cbuffer.m_world.make_transpose();
			}

			graphic::bc_buffer l_buffer = m_per_object_cbuffer_parameter.get_buffer();
			p_render_thread.update_subresource(l_buffer, 0, &l_per_object_cbuffer, 0, 0);
		}

		void bc_frame_renderer::update_per_skinned_object_cbuffer(bc_render_thread& p_render_thread, const bc_camera_instance& p_camera, const bc_skinned_render_instance& p_instance)
		{
			BC_ASSERT(p_instance.get_num_transforms() <= g_max_skinned_transform);
			
			_bc_render_system_per_object_cbuffer l_per_object_cbuffer;
			std::memcpy(&l_per_object_cbuffer.m_transforms[0], p_instance.get_transforms(), sizeof(core::bc_matrix4f) * p_instance.get_num_transforms());

			if constexpr (need_matrix_transpose())
			{
				for (auto& l_transform : l_per_object_cbuffer.m_transforms)
				{
					l_transform.make_transpose();
				}
			}

			graphic::bc_buffer l_buffer = m_per_object_cbuffer_parameter.get_buffer();
			p_render_thread.update_subresource(l_buffer, 0, &l_per_object_cbuffer, 0, 0);
		}

		bc_render_state_buffer bc_frame_renderer::create_buffer() const
		{
			return bc_render_state_buffer();
		}

		void bc_frame_renderer::render_buffer(bc_render_thread& p_render_thread, const bc_render_state_buffer& p_buffer, const bc_camera_instance& p_camera)
		{
			_bc_render_system_per_object_cbuffer l_per_object_cbuffer;
			const auto l_view_proj = p_camera.get_view() * p_camera.get_projection();
			auto l_last_render_group = bc_actor_render_group::unknown;
			
			for (const auto& l_render_state_entry : p_buffer.get_instances())
			{
				if (l_render_state_entry.second.empty())
				{
					continue;
				}

				auto& l_render_state = *l_render_state_entry.first;
				p_render_thread.bind_render_state(l_render_state);
				
				for (const bc_render_instance& l_render_instance : l_render_state_entry.second)
				{
					if(l_render_instance.get_render_group() != l_last_render_group)
					{
						l_last_render_group = l_render_instance.get_render_group();
						p_render_thread.bind_om_stencil_ref(static_cast<bcUINT32>(l_last_render_group));
						p_render_thread.pipeline_apply_states(graphic::bc_pipeline_stage::output_merger_stage);
					}
					
					l_per_object_cbuffer.m_world_view_projection = l_render_instance.get_transform() * l_view_proj;
					l_per_object_cbuffer.m_world = l_render_instance.get_transform();

					if constexpr (need_matrix_transpose())
					{
						l_per_object_cbuffer.m_world_view_projection.make_transpose();
						l_per_object_cbuffer.m_world.make_transpose();
					}
					
					graphic::bc_buffer l_buffer = m_per_object_cbuffer_parameter.get_buffer();
					p_render_thread.update_subresource(l_buffer, 0, &l_per_object_cbuffer, 0, 0);

					p_render_thread.draw_indexed
					(
						l_render_state_entry.first->get_index_buffer_offset(),
						l_render_state_entry.first->get_index_count(),
						l_render_state_entry.first->get_vertex_buffer_offset()
					);
				}

				p_render_thread.unbind_render_state(l_render_state);
			}
		}

		void bc_frame_renderer::render_skinned_buffer(bc_render_thread& p_render_thread, const bc_render_state_buffer& p_buffer, const bc_camera_instance& p_camera)
		{
			_bc_render_system_per_object_cbuffer l_per_object_cbuffer;
			auto l_last_render_group = game::bc_actor_render_group::unknown;
			
			for (const auto& l_render_state_entry : p_buffer.get_skinned_instances())
			{
				if (l_render_state_entry.second.empty())
				{
					continue;
				}

				auto& l_render_state = *l_render_state_entry.first;
				p_render_thread.bind_render_state(l_render_state);

				for (bc_skinned_render_instance& l_render_instance : l_render_state_entry.second)
				{
					BC_ASSERT(l_render_instance.get_num_transforms() <= g_max_skinned_transform);

					if (l_render_instance.get_render_group() != l_last_render_group)
					{
						l_last_render_group = l_render_instance.get_render_group();
						p_render_thread.bind_om_stencil_ref(static_cast<bcUINT32>(l_last_render_group));
						p_render_thread.pipeline_apply_states(graphic::bc_pipeline_stage::output_merger_stage);
					}
					
					std::memcpy(&l_per_object_cbuffer.m_transforms[0], l_render_instance.get_transforms(), sizeof(core::bc_matrix4f) * l_render_instance.get_num_transforms());

					if constexpr (need_matrix_transpose())
					{
						for (auto& l_transform : l_per_object_cbuffer.m_transforms)
						{
							l_transform.make_transpose();
						}
					}

					graphic::bc_buffer l_buffer = m_per_object_cbuffer_parameter.get_buffer();
					p_render_thread.update_subresource(l_buffer, 0, &l_per_object_cbuffer, 0, 0);

					p_render_thread.draw_indexed
					(
						l_render_state_entry.first->get_index_buffer_offset(),
						l_render_state_entry.first->get_index_count(),
						l_render_state_entry.first->get_vertex_buffer_offset()
					);
				}

				p_render_thread.unbind_render_state(l_render_state);
			}
		}

		void bc_frame_renderer::update(const update_context& p_update_param)
		{
			if(m_update_camera_instance.load(platform::bc_memory_order::relaxed))
			{
				m_render_camera = m_update_camera;
				m_render_camera_instance.store(&m_render_camera, platform::bc_memory_order::relaxed);
			}
			
			m_update_camera = p_update_param.m_camera;
			m_update_camera_instance.store(&m_update_camera, platform::bc_memory_order::relaxed);
			
			m_render_pass_manager->pass_update(bc_render_pass_update_context(p_update_param.m_clock, p_update_param.m_camera));
		}

		void bc_frame_renderer::render(const render_context& p_render_param)
		{
			const auto* l_update_camera = m_update_camera_instance.load(platform::bc_memory_order::consume);
			const auto* l_render_camera = m_render_camera_instance.load(platform::bc_memory_order::consume);

			if(!l_render_camera)
			{
				return;
			}

			m_render_thread_update_camera = *l_update_camera;
			m_render_thread_render_camera = *l_render_camera;
			
			m_render_pass_manager->pass_execute(bc_render_pass_render_context
			(
				p_render_param.m_clock,
				p_render_param.m_query_manager,
				p_render_param.m_render_system,
				*this,
				m_thread_manager->get_default_render_thread(),
				m_render_thread_update_camera,
				m_render_thread_render_camera
			));
		}
	}
}