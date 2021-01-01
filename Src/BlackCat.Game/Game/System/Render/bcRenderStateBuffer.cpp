// [04/17/2020 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderStateBuffer.h"

namespace black_cat
{
	namespace game
	{
		bc_render_state_buffer::bc_render_state_buffer() = default;

		bc_render_state_buffer::bc_render_state_buffer(bc_render_state_buffer&& p_other) noexcept
			: m_render_states(std::move(p_other.m_render_states))
		{
		}

		bc_render_state_buffer::~bc_render_state_buffer() = default;

		bc_render_state_buffer& bc_render_state_buffer::operator=(bc_render_state_buffer&& p_other) noexcept
		{
			m_render_states = std::move(p_other.m_render_states);
			return *this;
		}

		bc_render_state_buffer::instance_iterator bc_render_state_buffer::get_instances() const noexcept
		{
			return instance_iterator(m_render_states);
		}

		bc_render_state_buffer::skinned_instance_iterator bc_render_state_buffer::get_skinned_instances() const noexcept
		{
			return skinned_instance_iterator(m_skinned_render_states);
		}

		void bc_render_state_buffer::add_render_instance(bc_render_state_ptr p_render_state, const bc_render_instance& p_instance)
		{
			auto& l_entry = m_render_states[std::move(p_render_state)];
			l_entry.push_back(p_instance);
		}

		void bc_render_state_buffer::add_skinned_render_instance(bc_render_state_ptr p_render_state, bc_skinned_render_instance p_instance)
		{
			auto& l_entry = m_skinned_render_states[std::move(p_render_state)];
			l_entry.push_back(std::move(p_instance));
		}
	}
}