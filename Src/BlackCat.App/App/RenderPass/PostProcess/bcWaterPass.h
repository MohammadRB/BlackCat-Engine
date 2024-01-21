// [11/12/2023 MRB]

#pragma once

#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "Core/bcUtility.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/Query/bcSceneActorQuery.h"
#include "App/bcExport.h"

namespace black_cat::game
{
	class bc_scene_query;
}

namespace black_cat
{
	struct _bc_water_instance
	{
		graphic::bc_buffer_ref m_vb;
		graphic::bc_buffer_ref m_ib;
		game::bc_render_state_ptr m_render_state;
	};

	struct _bc_water_query_instance
	{
		core::bc_vector3f m_center;
		core::bc_vector2f m_half_extend;
	};

	class BC_DLL bc_water_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(water)

	public:
		bc_water_pass(game::bc_render_pass_variable_t p_render_target_texture, game::bc_render_pass_variable_t p_render_target_view);

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		static constexpr bcUINT s_query_execution_delay_ms = 10'000;
		static constexpr bcUINT s_max_instance_per_draw = 10;
		static constexpr bcUINT s_max_tessellation_factor = graphic::bc_render_api_info::max_tessellation_factor();

		game::bc_render_pass_variable_t m_render_target_texture;
		game::bc_render_pass_variable_t m_render_target_view;

		graphic::bc_sampler_state_ref m_point_sampler;
		graphic::bc_device_pipeline_state_ref m_device_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;

		core::bc_delay m_query_delay;
		core::bc_query_result_t<game::bc_scene_actor_query<_bc_water_query_instance>> m_water_query;
		core::bc_vector<_bc_water_query_instance> m_water_instances;
	};
}
