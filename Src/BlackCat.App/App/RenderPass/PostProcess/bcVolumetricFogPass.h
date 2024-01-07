// [23/07/2023 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcSpan.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/Query/bcSceneQuery.h"
#include "App/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;
	}

	struct _bc_fog_query_instance
	{
		core::bc_vector3f m_center;
		core::bc_vector3f m_half_extend;
		core::bc_vector3f m_min;
		core::bc_vector3f m_max;
		core::bc_vector3f m_color;
		float m_visibility;
		float m_center_fade;
		float m_intensity;
		bool m_global_light;
	};

	class BC_DLL bc_volumetric_fog_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(vol_fog)

	public:
		bc_volumetric_fog_pass(game::bc_render_pass_variable_t p_render_target_texture, game::bc_render_pass_variable_t p_render_target_view);

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		void _copy_fog_instances_to_buffer(const game::bc_render_pass_render_context& p_context, core::bc_const_span<_bc_fog_query_instance> p_instances);

		void _update_params(game::bc_default_render_thread& p_render_thread, const game::bc_camera_instance& p_camera);

		static constexpr bcUINT s_query_execution_delay_ms = 2000;
		static constexpr bcUINT s_max_instance_per_draw = 10;

		game::bc_render_pass_variable_t m_render_target_texture;
		game::bc_render_pass_variable_t m_render_target_view;

		graphic::bc_buffer_ref m_cube_vb;
		graphic::bc_buffer_ref m_cube_ib;
		graphic::bc_buffer_ref m_params_buffer;
		graphic::bc_buffer_ref m_fog_instances_buffer;
		graphic::bc_resource_view_ref m_fog_instances_buffer_view;
		graphic::bc_sampler_state_ref m_point_sampler;
		graphic::bc_device_pipeline_state_ref m_device_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;
		game::bc_render_state_ptr m_render_state;

		bcFLOAT m_last_query_execution_elapsed = s_query_execution_delay_ms;
		core::bc_query_result_t<game::bc_scene_query> m_fog_query;
		core::bc_vector<_bc_fog_query_instance> m_fog_instances;
		bool m_update_fog_instances;
	};
}