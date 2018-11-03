// [09/06/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcComputeState.h"

namespace black_cat
{
	namespace game
	{
		_bc_compute_state_handle_deleter::_bc_compute_state_handle_deleter()
			: m_render_system(nullptr)
		{
		}

		_bc_compute_state_handle_deleter::_bc_compute_state_handle_deleter(bc_render_system* p_render_system)
			: m_render_system(p_render_system)
		{
		}

		bc_compute_state* _bc_compute_state_handle_deleter::operator()(handle_t p_handle) const
		{
			bcAssert(m_render_system != nullptr);

			auto& l_render_state = m_render_system->m_compute_states.at(p_handle.m_index);

			if (l_render_state.is_set())
			{
				return &l_render_state.get();
			}

			return nullptr;
		}

		void _bc_compute_state_handle_deleter::operator()(bc_compute_state* p_compute_state) const
		{
			bcAssert(m_render_system != nullptr);

			m_render_system->destroy_compute_state(p_compute_state);
		}

		bc_compute_state::bc_compute_state(bc_compute_state&& p_other) noexcept
			: m_compute_state(p_other.m_compute_state),
			m_dispatch_x(p_other.m_dispatch_x),
			m_dispatch_y(p_other.m_dispatch_y),
			m_dispatch_z(p_other.m_dispatch_z),
			m_samplers(std::move(p_other.m_samplers)),
			m_resource_views(std::move(p_other.m_resource_views)),
			m_unordered_views(std::move(p_other.m_unordered_views)),
			m_cbuffers(std::move(p_other.m_cbuffers))
		{	
		}

		bc_compute_state& bc_compute_state::operator=(bc_compute_state&& p_other) noexcept
		{
			m_compute_state = p_other.m_compute_state;
			m_dispatch_x = p_other.m_dispatch_x;
			m_dispatch_y = p_other.m_dispatch_y;
			m_dispatch_z = p_other.m_dispatch_z;
			m_samplers = std::move(p_other.m_samplers);
			m_resource_views = std::move(p_other.m_resource_views);
			m_unordered_views = std::move(p_other.m_unordered_views);
			m_cbuffers = std::move(p_other.m_cbuffers);

			return *this;
		}

		bc_compute_state::bc_compute_state(graphic::bc_device_compute_state p_compute_state,
			bcUINT32 p_dispatch_x,
			bcUINT32 p_dispatch_y,
			bcUINT32 p_dispatch_z,
			bc_compute_state_sampler_array&& p_samplers,
			bc_compute_state_resource_view_array&& p_shader_views,
			bc_compute_state_unordered_view_array&& p_unordered_views,
			bc_compute_state_constant_buffer_array&& p_cbuffers)
			: m_compute_state(p_compute_state),
			m_dispatch_x(p_dispatch_x),
			m_dispatch_y(p_dispatch_y),
			m_dispatch_z(p_dispatch_z),
			m_samplers(std::move(p_samplers)),
			m_resource_views(std::move(p_shader_views)),
			m_unordered_views(std::move(p_unordered_views)),
			m_cbuffers(std::move(p_cbuffers))
		{
		}
	}
}