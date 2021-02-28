// [02/25/2021 MRB]

#pragma once

#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Query/bcMainCameraRenderStateQuery.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_decal;
	}
	
	class BC_DLL bc_gbuffer_decal_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(dc_pass)
		
	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;
		
		void update(const game::bc_render_pass_update_context& p_param) override;
		
		void initialize_frame(const game::bc_render_pass_render_context& p_param) override;
		
		void execute(const game::bc_render_pass_render_context& p_param) override;
		
		void before_reset(const game::bc_render_pass_reset_context& p_param) override;
		
		void after_reset(const game::bc_render_pass_reset_context& p_param) override;
		
		void destroy(game::bc_render_system& p_render_system) override;

	private:
		void _create_decal_render_state(game::bc_render_system& p_render_system, game::bc_decal& p_decal);
		
		graphic::bc_buffer_ptr m_cube_vb;
		graphic::bc_buffer_ptr m_cube_ib;
		graphic::bc_device_pipeline_state_ptr m_device_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;

		core::bc_query_result< game::bc_main_camera_render_state_query > m_decals_query;
		game::bc_render_state_buffer m_decals_buffer;
	};
}