// [08/30/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Query/bcHeightMapSceneQuery.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_gbuffer_terrain_pass_dx11 : public game::bc_irender_pass
	{
		BC_RENDER_PASS(gb_trn)

	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_update_param) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_param) override;

		void execute(const game::bc_render_pass_render_context& p_param) override;

		void cleanup_frame(const game::bc_render_pass_render_context& p_param) override;
		
		void before_reset(const game::bc_render_pass_reset_context& p_param) override;

		void after_reset(const game::bc_render_pass_reset_context& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

		void update_chunk_infos();
		
	private:
		static constexpr bcUINT16 s_shader_thread_group_size = 32;
		static constexpr bcUINT16 s_chunk_size = 64;

		bool m_run_chunk_info_shader = false;

		graphic::bc_device_pipeline_state_ptr m_device_pipeline_state;
		graphic::bc_device_compute_state_ptr m_device_compute_state;
		graphic::bc_buffer_ptr m_parameter_cbuffer;
		graphic::bc_sampler_state_ptr m_height_map_sampler;
		graphic::bc_sampler_state_ptr m_texture_sampler;
		game::bc_render_pass_state_ptr m_render_pass_state;

		core::bc_query_result<game::bc_height_map_scene_query> m_height_maps_query;
		game::bc_render_state_buffer m_height_maps_render_buffer;
	};
}