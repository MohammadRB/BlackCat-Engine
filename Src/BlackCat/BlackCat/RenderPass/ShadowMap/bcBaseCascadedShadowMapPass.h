// [09/27/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "Game/System/Render/Pass/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/Light/bcDirectLight.h"
#include "Game/System/Input/bcFreeCamera.h"
#include "Game/bcQuery.h"
#include "BlackCat/bcExport.h"
#include "BlackCat/RenderPass/ShadowMap/bcCascadedShadowMapCamera.h"
#include "BlackCat/RenderPass/ShadowMap/bcCascadedShadowMapBufferContainer.h"

namespace black_cat
{
	class _bc_cascaded_shadow_map_light_state
	{
	public:
		graphic::bc_texture2d_ptr m_depth_buffer;
		graphic::bc_resource_view_ptr m_depth_buffer_resource_view;
		core::bc_vector<graphic::bc_depth_stencil_view_ptr> m_depth_buffer_views;
		core::bc_vector<core::bc_vector<game::bc_render_pass_state_ptr>> m_render_pass_states;
		core::bc_vector_movale<core::bc_matrix4f> m_last_view_projections;
	};

	class _bc_cascaded_shadow_map_pass_state
	{
	public:
		const bcFLOAT m_cascade_cameras_distance = 100;
		bcSIZE m_instance_count;
		
		constant::bc_render_pass_variable_t m_output_depth_buffers_share_slot;
		bcSIZE m_shadow_map_size;
		core::bc_vector_program<bcSIZE> m_cascade_sizes;
		core::bc_vector_program<std::tuple<bcUBYTE, bcUBYTE>> m_cascade_update_intervals;

		graphic::bc_device_command_list_ptr m_command_list;
		core::bc_vector_program<_bc_cascaded_shadow_map_light_state> m_light_instance_states;

		core::bc_query_result<game::bc_scene_graph_query> m_scene_query;
		game::bc_scene_graph_buffer m_scene_query_result;
		
		bool m_capture_debug_shapes = false;
		game::bc_icamera::extend m_captured_camera;
		core::bc_vector_movale<game::bc_icamera::extend> m_captured_cascades;
		core::bc_vector_movale<physics::bc_bound_box> m_captured_boxes;
	};

	class bc_cascaded_shadow_map_pass_render_param : public game::bc_render_pass_render_param
	{
	public:
		bc_cascaded_shadow_map_pass_render_param(const game::bc_render_pass_render_param& p_render_param,
			const core::bc_vector< game::bc_render_pass_state_ptr >& p_render_states,
			const game::bc_camera_instance& p_cascade_camera,
			bcSIZE p_light_index,
			bcSIZE p_cascade_index,
			bcSIZE p_cascade_count)
			: game::bc_render_pass_render_param
			(
				p_render_param.m_clock,
				p_render_param.m_camera,
				p_render_param.m_render_system,
				p_render_param.m_frame_renderer,
				p_render_param.m_render_thread
			),
			m_render_pass_states(p_render_states),
			m_cascade_camera(p_cascade_camera),
			m_light_index(p_light_index),
			m_cascade_index(p_cascade_index),
			m_cascade_count(p_cascade_count)
		{
		}

		const core::bc_vector<game::bc_render_pass_state_ptr>& m_render_pass_states;
		const game::bc_camera_instance& m_cascade_camera;
		bcSIZE m_light_index;
		bcSIZE m_cascade_index;
		bcSIZE m_cascade_count;
	};

	class BC_DLL bc_base_cascaded_shadow_map_pass : public game::bc_irender_pass
	{
	public:
		void initialize_resources(game::bc_render_system& p_render_system) override final;

		void update(const game::bc_render_pass_update_param& p_param) override final;

		void initialize_frame(const game::bc_render_pass_render_param& p_param) override final;

		void execute(const game::bc_render_pass_render_param& p_param) override final;

		void cleanup_frame(const game::bc_render_pass_render_param& p_param) override final;

		void before_reset(const game::bc_render_pass_reset_param& p_param) override final;

		void after_reset(const game::bc_render_pass_reset_param& p_param) override final;

		void destroy(game::bc_render_system& p_render_system) override final;

		void capture_debug_shapes();

	protected:
		bc_base_cascaded_shadow_map_pass(const bc_base_cascaded_shadow_map_pass* p_other_instance);

		bc_base_cascaded_shadow_map_pass(constant::bc_render_pass_variable_t p_output_depth_buffers,
			bcSIZE p_shadow_map_size,
			std::initializer_list<std::tuple<bcSIZE, bcUBYTE>> p_cascade_sizes);

		bcSIZE my_index() const;

	private:
		virtual void initialize_pass(game::bc_render_system& p_render_system) = 0;

		virtual void execute_pass(const bc_cascaded_shadow_map_pass_render_param& p_param) = 0;

		virtual void destroy_pass(game::bc_render_system& p_render_system) = 0;

		virtual core::bc_vector<game::bc_render_pass_state_ptr> create_render_pass_states(game::bc_render_system& p_render_system,
			graphic::bc_texture2d p_depth,
			const core::bc_vector<graphic::bc_depth_stencil_view_ptr>& p_depth_views) = 0;

		_bc_cascaded_shadow_map_light_state _create_light_instance(game::bc_render_system& p_render_system);

		core::bc_vector_frame<bc_cascaded_shadow_map_camera> _get_light_cascades(const game::bc_icamera& p_camera, const game::bc_direct_light& p_light);

		core::bc_vector_frame<bc_cascaded_shadow_map_camera> _get_light_stabilized_cascades(const game::bc_camera_instance& p_camera, const game::bc_direct_light& p_light);

		const bcSIZE m_my_index;
		core::bc_shared_ptr<_bc_cascaded_shadow_map_pass_state> m_state;
	};
}