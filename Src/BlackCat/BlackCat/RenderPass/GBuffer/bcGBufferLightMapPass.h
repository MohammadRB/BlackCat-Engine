// [03/15/2019 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Shader/Parameter/bcShaderParameter.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcComputeState.h"
#include "Game/Query/bcSceneLightQuery.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_gbuffer_light_map_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(gb_lgt)

	public:
		bc_gbuffer_light_map_pass(constant::bc_render_pass_variable_t p_csm_buffers_container, constant::bc_render_pass_variable_t p_output_texture);

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_param) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_param) override;

		void execute(const game::bc_render_pass_render_context& p_param) override;

		void before_reset(const game::bc_render_pass_reset_context& p_param) override;

		void after_reset(const game::bc_render_pass_reset_context& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;
		
	private:
		constexpr static bcSIZE m_shader_thread_group_size = 16;
		constexpr static bcSIZE m_shader_shadow_map_cascade_count = 4;
		constexpr static bcSIZE m_shader_shadow_map_array_count = 1;
		constexpr static bcSIZE m_shader_shadow_map_count = 1;
		constexpr static bcSIZE m_num_direct_lights = 2;
		constexpr static bcSIZE m_num_point_lights = 100;
		constexpr static bcSIZE m_num_spot_lights = 100;

		constant::bc_render_pass_variable_t m_csm_buffers_container_share_slot;
		constant::bc_render_pass_variable_t m_output_texture_share_slot;

		graphic::bc_resource_view_ptr m_depth_stencil_view;
		graphic::bc_resource_view_ptr m_diffuse_map_view;
		graphic::bc_resource_view_ptr m_normal_map_view;
		graphic::bc_resource_view_ptr m_specular_map_view;

		graphic::bc_sampler_state_ptr m_pcf_sampler;

		graphic::bc_buffer_ptr m_parameters_cbuffer;

		graphic::bc_buffer_ptr m_direct_lights_buffer;
		graphic::bc_resource_view_ptr m_direct_lights_buffer_view;
		graphic::bc_buffer_ptr m_point_lights_buffer;
		graphic::bc_resource_view_ptr m_point_lights_buffer_view;
		graphic::bc_buffer_ptr m_spot_lights_buffer;
		graphic::bc_resource_view_ptr m_spot_lights_buffer_view;
		graphic::bc_buffer_ptr m_shadow_maps_buffer;
		graphic::bc_resource_view_ptr m_shadow_maps_buffer_view;

		core::bc_array<graphic::bc_shader_parameter_link, m_shader_shadow_map_array_count + m_shader_shadow_map_count> m_shadow_map_parameters;

		graphic::bc_texture2d_ptr m_output_texture;
		graphic::bc_resource_view_ptr m_output_texture_unordered_view;

		graphic::bc_device_compute_state_ptr m_device_compute_state;
		game::bc_compute_state_ptr m_compute_state;

		core::bc_query_result<game::bc_scene_light_query> m_lights_query;
		core::bc_vector_movable<game::bc_light_instance> m_lights;
	};
}
