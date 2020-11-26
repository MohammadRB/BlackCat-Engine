// [11/16/2020 MRB]

#pragma once

#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcComputeState.h"
#include "Game/System/Render/Pass/bcRenderPassState.h"
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
		static const bcSIZE m_emitters_count = 128;
		static const bcSIZE m_particles_count = 8192;
		static const bcSIZE m_emission_shader_group_size = 1;
		static const bcSIZE m_simulation_shader_group_size = 16;
		static const bcSIZE m_sort_shader_group_size = 16;

		bcINT32 m_dead_particles_counter = m_particles_count;
		
		graphic::bc_device_command_list_ptr m_command_list;
		
		graphic::bc_resource_view m_depth_buffer_view;
		graphic::bc_buffer_ptr m_emitters_buffer;
		graphic::bc_buffer_ptr m_particles_buffer;
		graphic::bc_buffer_ptr m_alive_particles_buffer;
		graphic::bc_buffer_ptr m_dead_particles_buffer;
		graphic::bc_buffer_ptr m_draw_args_buffer;

		graphic::bc_resource_view_ptr m_emitters_shader_view;
		graphic::bc_resource_view_ptr m_particles_shader_view;
		graphic::bc_resource_view_ptr m_particles_unordered_view;
		graphic::bc_resource_view_ptr m_alive_particles_shader_view;
		graphic::bc_resource_view_ptr m_alive_particles_unordered_view;
		graphic::bc_resource_view_ptr m_dead_particles_unordered_view;
		graphic::bc_resource_view_ptr m_draw_args_unordered_view;

		graphic::bc_device_compute_state_ptr m_emission_compute_state;
		game::bc_compute_state_ptr m_emission_compute;
		graphic::bc_device_compute_state_ptr m_simulation_compute_state;
		game::bc_compute_state_ptr m_simulation_compute;
		graphic::bc_device_compute_state_ptr m_sort_compute_state;
		game::bc_compute_state_ptr m_sort_compute;

		graphic::bc_sampler_state_ptr m_sampler;
		graphic::bc_device_pipeline_state_ptr m_device_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;
		
		core::bc_query_result<game::bc_particle_emitter_query> m_emitters_query;
		core::bc_vector_movable< game::bc_particle_emitter> m_emitters_query_result;
	};
}