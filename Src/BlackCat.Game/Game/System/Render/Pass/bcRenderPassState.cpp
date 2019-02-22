// [06/13/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/Pass/bcRenderPassState.h"

namespace black_cat
{
	namespace game
	{
		_bc_render_pass_state_handle_deleter::_bc_render_pass_state_handle_deleter()
			: m_render_system(nullptr)
		{
		}

		_bc_render_pass_state_handle_deleter::_bc_render_pass_state_handle_deleter(bc_render_system* p_render_system)
			: m_render_system(p_render_system)
		{
		}

		bc_render_pass_state* _bc_render_pass_state_handle_deleter::operator()(handle_t p_handle) const
		{
			bcAssert(m_render_system != nullptr);

			auto& l_render_state = m_render_system->m_render_pass_states.at(p_handle.m_index);

			if (l_render_state.is_set())
			{
				return &l_render_state.get();
			}

			return nullptr;
		}

		void _bc_render_pass_state_handle_deleter::operator()(bc_render_pass_state* p_render_state) const
		{
			bcAssert(m_render_system != nullptr);

			m_render_system->destroy_render_pass_state(p_render_state);
		}

		bc_render_pass_state::bc_render_pass_state(bc_render_pass_state&& p_other) noexcept
			: bc_ref_count(std::move(p_other)),
			m_pipeline_state(p_other.m_pipeline_state),
			m_viewport(p_other.m_viewport),
			m_shader_targets(std::move(p_other.m_shader_targets)),
			m_shader_depth(p_other.m_shader_depth),
			m_shader_samplers(std::move(p_other.m_shader_samplers)),
			m_resource_views(std::move(p_other.m_resource_views)),
			m_shader_cbuffers(std::move(p_other.m_shader_cbuffers))
		{
		}

		bc_render_pass_state& bc_render_pass_state::operator=(bc_render_pass_state&& p_other) noexcept
		{
			m_pipeline_state = p_other.m_pipeline_state;
			m_viewport = p_other.m_viewport;
			m_shader_targets = std::move(p_other.m_shader_targets);
			m_shader_depth = p_other.m_shader_depth;
			m_shader_samplers = std::move(p_other.m_shader_samplers);
			m_resource_views = std::move(p_other.m_resource_views);
			m_shader_cbuffers = std::move(p_other.m_shader_cbuffers);
			bc_ref_count::operator=(std::move(p_other));

			return *this;
		}

		bc_render_pass_state::bc_render_pass_state(graphic::bc_device_pipeline_state p_pipeline_state,
			graphic::bc_viewport p_viewport,
			bc_render_pass_state_render_target_view_array&& p_shader_targets, 
			graphic::bc_depth_stencil_view p_shader_depth, 
			bc_render_pass_state_sampler_array&& p_shader_samplers, 
			bc_render_pass_state_resource_view_array&& p_shader_views, 
			bc_render_pass_state_constant_buffer_array&& p_shader_cbuffers)
			: m_pipeline_state(p_pipeline_state),
			m_viewport(p_viewport),
			m_shader_targets(std::move(p_shader_targets)),
			m_shader_depth(p_shader_depth),
			m_shader_samplers(std::move(p_shader_samplers)),
			m_resource_views(std::move(p_shader_views)),
			m_shader_cbuffers(std::move(p_shader_cbuffers))
		{
		}
	}
}