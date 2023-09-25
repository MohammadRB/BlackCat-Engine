// [27/09/2019 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcDeque.h"
#include "Core/Container/bcSpan.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcConcurrentRenderPass.h"
#include "Game/System/Render/Light/bcDirectLight.h"
#include "Game/System/Input/bcCameraInstance.h"
#include "Game/Query/bcSceneLightQuery.h"
#include "App/bcExport.h"
#include "App/RenderPass/ShadowMap/bcCascadedShadowMapCamera.h"
#include "App/RenderPass/ShadowMap/bcCascadedShadowMapBufferContainer.h"

namespace black_cat
{
	struct bc_cascade_shadow_map_trait
	{
		bcUINT m_distance;
		bcUINT m_update_interval;
	};

	struct _bc_cascaded_shadow_map_light_state
	{
	public:
		graphic::bc_texture2d_ref m_depth_buffer;
		graphic::bc_resource_view_ref m_depth_buffer_resource_view;
		core::bc_vector<graphic::bc_depth_stencil_view_ref> m_depth_buffer_views;
		core::bc_vector<core::bc_vector<game::bc_render_pass_state_ptr>> m_render_pass_states;
		core::bc_vector_movable<core::bc_matrix4f> m_last_view_projections;
	};

	struct _bc_cascaded_shadow_map_pass_state
	{
	public:
		const bcFLOAT m_cascade_cameras_distance = 100;
		bcUINT m_instance_count;
		bcUINT m_active_instance_count;
		bcUINT m_back_buffer_width;
		bcUINT m_back_buffer_height;
		bcFLOAT m_shadow_map_multiplier;
		bcUINT m_shadow_map_size;

		core::bc_vector<bc_cascade_shadow_map_trait> m_cascade_sizes;
		core::bc_vector<std::tuple<bcUBYTE, bcUBYTE>> m_cascade_update_intervals;

		game::bc_render_pass_variable_t m_output_depth_buffers_share_slot;
		core::bc_vector<_bc_cascaded_shadow_map_light_state> m_light_states;
		core::bc_query_result<game::bc_scene_light_query> m_lights_query;
		core::bc_vector<game::bc_light_instance> m_lights;
		
		bool m_capture_debug_shapes = false;
		game::bci_camera::extend_points m_captured_camera;
		core::bc_vector_movable<game::bci_camera::extend_points> m_captured_cascades;
		core::bc_vector_movable<physics::bc_bound_box> m_captured_boxes;

		bool enter_once_call()
		{
			const bcUINT l_once_flag = m_once_flag.fetch_add(1);
			return l_once_flag % m_active_instance_count == 0;
		}

		void reset_once_flag()
		{
			m_once_flag.store(0, platform::bc_memory_order::relaxed);
		}

		void wait_for_sync_flag(bcUINT p_sync_id) noexcept
		{
			m_sync_flag.fetch_add(1);
			while (m_sync_flag.load() / m_active_instance_count < p_sync_id)
			{
			}
		}

		void reset_sync_flag() noexcept
		{
			m_sync_flag.store(0, platform::bc_memory_order::relaxed);
		}

	private:
		platform::bc_atomic<bcUINT> m_sync_flag{ 0 };
		platform::bc_atomic<bcUINT> m_once_flag{ 0 };
	};

	struct bc_cascaded_shadow_map_pass_init_frame_context : public game::bc_concurrent_render_pass_render_context
	{
	public:
		bc_cascaded_shadow_map_pass_init_frame_context(const game::bc_concurrent_render_pass_render_context& p_context,
			const game::bc_camera_instance& p_update_cascade_camera,
			const game::bc_camera_instance& p_render_cascade_camera,
			bcSIZE p_light_index,
			bcSIZE p_cascade_index,
			bcSIZE p_cascade_count,
			bool p_render_next_frame,
			bool p_render_this_frame)
			: bc_concurrent_render_pass_render_context(p_context),
			m_update_cascade_camera(p_update_cascade_camera),
			m_render_cascade_camera(p_render_cascade_camera),
			m_light_index(p_light_index),
			m_cascade_index(p_cascade_index),
			m_cascade_count(p_cascade_count),
			m_cascade_absolute_index(p_light_index * p_cascade_count + p_cascade_index),
			m_render_next_frame(p_render_next_frame),
			m_render_this_frame(p_render_this_frame)
		{
		}

		const game::bc_camera_instance& m_update_cascade_camera;
		const game::bc_camera_instance& m_render_cascade_camera;
		bcSIZE m_light_index;
		bcSIZE m_cascade_index;
		bcSIZE m_cascade_count;
		bcSIZE m_cascade_absolute_index;
		bool m_render_next_frame;
		bool m_render_this_frame;
	};
	
	struct bc_cascaded_shadow_map_pass_render_context : public game::bc_concurrent_render_pass_render_context
	{
	public:
		bc_cascaded_shadow_map_pass_render_context(const game::bc_concurrent_render_pass_render_context& p_context,
			const core::bc_vector<game::bc_render_pass_state_ptr>& p_render_states,
			const game::bc_camera_instance& p_update_cascade_camera,
			const game::bc_camera_instance& p_render_cascade_camera,
			bcSIZE p_light_index,
			bcSIZE p_cascade_index,
			bcSIZE p_cascade_count)
			: bc_concurrent_render_pass_render_context(p_context),
			m_render_pass_states(p_render_states),
			m_update_cascade_camera(p_update_cascade_camera),
			m_render_cascade_camera(p_render_cascade_camera),
			m_light_index(p_light_index),
			m_cascade_index(p_cascade_index),
			m_cascade_count(p_cascade_count),
			m_cascade_absolute_index(p_light_index* p_cascade_count + p_cascade_index)
		{
		}

		const core::bc_vector<game::bc_render_pass_state_ptr>& m_render_pass_states;
		const game::bc_camera_instance& m_update_cascade_camera;
		const game::bc_camera_instance& m_render_cascade_camera;
		bcSIZE m_light_index;
		bcSIZE m_cascade_index;
		bcSIZE m_cascade_count;
		bcSIZE m_cascade_absolute_index;
	};

	struct bc_cascaded_shadow_map_pass_cleanup_context : public game::bc_render_pass_render_context
	{
	public:
		bc_cascaded_shadow_map_pass_cleanup_context(const game::bc_render_pass_render_context& p_context,
			const core::bc_vector<game::bc_render_pass_state_ptr>& p_render_states,
			bcSIZE p_light_index,
			bcSIZE p_cascade_index,
			bcSIZE p_cascade_count)
			: bc_render_pass_render_context(p_context),
			m_render_pass_states(p_render_states),
			m_light_index(p_light_index),
			m_cascade_index(p_cascade_index),
			m_cascade_count(p_cascade_count),
			m_cascade_absolute_index(p_light_index* p_cascade_count + p_cascade_index)
		{
		}

		const core::bc_vector<game::bc_render_pass_state_ptr>& m_render_pass_states;
		bcSIZE m_light_index;
		bcSIZE m_cascade_index;
		bcSIZE m_cascade_count;
		bcSIZE m_cascade_absolute_index;
	};

	class BC_DLL bc_base_cascaded_shadow_map_pass : public game::bci_concurrent_render_pass
	{
	public:
		void initialize_resources(game::bc_render_system& p_render_system) override final;

		void update(const game::bc_render_pass_update_context& p_context) override final;

		void initialize_frame(const game::bc_concurrent_render_pass_render_context& p_context) override final;

		void execute(const game::bc_concurrent_render_pass_render_context& p_context) override final;

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
			std::initializer_list<bc_cascade_shadow_map_trait> p_cascade_sizes);

	private:
		virtual void initialize_pass(game::bc_render_system& p_render_system) = 0;

		virtual void initialize_frame_pass(const bc_cascaded_shadow_map_pass_init_frame_context& p_context) = 0;
		
		virtual void execute_pass(const bc_cascaded_shadow_map_pass_render_context& p_context) = 0;

		virtual void cleanup_frame_pass(const bc_cascaded_shadow_map_pass_cleanup_context& p_context) = 0;

		virtual void destroy_pass(game::bc_render_system& p_render_system) = 0;

		virtual core::bc_vector<game::bc_render_pass_state_ptr> create_render_pass_states(game::bc_render_system& p_render_system,
			graphic::bc_texture2d p_depth,
			const core::bc_vector<graphic::bc_depth_stencil_view_ref>& p_depth_views) = 0;

		void _reset_cascade_sizes(bcFLOAT p_shadow_map_multiplier, core::bc_const_span<bc_cascade_shadow_map_trait> p_cascades);

		_bc_cascaded_shadow_map_light_state _create_light_state(game::bc_render_system& p_render_system);

		core::bc_vector<bc_cascaded_shadow_map_camera> _get_light_cascades(const game::bci_camera& p_camera, const game::bc_direct_light& p_light);

		core::bc_vector<bc_cascaded_shadow_map_camera> _get_light_stabilized_cascades(const game::bc_camera_instance& p_camera, const game::bc_direct_light& p_light);
	
		const bcSIZE m_my_index;
		graphic::bc_device_command_list_ref m_command_list;
		core::bc_shared_ptr<_bc_cascaded_shadow_map_pass_state> m_state;

		// Passed camera references to 'init frame' and 'execute' functions must remain valid until the end of frame.
		// We store them in this list to maintain their references.
		core::bc_deque<game::bc_camera_instance> m_last_cameras;
		core::bc_vector<bc_cascaded_shadow_map_camera> m_last_update_cascade_cameras;
		core::bc_vector<bc_cascaded_shadow_map_camera> m_last_render_cascade_cameras;
	};
}