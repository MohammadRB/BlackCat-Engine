// [12/06/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/State/bcRenderState.h"

namespace black_cat
{
	namespace game
	{
		_bc_render_state_handle_deleter::_bc_render_state_handle_deleter()
			: m_render_system(nullptr) 
		{
		}

		_bc_render_state_handle_deleter::_bc_render_state_handle_deleter(bc_render_system* p_render_system)
			: m_render_system(p_render_system)
		{
		}

		void _bc_render_state_handle_deleter::operator()(bc_render_state* p_render_state) const
		{
			BC_ASSERT(m_render_system != nullptr);

			m_render_system->_destroy_render_state(p_render_state);
		}

		bc_render_state::bc_render_state(bc_render_state&& p_other) noexcept
			: bc_ref_count(std::move(p_other)),
			m_primitive(p_other.m_primitive),
			m_vertex_buffer(p_other.m_vertex_buffer),
			m_vertex_buffer_stride(p_other.m_vertex_buffer_stride),
			m_vertex_buffer_offset(p_other.m_vertex_buffer_offset),
			m_index_buffer(p_other.m_index_buffer),
			m_index_type(p_other.m_index_type),
			m_index_count(p_other.m_index_count),
			m_index_buffer_offset(p_other.m_index_buffer_offset),
			m_resource_views(std::move(p_other.m_resource_views)),
			m_shader_cbuffers(std::move(p_other.m_shader_cbuffers))
		{
		}

		bc_render_state& bc_render_state::operator=(bc_render_state&& p_other) noexcept
		{
			bc_ref_count::operator=(std::move(p_other));
			m_primitive = p_other.m_primitive;
			m_vertex_buffer = p_other.m_vertex_buffer;
			m_vertex_buffer_stride = p_other.m_vertex_buffer_stride;
			m_vertex_buffer_offset = p_other.m_vertex_buffer_offset;
			m_index_buffer = p_other.m_index_buffer;
			m_index_type = p_other.m_index_type;
			m_index_count = p_other.m_index_count;
			m_index_buffer_offset = p_other.m_index_buffer_offset;
			m_resource_views = std::move(p_other.m_resource_views);
			m_shader_cbuffers = std::move(p_other.m_shader_cbuffers);

			return *this;
		}

		bc_render_state::bc_render_state(graphic::bc_primitive p_primitive,
			graphic::bc_buffer p_vertex_buffer,
			bcUINT32 p_vertex_buffer_stride,
			bcUINT32 p_vertex_buffer_offset,
			graphic::bc_buffer p_index_buffer,
			bc_index_type p_index_type,
			bcUINT32 p_index_count,
			bcUINT32 p_index_buffer_offset,
			bc_render_state_resource_view_array&& p_shader_views,
			bc_render_state_constant_buffer_array&& p_shader_buffers)
			: m_primitive(p_primitive),
			m_vertex_buffer(p_vertex_buffer),
			m_vertex_buffer_stride(p_vertex_buffer_stride),
			m_vertex_buffer_offset(p_vertex_buffer_offset),
			m_index_buffer(p_index_buffer),
			m_index_type(p_index_type),
			m_index_count(p_index_count),
			m_index_buffer_offset(p_index_buffer_offset),
			m_resource_views(std::move(p_shader_views)),
			m_shader_cbuffers(std::move(p_shader_buffers))
		{
		}
	}
}