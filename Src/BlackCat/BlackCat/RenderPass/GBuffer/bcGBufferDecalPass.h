// [02/25/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
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
		class bc_mesh_material;
		class bc_render_instance;
	}
	
	class BC_DLL bc_gbuffer_decal_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(gb_dcl)

	private:
		using decal_group_container = core::bc_unordered_map_frame< const game::bc_mesh_material*, core::bc_vector_frame< std::pair< game::bc_decal*, game::bc_render_instance* >>>;
		
	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;
		
		void update(const game::bc_render_pass_update_context& p_param) override;
		
		void initialize_frame(const game::bc_render_pass_render_context& p_param) override;
		
		void execute(const game::bc_render_pass_render_context& p_param) override;
		
		void before_reset(const game::bc_render_pass_reset_context& p_param) override;
		
		void after_reset(const game::bc_render_pass_reset_context& p_param) override;
		
		void destroy(game::bc_render_system& p_render_system) override;

	private:
		void _render_decals(const game::bc_render_pass_render_context& p_param,
			game::bc_render_pass_state& p_render_pass_state, 
			decal_group_container& p_instances,
			decal_group_container* p_non_culling_instances);
		
		void _create_decal_render_state(game::bc_render_system& p_render_system, const game::bc_mesh_material& p_material);

		static const bcUINT32 s_max_instance_per_draw = 300;
		
		graphic::bc_buffer_ptr m_cube_vb;
		graphic::bc_buffer_ptr m_cube_ib;
		graphic::bc_buffer_ptr m_instance_buffer;
		graphic::bc_resource_view_ptr m_instance_buffer_view;
		graphic::bc_resource_view_ptr m_depth_view;
		graphic::bc_resource_view_ptr m_stencil_view;
		graphic::bc_sampler_state_ptr m_point_sampler;
		graphic::bc_sampler_state_ptr m_linear_sampler;
		graphic::bc_device_pipeline_state_ptr m_device_pipeline_state;
		graphic::bc_device_pipeline_state_ptr m_device_pipeline_state_for_non_culling;
		game::bc_render_pass_state_ptr m_render_pass_state;
		game::bc_render_pass_state_ptr m_render_pass_state_for_non_culling;
		core::bc_unordered_map< const game::bc_mesh_material*, game::bc_render_state_ptr > m_render_states; // Use raw pointer to let materials get destroyed

		core::bc_query_result< game::bc_main_camera_render_state_query > m_decals_query;
		game::bc_render_state_buffer m_decals_buffer;
	};
}