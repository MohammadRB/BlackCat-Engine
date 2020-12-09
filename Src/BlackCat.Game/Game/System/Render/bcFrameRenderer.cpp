// [04/03/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcEnumOperand.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
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
			core::bc_matrix4f m_projection;
			BC_CBUFFER_ALIGN
			core::bc_matrix4f m_view_projection;
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
		};

		struct _bc_render_system_per_object_cbuffer
		{
			BC_CBUFFER_ALIGN
			core::bc_matrix4f m_world_view_projection;
			BC_CBUFFER_ALIGN
			core::bc_matrix4f m_world;
		};
		
		bc_frame_renderer::bc_frame_renderer(graphic::bc_device& p_device, bc_render_thread_manager& p_thread_manager, bc_render_pass_manager& p_render_pass_manager) noexcept
			: m_thread_manager(&p_thread_manager),
			m_render_pass_manager(&p_render_pass_manager),
			m_prev_camera(nullptr),
			m_camera(nullptr)
		{
			auto l_global_cbuffer_config = graphic::bc_graphic_resource_builder()
				.as_resource()
				.as_buffer
				(
					1,
					sizeof(_bc_render_system_global_state_cbuffer),
					graphic::bc_resource_usage::gpu_rw,
					graphic::bc_resource_view_type::none
				)
				.as_constant_buffer();
			auto l_per_object_cbuffer_config = graphic::bc_graphic_resource_builder()
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
			m_global_cbuffer_parameter = graphic::bc_constant_buffer_parameter
			(
				g_render_pass_state_constant_buffer_min_index,
				core::bc_enum:: or
				({
					graphic::bc_shader_type::vertex,
					graphic::bc_shader_type::hull,
					graphic::bc_shader_type::domain,
					graphic::bc_shader_type::geometry,
					graphic::bc_shader_type::pixel,
					graphic::bc_shader_type::compute
					}),
				m_global_cbuffer.get()
			);
			m_per_object_cbuffer_parameter = graphic::bc_constant_buffer_parameter
			(
				g_render_state_constant_buffer_min_index,
				core::bc_enum:: or ({ graphic::bc_shader_type::vertex, graphic::bc_shader_type::hull, graphic::bc_shader_type::domain }),
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
			
			return *this;
		}

		const graphic::bc_constant_buffer_parameter& bc_frame_renderer::get_global_cbuffer() const noexcept
		{
			return m_global_cbuffer_parameter;
		}

		const graphic::bc_constant_buffer_parameter& bc_frame_renderer::get_per_object_cbuffer() const noexcept
		{
			return m_per_object_cbuffer_parameter;
		}

		bc_render_thread_manager& bc_frame_renderer::get_thread_manager() noexcept
		{
			return *m_thread_manager;
		}

		void bc_frame_renderer::update_global_cbuffer(bc_render_thread& p_render_thread, const core_platform::bc_clock::update_param& p_clock, const bc_camera_instance& p_camera)
		{
			_bc_render_system_global_state_cbuffer l_global_state;
			l_global_state.m_view = p_camera.get_view().transpose();
			l_global_state.m_projection = p_camera.get_projection().transpose();
			l_global_state.m_view_projection = (p_camera.get_view() * p_camera.get_projection()).transpose();
			l_global_state.m_screen_width = p_camera.get_screen_width();
			l_global_state.m_screen_height = p_camera.get_screen_height();
			l_global_state.m_near_plan = p_camera.get_near_clip();
			l_global_state.m_far_plan = p_camera.get_far_clip();
			l_global_state.m_camera_position = p_camera.get_position();
			l_global_state.m_total_elapsed = p_clock.m_total_elapsed;
			l_global_state.m_total_elapsed_second = p_clock.m_total_elapsed_second;
			l_global_state.m_elapsed = p_clock.m_elapsed;
			l_global_state.m_elapsed_second = p_clock.m_elapsed_second;

			graphic::bc_buffer l_buffer = m_global_cbuffer_parameter.get_buffer();
			p_render_thread.update_subresource(l_buffer, 0, &l_global_state, 0, 0);
		}
		
		bc_render_state_buffer bc_frame_renderer::create_buffer() const
		{
			return bc_render_state_buffer();
		}

		void bc_frame_renderer::render_buffer(bc_render_thread& p_render_thread, const bc_render_state_buffer& p_buffer, const bc_camera_instance& p_camera)
		{
			const auto l_view_proj = p_camera.get_view() * p_camera.get_projection();

			for (auto& l_render_state_entry : p_buffer)
			{
				if (l_render_state_entry.second.empty())
				{
					continue;
				}

				auto& l_render_state = *l_render_state_entry.first;
				p_render_thread.bind_render_state(l_render_state);

				for (auto& l_render_instance : l_render_state_entry.second)
				{
					_bc_render_system_per_object_cbuffer l_per_object_cbuffer
					{
						(l_render_instance.get_world() * l_view_proj).transpose(),
						l_render_instance.get_world().transpose()
					};

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
		
		void bc_frame_renderer::update(const bc_frame_renderer_update_param& p_update_param)
		{
			if(m_camera_instance.is_set())
			{
				m_prev_camera_instance.reset(m_camera_instance.get());
				m_prev_camera.store(&m_prev_camera_instance.get(), core_platform::bc_memory_order::release);
			}
			
			m_camera_instance.reset(p_update_param.m_camera);
			m_camera.store(&m_camera_instance.get(), core_platform::bc_memory_order::release);
			
			m_render_pass_manager->pass_update(bc_render_pass_update_param(p_update_param.m_clock, p_update_param.m_camera));
		}

		void bc_frame_renderer::render(const bc_frame_renderer_render_param& p_render_param)
		{
			auto* l_prev_camera = m_prev_camera.load(core_platform::bc_memory_order::consume);
			auto* l_camera = m_camera.load(core_platform::bc_memory_order::consume);

			if(!l_prev_camera)
			{
				return;
			}
			
			m_render_pass_manager->pass_execute(bc_render_pass_render_param
			(
				p_render_param.m_clock,
				*l_camera,
				*l_prev_camera,
				p_render_param.m_render_system,
				*this,
				m_thread_manager->get_default_render_thread()
			));
		}
	}
}