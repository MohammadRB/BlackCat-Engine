// [04/17/2020 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Input/bcCamera.h"
#include "Game/System/Render/bcRenderThread.h"
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

		bc_render_state_buffer::iterator bc_render_state_buffer::begin()
		{
			return m_render_states.begin();
		}

		bc_render_state_buffer::const_iterator bc_render_state_buffer::begin() const
		{
			return m_render_states.begin();
		}

		bc_render_state_buffer::const_iterator bc_render_state_buffer::cbegin() const
		{
			return m_render_states.cbegin();
		}

		bc_render_state_buffer::iterator bc_render_state_buffer::end()
		{
			return m_render_states.end();
		}

		bc_render_state_buffer::const_iterator bc_render_state_buffer::end() const
		{
			return m_render_states.end();
		}

		bc_render_state_buffer::const_iterator bc_render_state_buffer::cend() const
		{
			return m_render_states.cend();
		}

		void bc_render_state_buffer::add_render_instance(bc_render_state_ptr p_render_state, const bc_render_instance& p_instance)
		{
			auto& l_entry = m_render_states[std::move(p_render_state)];

			l_entry.push_back(p_instance);
		}
	}
}