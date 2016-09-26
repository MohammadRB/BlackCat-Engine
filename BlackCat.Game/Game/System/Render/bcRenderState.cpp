// [06/12/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderState.h"

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

		bc_render_state* _bc_render_state_handle_deleter::operator()(handle_t p_handle) const
		{
			bcAssert(m_render_system != nullptr);

			auto& l_render_state = m_render_system->m_render_states.at(p_handle.m_index);
			
			if(l_render_state.first.is_set())
			{
				return &l_render_state.first.get();
			}

			return nullptr;
		}

		void _bc_render_state_handle_deleter::operator()(bc_render_state* p_render_state) const
		{
			bcAssert(m_render_system != nullptr);

			m_render_system->destroy_render_state(p_render_state);
		}

		bc_render_state::bc_render_state(graphic::bc_primitive p_primitive,
			graphic::bc_buffer_ptr& p_vertex_buffer,
			bcUINT32 p_verext_buffer_offset,
			graphic::bc_buffer_ptr& p_index_buffer,
			bc_index_type p_index_type,
			bcUINT32 p_index_count,
			bcUINT32 p_index_buffer_offset,
			bc_render_state_resource_view_array&& p_shader_views,
			bc_render_state_constant_buffer_array&& p_shader_buffers)
			: m_primitive(p_primitive),
			m_vertex_buffer(p_vertex_buffer),
			m_vertex_buffer_offset(p_verext_buffer_offset),
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