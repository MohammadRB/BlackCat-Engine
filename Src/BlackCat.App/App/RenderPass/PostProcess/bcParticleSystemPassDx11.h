// [16/11/2020 MRB]

#pragma once

#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcComputeState.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/Query/bcSceneParticleEmitterQuery.h"
#include "Game/Query/bcSceneLightQuery.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_particle_system_pass_dx11 : public game::bci_render_pass
	{
		BC_RENDER_PASS(ptc_sys)
		
	public:
		bc_particle_system_pass_dx11(game::bc_render_pass_variable_t p_render_target_texture, 
			game::bc_render_pass_variable_t p_render_target_view, 
			core::bc_estring_view p_sprites_content_path);
		
		void initialize_resources(game::bc_render_system& p_render_system) override;
		
		void update(const game::bc_render_pass_update_context& p_context) override;
		
		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;
		
		void execute(const game::bc_render_pass_render_context& p_context) override;
		
		void before_reset(const game::bc_render_pass_reset_context& p_context) override;
		
		void after_reset(const game::bc_render_pass_reset_context& p_context) override;
		
		void destroy(game::bc_render_system& p_render_system) override;

	private:
		void _execute_sort_shader(const game::bc_render_pass_render_context& p_param);
		
		// The number of elements to sort is limited to an even power of 2
		// At minimum - m_sort_shader_group_size * m_sort_transpose_shader_group_size
		// At maximum - m_sort_shader_group_size * m_sort_shader_group_size
		static constexpr bcUINT s_emitters_count = 128;
		static constexpr bcUINT s_particles_count = 32768;
		static constexpr bcUINT s_emission_shader_group_size = 1;
		static constexpr bcUINT s_simulation_shader_group_size = 1024;
		static constexpr bcUINT s_sort_shader_group_size = 1024;
		static constexpr bcUINT s_sort_transpose_shader_group_size = 32;
		static constexpr bcUINT s_sort_transpose_matrix_width = s_sort_shader_group_size;
		static constexpr bcUINT s_sort_transpose_matrix_height = s_particles_count / s_sort_shader_group_size;
		static constexpr bcUINT s_lights_count = 10;
		
		bcINT32 m_dead_particles_initial_count = s_particles_count;
		const game::bc_render_pass_variable_t m_render_target_texture_param;
		const game::bc_render_pass_variable_t m_render_target_view_param;
		core::bc_estring_view m_sprites_content_path;

		graphic::bc_texture_config m_intermediate_texture_config;
		graphic::bc_shader_parameter_link m_intermediate_texture_link;
		graphic::bc_depth_stencil_view m_depth_buffer_view;
		graphic::bc_resource_view m_depth_buffer_shader_view;
		graphic::bc_texture2d_content_ptr m_sprites_texture;
		graphic::bc_resource_view_ref m_sprites_view;
		graphic::bc_buffer_ref m_emitters_buffer;
		graphic::bc_buffer_ref m_particles_buffer;
		graphic::bc_buffer_ref m_alive_particles_buffer1;
		graphic::bc_buffer_ref m_alive_particles_buffer2;
		graphic::bc_buffer_ref m_dead_particles_buffer;
		graphic::bc_buffer_ref m_draw_args_buffer;
		graphic::bc_buffer_ref m_sort_cbuffer;
		graphic::bc_buffer_ref m_curves_cbuffer;
		graphic::bc_buffer_ref m_lights_cbuffer;

		graphic::bc_resource_view_ref m_emitters_shader_view;
		graphic::bc_resource_view_ref m_particles_shader_view;
		graphic::bc_resource_view_ref m_particles_unordered_view;
		graphic::bc_resource_view_ref m_alive_particles1_shader_view;
		graphic::bc_resource_view_ref m_alive_particles1_unordered_view;
		graphic::bc_resource_view_ref m_alive_particles2_shader_view;
		graphic::bc_resource_view_ref m_alive_particles2_unordered_view;
		graphic::bc_resource_view_ref m_dead_particles_unordered_view;
		graphic::bc_resource_view_ref m_draw_args_shader_view;

		graphic::bc_device_compute_state_ref m_emission_compute_state;
		game::bc_compute_state_ptr m_emission_compute;
		graphic::bc_device_compute_state_ref m_simulation_compute_state;
		game::bc_compute_state_ptr m_simulation_compute;
		graphic::bc_device_compute_state_ref m_sort_group_compute_state;
		game::bc_compute_state_ptr m_sort_group_compute;
		graphic::bc_device_compute_state_ref m_sort_compute_state;
		game::bc_compute_state_ptr m_sort_compute;
		game::bc_compute_state_ptr m_sort1_after_transpose_compute;
		game::bc_compute_state_ptr m_sort2_after_transpose_compute;
		graphic::bc_device_compute_state_ref m_sort_transpose_compute_state;
		game::bc_compute_state_ptr m_sort_transpose1_compute;
		game::bc_compute_state_ptr m_sort_transpose2_compute;
		
		graphic::bc_sampler_state_ref m_linear_sampler;
		graphic::bc_sampler_state_ref m_point_sampler;
		graphic::bc_device_pipeline_state_ref m_render_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;
		graphic::bc_device_pipeline_state_ref m_merge_pipeline_state;
		game::bc_render_pass_state_ptr m_merge_pass_state;

		core::bc_query_result<game::bc_scene_particle_emitter_query> m_emitters_query;
		core::bc_query_result<game::bc_scene_light_query> m_lights_query;
		core::bc_vector_movable<game::bc_particle_emitter_state> m_emitters_query_result;
		core::bc_vector<game::bc_light_instance> m_lights_query_result;
	};
}