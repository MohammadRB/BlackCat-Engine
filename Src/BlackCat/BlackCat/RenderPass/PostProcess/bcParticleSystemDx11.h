// [11/16/2020 MRB]

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
#include "Game/Query/bcParticleEmittersQuery.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_particle_system_dx11 : public game::bc_irender_pass
	{
		BC_RENDER_PASS(ptc_sys)
		
	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;
		
		void update(const game::bc_render_pass_update_param& p_param) override;
		
		void initialize_frame(const game::bc_render_pass_render_param& p_param) override;
		
		void execute(const game::bc_render_pass_render_param& p_param) override;
		
		void before_reset(const game::bc_render_pass_reset_param& p_param) override;
		
		void after_reset(const game::bc_render_pass_reset_param& p_param) override;
		
		void destroy(game::bc_render_system& p_render_system) override;

	private:
		void _execute_sort_shader(const game::bc_render_pass_render_param& p_param);
		
		// The number of elements to sort is limited to an even power of 2
		// At minimum - m_sort_shader_group_size * m_sort_transpose_shader_group_size
		// At maximum - m_sort_shader_group_size * m_sort_shader_group_size
		static const bcSIZE m_emitters_count = 128;
		static const bcSIZE m_particles_count = 128 * 128;
		static const bcSIZE m_emission_shader_group_size = 1;
		static const bcSIZE m_simulation_shader_group_size = 1024;
		static const bcSIZE m_sort_shader_group_size = 1024;
		static const bcSIZE m_sort_transpose_shader_group_size = 32;
		static const bcSIZE m_sort_transpose_matrix_width = m_sort_shader_group_size;
		static const bcSIZE m_sort_transpose_matrix_height = m_particles_count / m_sort_shader_group_size;
		
		bcINT32 m_dead_particles_initial_count = m_particles_count;

		graphic::bc_depth_stencil_view m_depth_buffer_view;
		graphic::bc_resource_view m_depth_buffer_shader_view;
		
		graphic::bc_buffer_ptr m_emitters_buffer;
		graphic::bc_buffer_ptr m_particles_buffer;
		graphic::bc_buffer_ptr m_alive_particles_buffer1;
		graphic::bc_buffer_ptr m_alive_particles_buffer2;
		graphic::bc_buffer_ptr m_dead_particles_buffer;
		graphic::bc_buffer_ptr m_draw_args_buffer;
		graphic::bc_buffer_ptr m_sort_cbuffer;

		graphic::bc_resource_view_ptr m_emitters_shader_view;
		graphic::bc_resource_view_ptr m_particles_shader_view;
		graphic::bc_resource_view_ptr m_particles_unordered_view;
		graphic::bc_resource_view_ptr m_alive_particles1_shader_view;
		graphic::bc_resource_view_ptr m_alive_particles1_unordered_view;
		graphic::bc_resource_view_ptr m_alive_particles2_shader_view;
		graphic::bc_resource_view_ptr m_alive_particles2_unordered_view;
		graphic::bc_resource_view_ptr m_dead_particles_unordered_view;
		graphic::bc_resource_view_ptr m_draw_args_shader_view;

		graphic::bc_device_compute_state_ptr m_emission_compute_state;
		game::bc_compute_state_ptr m_emission_compute;
		graphic::bc_device_compute_state_ptr m_simulation_compute_state;
		game::bc_compute_state_ptr m_simulation_compute;
		graphic::bc_device_compute_state_ptr m_sort_compute_state;
		game::bc_compute_state_ptr m_sort_compute;
		game::bc_compute_state_ptr m_sort1_after_transpose_compute;
		game::bc_compute_state_ptr m_sort2_after_transpose_compute;
		graphic::bc_device_compute_state_ptr m_sort_transpose_compute_state;
		game::bc_compute_state_ptr m_sort_transpose1_compute;
		game::bc_compute_state_ptr m_sort_transpose2_compute;

		graphic::bc_sampler_state_ptr m_sampler;
		graphic::bc_device_pipeline_state_ptr m_device_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;
		
		core::bc_query_result<game::bc_particle_emitter_query> m_emitters_query;
		core::bc_vector_movable< game::bc_particle_emitter> m_emitters_query_result;
	};
}