// [03/07/2019 MRB]

#pragma once

#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/Query/bcSceneLightQuery.h"
#include "Game/Query/bcSceneWindQuery.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_gbuffer_initialize_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(gb_init)

	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_update_param) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_param) override;

		void execute(const game::bc_render_pass_render_context& p_param) override;

		void cleanup_frame(const game::bc_render_pass_render_context& p_param) override;

		void before_reset(const game::bc_render_pass_reset_context& p_param) override;

		void after_reset(const game::bc_render_pass_reset_context& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		graphic::bc_texture2d_ref m_depth_stencil;
		graphic::bc_texture2d_ref m_diffuse_map;
		graphic::bc_texture2d_ref m_normal_map;
		graphic::bc_texture2d_ref m_specular_map;
		graphic::bc_depth_stencil_view_ref m_depth_stencil_view;
		graphic::bc_render_target_view_ref m_diffuse_map_view;
		graphic::bc_render_target_view_ref m_normal_map_view;
		graphic::bc_render_target_view_ref m_specular_map_view;

		core::bc_query_result< game::bc_scene_light_query > m_lights_query;
		core::bc_vector_movable< game::bc_light_instance > m_lights_query_result;
		core::bc_query_result< game::bc_scene_wind_query > m_winds_query;
		core::bc_vector<game::bc_wind> m_winds_query_result;
	};
}