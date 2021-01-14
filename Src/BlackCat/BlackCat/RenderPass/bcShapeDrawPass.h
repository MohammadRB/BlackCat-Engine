// [12/06/2018 MRB]

#pragma once

#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/bcShapeDrawer.h"
#include "Game/Query/bcQueryContext.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class bc_scene_debug_shape_query : public core::bc_query< game::bc_scene_query_context >
	{
		BC_QUERY(sc_dbg)

	public:
		explicit bc_scene_debug_shape_query(game::bc_shape_drawer& p_shape_drawer, const game::bc_actor& p_selected_actor) noexcept;

		void execute(const game::bc_scene_query_context& p_context) noexcept override;

	private:
		game::bc_shape_drawer* m_shape_drawer;
		game::bc_actor m_selected_actor;
	};
	
	class BC_DLL bc_shape_draw_pass : public game::bc_irender_pass
	{
		BC_RENDER_PASS(shp_drw)

	public:
		explicit bc_shape_draw_pass(constant::bc_render_pass_variable_t p_render_target_view);

		void set_selected_actor(const game::bc_actor& p_actor);
		
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_update_param) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_param) override;

		void execute(const game::bc_render_pass_render_context& p_param) override;

		void cleanup_frame(const game::bc_render_pass_render_context& p_param) override;
		
		void before_reset(const game::bc_render_pass_reset_context& p_param) override;
		
		void after_reset(const game::bc_render_pass_reset_context& p_param) override;
		
		void destroy(game::bc_render_system& p_render_system) override;

	private:
		constant::bc_render_pass_variable_t m_render_target_view_variable;

		graphic::bc_device_pipeline_state_ptr m_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;

		core::bc_query_result<bc_scene_debug_shape_query> m_scene_debug_query;
		game::bc_actor m_selected_actor;
	};
}