// [12/04/2018 MRB]

#include "Game/GamePCH.h"

#include "GraphicImp/Resource/Buffer/bcBufferConfig.h"
#include "GraphicImp/Resource/bcResourceConfig.h"
#include "Game/System/Render/bcShapeDrawer.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/Object/Mesh/bcShapeGenerator.h"

namespace black_cat
{
	namespace game
	{
		bc_shape_drawer::bc_shape_drawer()
			: m_last_frame_indices_count(0)
		{
			m_vertices.reserve(1024);
			m_indices.reserve(1024);
		}

		bc_shape_drawer::bc_shape_drawer(bc_shape_drawer&& p_other) noexcept
			: m_vertices(std::move(p_other.m_vertices)),
			m_indices(std::move(p_other.m_indices)),
			m_vb(std::move(p_other.m_vb)),
			m_ib(std::move(p_other.m_ib)),
			m_last_frame_indices_count(p_other.m_last_frame_indices_count),
			m_render_state(std::move(p_other.m_render_state))
		{
		}

		bc_shape_drawer::~bc_shape_drawer()
		{
		}

		bc_shape_drawer& bc_shape_drawer::operator=(bc_shape_drawer&& p_other) noexcept
		{
			m_vertices = std::move(p_other.m_vertices);
			m_indices = std::move(p_other.m_indices);
			m_vb = std::move(p_other.m_vb);
			m_ib = std::move(p_other.m_ib);
			m_last_frame_indices_count = p_other.m_last_frame_indices_count;
			m_render_state = std::move(p_other.m_render_state);

			return *this;
		}

		void bc_shape_drawer::render_wired_box(const physics::bc_bound_box& p_box)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_mutex);

				auto l_vertices_count = m_vertices.size();
				bc_shape_generator_buffer l_buffer(m_vertices, m_indices);
				bc_shape_generator::create_wired_box(l_buffer, p_box);
			}
		}

		void bc_shape_drawer::update_buffers(bc_render_system& p_render_system, bc_render_thread& p_thread)
		{
			using vertices_size_type = core::bc_vector_movale<core::bc_vector3f>::size_type;
			using indices_size_type = core::bc_vector_movale<bcUINT32>::size_type;

			vertices_size_type l_vertices_count, l_vertices_capacity;
			indices_size_type l_indices_count, l_indices_capacity;
			core::bc_vector3f* l_vertices_data;
			bcUINT32* l_indices_data;

			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_mutex);

				l_vertices_count = m_vertices.size();
				l_indices_count = m_indices.size();
				l_vertices_capacity = m_vertices.capacity();
				l_indices_capacity = m_indices.capacity();
				l_vertices_data = m_vertices.data();
				l_indices_data = m_indices.data();
			}

			auto& l_device = p_render_system.get_device();
			const auto l_vb_size = m_vb->is_valid() ? m_vb->get_byte_width() : 0;
			const auto l_ib_size = m_ib->is_valid() ? m_ib->get_byte_width() : 0;
			bool l_requires_render_state_update = l_indices_count != m_last_frame_indices_count;

			if (l_vertices_count == 0 || l_indices_count == 0)
			{
				return;
			}

			if (l_vb_size < l_vertices_count * sizeof(core::bc_vector3f))
			{
				graphic::bc_buffer_config l_vb_config = graphic::bc_resource_configure().as_buffer
				(
					l_vertices_capacity,
					sizeof(core::bc_vector3f),
					graphic::bc_resource_usage::gpu_rw
				).as_vertex_buffer();
				graphic::bc_subresource_data l_vb_data(l_vertices_data, 0, 0);
				m_vb = l_device.create_buffer(l_vb_config, &l_vb_data);
				l_requires_render_state_update = true;
			}
			else
			{
				p_thread.update_subresource(m_vb.get(), 0, l_vertices_data, 0, 0);
			}

			if (l_ib_size < l_indices_count * sizeof(bcUINT32))
			{
				graphic::bc_buffer_config l_ib_config = graphic::bc_resource_configure().as_buffer
				(
					l_indices_capacity,
					sizeof(bcUINT32),
					graphic::bc_resource_usage::gpu_rw
				).as_index_buffer();
				graphic::bc_subresource_data l_ib_data(l_indices_data, 0, 0);
				m_ib = l_device.create_buffer(l_ib_config, &l_ib_data);
				l_requires_render_state_update = true;
			}
			else
			{
				p_thread.update_subresource(m_ib.get(), 0, l_indices_data, 0, 0);
			}

			if (l_requires_render_state_update)
			{
				m_render_state = p_render_system.create_render_state
				(
					graphic::bc_primitive::linelist,
					m_vb.get(),
					sizeof(core::bc_vector3f),
					0,
					m_ib.get(),
					i32bit,
					l_indices_count,
					0,
					{},
					{}
				);
			}

			m_last_frame_indices_count = l_indices_count;
		}

		void bc_shape_drawer::render(bc_render_system& p_render_system, bc_render_thread& p_thread)
		{
			update_buffers(p_render_system, p_thread);

			if(!m_render_state)
			{
				return;
			}

			const bc_render_instance l_instance(core::bc_matrix4f::identity());
			p_render_system.add_render_instance(*m_render_state.get(), l_instance);
		}

		void bc_shape_drawer::clear_buffers()
		{
			m_vertices.clear();
			m_indices.clear();
		}

		void bc_shape_drawer::destroy_buffers()
		{
			clear_buffers();
			m_vertices.shrink_to_fit();
			m_indices.shrink_to_fit();
			m_vb.reset();
			m_ib.reset();
			m_render_state.reset();
		}
	}
}