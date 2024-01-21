// [11/12/2023 MRB]

#include "App/AppPCH.h"

#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/Object/Scene/Component/bcWaterComponent.h"
#include "App/RenderPass/PostProcess/bcWaterPass.h"

namespace black_cat
{
	bc_water_pass::bc_water_pass(game::bc_render_pass_variable_t p_render_target_texture,
		game::bc_render_pass_variable_t p_render_target_view)
		: m_render_target_texture(p_render_target_texture),
		m_render_target_view(p_render_target_view),
		m_query_delay(std::chrono::milliseconds(s_query_execution_delay_ms), true)
	{
	}

	void bc_water_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();

		after_reset(game::bc_render_pass_reset_context::create_default_instance(p_render_system));
	}

	void bc_water_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_water_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
		if (m_water_query.is_executed())
		{
			m_water_instances = m_water_query.get().get_result();
		}

		if (!m_query_delay.update(p_context.m_clock))
		{
			return;
		}

		auto l_query = game::bc_scene_actor_query<_bc_water_query_instance>();
		l_query
			.only<game::bc_water_component>()
			.with(game::bc_camera_frustum(p_context.m_render_camera))
			.transform([](const game::bc_actor& p_actor)
				{
					const auto& l_water_component = p_actor.get_component<game::bc_water_component>();
					return _bc_water_query_instance
					{
						l_water_component->get_center(),
						l_water_component->get_half_extend()
					};
				});

		m_water_query = p_context.m_query_manager.queue_query(std::move(l_query));
	}

	void bc_water_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_water_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_water_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_water_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_point_sampler.reset();
		m_device_pipeline_state.reset();
		m_render_pass_state.reset();
	}
}
