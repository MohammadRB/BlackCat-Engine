// [06/07/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "Game/System/Render/Pass/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/Light/bcDirectLight.h"
#include "Game/System/Input/bcFreeCamera.h"
#include "BlackCat/bcExport.h"
#include "BlackCat/RenderPass/ShadowMap/bcCascadedShadowMapCamera.h"
#include "BlackCat/RenderPass/ShadowMap/bcCascadedShadowMapBufferContainer.h"

namespace black_cat
{
	class _bc_cascaded_shadow_map_light_state
	{
	public:
		core::bc_vector_movale<core::bc_matrix4f> m_last_view_projections;

		graphic::bc_texture2d_ptr m_depth_buffer;
		graphic::bc_resource_view_ptr m_depth_buffer_resource_view;
		core::bc_vector<graphic::bc_depth_stencil_view_ptr> m_depth_buffer_views;
		core::bc_vector<game::bc_render_pass_state_ptr> m_render_pass_states;
	};
	
	class BC_DLL bc_cascaded_shadow_map_pass : public game::bc_irender_pass
	{
		BC_RENDER_PASS(cascaded_shadow_map)

	public:
		bc_cascaded_shadow_map_pass(constant::bc_render_pass_variable_t p_output_depth_buffers, bcSIZE p_shadow_map_size, std::initializer_list<std::tuple<bcSIZE, bcUBYTE>> p_cascade_sizes);

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_param& p_param) override;

		void initialize_frame(const game::bc_render_pass_render_param& p_param) override;

		void execute(const game::bc_render_pass_render_param& p_param) override;

		void cleanup_frame(const game::bc_render_pass_render_param& p_param) override;

		void before_reset(const game::bc_render_pass_reset_param& p_param) override;

		void after_reset(const game::bc_render_pass_reset_param& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

		void capture_debug_shapes();

	private:
		_bc_cascaded_shadow_map_light_state _create_light_instance(game::bc_render_system& p_render_system);

		core::bc_vector_frame<bc_cascaded_shadow_map_camera> _get_light_cascades(const game::bc_icamera& p_camera, const game::bc_direct_light& p_light);

		core::bc_vector_frame<bc_cascaded_shadow_map_camera> _get_light_stabilized_cascades(const game::bc_icamera& p_camera, const game::bc_direct_light& p_light);

		const bcFLOAT m_cascade_cameras_distance = 100;
		constant::bc_render_pass_variable_t m_depth_buffers_share_slot;
		bcSIZE m_shadow_map_size;
		core::bc_vector_program<bcSIZE> m_cascade_sizes;
		core::bc_vector_program<std::tuple<bcUBYTE, bcUBYTE>> m_cascade_update_intervals;

		graphic::bc_device_command_list_ptr m_command_list;
		graphic::bc_device_pipeline_state_ptr m_device_pipeline;
		//graphic::bc_buffer_ptr m_parameters_cbuffer;

		core::bc_vector_program<_bc_cascaded_shadow_map_light_state> m_light_instance_states;

		bool m_capture_debug_shapes = false;
		game::bc_icamera::extend m_captured_camera;
		core::bc_vector_movale<game::bc_icamera::extend> m_captured_cascades;
		core::bc_vector_movale<physics::bc_bound_box> m_captured_boxes;
	};
}