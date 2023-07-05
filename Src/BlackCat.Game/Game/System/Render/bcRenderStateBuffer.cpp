// [17/04/2020 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderStateBuffer.h"

namespace black_cat::game
{
	bc_render_state_buffer::bc_render_state_buffer() = default;

	bc_render_state_buffer::bc_render_state_buffer(bc_render_state_buffer&& p_other) noexcept
		: m_render_states(std::move(p_other.m_render_states)),
		  m_skinned_render_states(std::move(p_other.m_skinned_render_states)),
		  m_decals(std::move(p_other.m_decals))
	{
	}

	bc_render_state_buffer::~bc_render_state_buffer() = default;

	bc_render_state_buffer& bc_render_state_buffer::operator=(bc_render_state_buffer&& p_other) noexcept
	{
		m_render_states = std::move(p_other.m_render_states);
		m_skinned_render_states = std::move(p_other.m_skinned_render_states);
		m_decals = std::move(p_other.m_decals);
		return *this;
	}
}
