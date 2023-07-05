// [06/09/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/State/bcComputeState.h"

namespace black_cat::game
{
	_bc_compute_state_handle_deleter::_bc_compute_state_handle_deleter()
		: m_render_system(nullptr)
	{
	}

	_bc_compute_state_handle_deleter::_bc_compute_state_handle_deleter(bc_render_system* p_render_system)
		: m_render_system(p_render_system)
	{
	}

	void _bc_compute_state_handle_deleter::operator()(bc_compute_state* p_compute_state) const
	{
		BC_ASSERT(m_render_system != nullptr);

		m_render_system->_destroy_compute_state(p_compute_state);
	}

	bc_compute_state::bc_compute_state(bc_compute_state&& p_other) noexcept
		: bc_ref_count(std::move(p_other)),
		  m_compute_state(p_other.m_compute_state),
		  m_samplers(std::move(p_other.m_samplers)),
		  m_resource_views(std::move(p_other.m_resource_views)),
		  m_unordered_views(std::move(p_other.m_unordered_views)),
		  m_cbuffers(std::move(p_other.m_cbuffers))
	{	
	}

	bc_compute_state::~bc_compute_state()
	{
	}

	bc_compute_state& bc_compute_state::operator=(bc_compute_state&& p_other) noexcept
	{
		bc_ref_count::operator=(std::move(p_other));
		m_compute_state = p_other.m_compute_state;
		m_samplers = std::move(p_other.m_samplers);
		m_resource_views = std::move(p_other.m_resource_views);
		m_unordered_views = std::move(p_other.m_unordered_views);
		m_cbuffers = std::move(p_other.m_cbuffers);

		return *this;
	}

	bc_compute_state::bc_compute_state(graphic::bc_device_compute_state p_compute_state,
	                                   bc_compute_state_sampler_array&& p_samplers,
	                                   bc_compute_state_resource_view_array&& p_shader_views,
	                                   bc_compute_state_unordered_view_array&& p_unordered_views,
	                                   bc_compute_state_constant_buffer_array&& p_cbuffers)
		: bc_ref_count(),
		  m_compute_state(p_compute_state),
		  m_samplers(std::move(p_samplers)),
		  m_resource_views(std::move(p_shader_views)),
		  m_unordered_views(std::move(p_unordered_views)),
		  m_cbuffers(std::move(p_cbuffers))
	{
	}
}
