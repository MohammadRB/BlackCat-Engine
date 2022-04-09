// [09/27/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Container/bcDeque.h"
#include "Core/Container/bcSpan.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/Light/bcDirectLight.h"
#include "Game/System/Input/bcCameraInstance.h"
#include "Game/Query/bcSceneLightQuery.h"
#include "App/bcExport.h"
#include "App/RenderPass/ShadowMap/bcCascadedShadowMapCamera.h"
#include "App/RenderPass/ShadowMap/bcCascadedShadowMapBufferContainer.h"

namespace black_cat
{
	class _bc_cascaded_shadow_map_light_state
	{
	public:
		graphic::bc_texture2d_ref m_depth_buffer;
		graphic::bc_resource_view_ref m_depth_buffer_resource_view;
		core::bc_vector<graphic::bc_depth_stencil_view_ref> m_depth_buffer_views;
		core::bc_vector<core::bc_vector<game::bc_render_pass_state_ptr>> m_render_pass_states;
		core::bc_vector_movable<core::bc_matrix4f> m_last_view_projections;
		// Passed camera references to 'init frame' and 'execute' functions must remain valid until the end of frame.
		// We store them in this list to maintain their references.
		core::bc_deque<game::bc_camera_instance> m_last_cameras;
	};

	class _bc_cascaded_shadow_map_pass_state
	{
	public:
		const bcFLOAT m_cascade_cameras_distance = 100;
		bcSIZE m_instance_count;
		bcSIZE m_back_buffer_width;
		bcSIZE m_back_buffer_height;
		
		game::bc_render_pass_variable_t m_output_depth_buffers_share_slot;
		bcFLOAT m_shadow_map_multiplier;
		bcUINT32 m_shadow_map_size;
		core::bc_vector<bcSIZE> m_cascade_sizes;
		core::bc_vector<std::tuple<bcUBYTE, bcUBYTE>> m_cascade_update_intervals;

		core::bc_vector<_bc_cascaded_shadow_map_light_state> m_light_states;
		core::bc_query_result<game::bc_scene_light_query> m_lights_query;
		core::bc_vector<game::bc_light_instance> m_lights;
		
		bool m_capture_debug_shapes = false;
		game::bci_camera::extend m_captured_camera;
		core::bc_vector_movable<game::bci_camera::extend> m_captured_cascades;
		core::bc_vector_movable<physics::bc_bound_box> m_captured_boxes;
	};

	class bc_cascaded_shadow_map_pass_init_frame_param : public game::bc_render_pass_render_context
	{
	public:
		bc_cascaded_shadow_map_pass_init_frame_param(const game::bc_render_pass_render_context& p_render_param,
			const game::bc_camera_instance& p_update_cascade_camera,
			const game::bc_camera_instance& p_render_cascade_camera,
			bcSIZE p_light_index,
			bcSIZE p_cascade_index,
			bcSIZE p_cascade_count)
			: bc_render_pass_render_context
			(
				p_render_param.m_clock,
				p_render_param.m_query_manager,
				p_render_param.m_render_system,
				p_render_param.m_frame_renderer,
				p_render_param.m_render_thread,
				p_render_param.m_update_camera,
				p_render_param.m_render_camera
			),
			m_update_cascade_camera(p_update_cascade_camera),
			m_render_cascade_camera(p_render_cascade_camera),
			m_light_index(p_light_index),
			m_cascade_index(p_cascade_index),
			m_cascade_count(p_cascade_count)
		{
		}

		const game::bc_camera_instance& m_update_cascade_camera;
		const game::bc_camera_instance& m_render_cascade_camera;
		bcSIZE m_light_index;
		bcSIZE m_cascade_index;
		bcSIZE m_cascade_count;
	};
	
	class bc_cascaded_shadow_map_pass_render_param : public game::bc_render_pass_render_context
	{
	public:
		bc_cascaded_shadow_map_pass_render_param(const game::bc_render_pass_render_context& p_render_param,
			const core::bc_vector<game::bc_render_pass_state_ptr>& p_render_states,
			const game::bc_camera_instance& p_update_cascade_camera,
			const game::bc_camera_instance& p_render_cascade_camera,
			bcSIZE p_light_index,
			bcSIZE p_cascade_index,
			bcSIZE p_cascade_count)
			: bc_render_pass_render_context
			(
				p_render_param.m_clock,
				p_render_param.m_query_manager,
				p_render_param.m_render_system,
				p_render_param.m_frame_renderer,
				p_render_param.m_render_thread,
				p_render_param.m_update_camera,
				p_render_param.m_render_camera
			),
			m_render_pass_states(p_render_states),
			m_update_cascade_camera(p_update_cascade_camera),
			m_render_cascade_camera(p_render_cascade_camera),
			m_light_index(p_light_index),
			m_cascade_index(p_cascade_index),
			m_cascade_count(p_cascade_count)
		{
		}

		const core::bc_vector<game::bc_render_pass_state_ptr>& m_render_pass_states;
		const game::bc_camera_instance& m_update_cascade_camera;
		const game::bc_camera_instance& m_render_cascade_camera;
		bcSIZE m_light_index;
		bcSIZE m_cascade_index;
		bcSIZE m_cascade_count;
	};

	class BC_DLL bc_base_cascaded_shadow_map_pass : public game::bci_render_pass
	{
	public:
		void initialize_resources(game::bc_render_system& p_render_system) override final;

		void update(const game::bc_render_pass_update_context& p_context) override final;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override final;

		void execute(const game::bc_render_pass_render_context& p_context) override final;

		void cleanup_frame(const game::bc_render_pass_render_context& p_context) override final;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override final;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override final;

		void destroy(game::bc_render_system& p_render_system) override final;

		void config_changed(const game::bc_render_pass_config_change_context& p_context) override final;

		void capture_debug_shapes();

	protected:
		bc_base_cascaded_shadow_map_pass(const bc_base_cascaded_shadow_map_pass* p_other_instance);

		bc_base_cascaded_shadow_map_pass(game::bc_render_pass_variable_t p_output_depth_buffers,
			bcFLOAT p_shadow_map_multiplier,
			std::initializer_list<std::tuple<bcSIZE, bcUBYTE>> p_cascade_sizes);

		bcSIZE my_index() const;

	private:
		virtual void initialize_pass(game::bc_render_system& p_render_system) = 0;

		virtual void initialize_frame_pass(const bc_cascaded_shadow_map_pass_init_frame_param& p_param) = 0;
		
		virtual void execute_pass(const bc_cascaded_shadow_map_pass_render_param& p_param) = 0;

		virtual void destroy_pass(game::bc_render_system& p_render_system) = 0;

		virtual core::bc_vector<game::bc_render_pass_state_ptr> create_render_pass_states(game::bc_render_system& p_render_system,
			graphic::bc_texture2d p_depth,
			const core::bc_vector<graphic::bc_depth_stencil_view_ref>& p_depth_views) = 0;

		void _reset_cascade_sizes(bcFLOAT p_shadow_map_multiplier, core::bc_const_span<core::bc_vector2i> p_cascades);

		_bc_cascaded_shadow_map_light_state _create_light_instance(game::bc_render_system& p_render_system);

		core::bc_vector_frame<bc_cascaded_shadow_map_camera> _get_light_cascades(const game::bci_camera& p_camera, const game::bc_direct_light& p_light);

		core::bc_vector_frame<bc_cascaded_shadow_map_camera> _get_light_stabilized_cascades(const game::bc_camera_instance& p_camera, const game::bc_direct_light& p_light);
	
		const bcSIZE m_my_index;
		core::bc_shared_ptr<_bc_cascaded_shadow_map_pass_state> m_state;
	};
}