// [06/07/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "Game/System/Render/Pass/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/Light/bcDirectLight.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "BlackCat/bcExport.h"
#include "BlackCat/RenderPass/ShadowMap/bcCascadedShadowMapCamera.h"
#include "BlackCat/RenderPass/ShadowMap/bcCascadedShadowMapBufferContainer.h"

namespace black_cat
{
	class _bc_cascaded_shadow_map_light_state
	{
	public:
		/*graphic::bc_texture2d_ptr m_shadow_map;
		graphic::bc_render_target_view_ptr m_render_target;*/
		graphic::bc_texture2d_ptr m_depth_buffer;
		graphic::bc_depth_stencil_view_ptr m_depth_buffer_view;
		graphic::bc_resource_view_ptr m_depth_buffer_resource_view;

		game::bc_render_pass_state_ptr m_render_pass_state;
	};
	
	class BC_DLL bc_cascaded_shadow_map_pass : public game::bc_irender_pass
	{
		BC_RENDER_PASS(cascaded_shadow_map)

	public:
		bc_cascaded_shadow_map_pass(constant::bc_render_pass_variable_t p_output_depth_buffers, bcSIZE p_shadow_map_size, std::initializer_list<bcSIZE> p_cascade_sizes);

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_param& p_param) override;

		void initialize_frame(const game::bc_render_pass_render_param& p_param) override;

		void execute(const game::bc_render_pass_render_param& p_param) override;

		void cleanup_frame(const game::bc_render_pass_render_param& p_param) override;

		void before_reset(const game::bc_render_pass_reset_param& p_param) override;

		void after_reset(const game::bc_render_pass_reset_param& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		_bc_cascaded_shadow_map_light_state _create_light_instance(game::bc_render_system& p_render_system);

		core::bc_vector_frame<bc_cascaded_shadow_map_camera> _get_light_cascades(const game::bc_icamera& p_camera, const game::bc_direct_light& p_light);

		const bcSIZE m_cascade_cameras_distance = 300;
		bcSIZE m_shadow_map_size;
		core::bc_vector_program<bcSIZE> m_cascade_sizes;

		graphic::bc_device_command_list_ptr m_command_list;
		graphic::bc_device_pipeline_state_ptr m_device_pipeline;

		graphic::bc_buffer_ptr m_parameters_cbuffer;

		core::bc_vector_program<_bc_cascaded_shadow_map_light_state> m_light_instance_states;

		constant::bc_render_pass_variable_t m_depth_buffers_share_slot;
	};
}