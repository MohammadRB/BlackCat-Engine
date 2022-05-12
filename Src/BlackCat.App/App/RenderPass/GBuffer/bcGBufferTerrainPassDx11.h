// [08/30/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/System/Render/Pass/bcConcurrentRenderPass.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Query/bcHeightMapSceneQuery.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_gbuffer_terrain_pass_dx11 : public game::bci_render_pass
	{
		BC_RENDER_PASS(gb_trn)

	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void cleanup_frame(const game::bc_render_pass_render_context& p_context) override;
		
		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

		void update_chunk_infos();
		
	private:
		static constexpr bcUINT16 s_shader_thread_group_size = 32;
		static constexpr bcUINT16 s_chunk_size = 64;

		bool m_run_chunk_info_shader = false;

		graphic::bc_device_command_list_ref m_command_list;
		graphic::bc_device_pipeline_state_ref m_device_pipeline_state;
		graphic::bc_device_compute_state_ref m_chunk_info_device_compute_state;
		graphic::bc_buffer_ref m_parameter_cbuffer;
		graphic::bc_sampler_state_ref m_height_map_sampler;
		graphic::bc_sampler_state_ref m_texture_sampler;
		game::bc_render_pass_state_ptr m_render_pass_state;

		game::bc_height_map_scene_query m_height_maps_query;
		core::bc_query_result<core::bci_query> m_height_maps_query_result;
		game::bc_render_state_buffer m_height_maps_render_buffer;
		core::bc_vector<const game::bc_height_map*> m_last_frame_height_maps; // Use weak pointers to let resources get free if needed
	};
}