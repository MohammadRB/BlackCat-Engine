// [04/29/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Device/bcDeviceOcclusionQuery.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Shader/Parameter/bcShaderParameterLink.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Render/Light/bcLightInstance.h"
#include "Game/Query/bcMainCameraSceneLightQuery.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	struct _bc_light_flare_query_instance
	{
		bool m_free;
		game::bc_light_instance::id_t m_light_id;
		graphic::bc_device_occlusion_query_ref m_device_query;
	};

	struct _bc_light_flare_instance
	{
		game::bc_light_instance* m_light;
		core::bc_vector3f m_position;
		core::bc_vector3f m_color;
		bcFLOAT m_size;
		bcFLOAT m_intensity;
		bcUINT32 m_texture_index;
		bcFLOAT m_u0;
		bcFLOAT m_v0;
		bcFLOAT m_u1;
		bcFLOAT m_v1;
	};
	
	class BC_DLL bc_light_flare_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(lgt_flr)
		
	public:
		bc_light_flare_pass();
		
		void initialize_resources(game::bc_render_system& p_render_system) override;
		
		void update(const game::bc_render_pass_update_context& p_context) override;
		
		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;
		
		void execute(const game::bc_render_pass_render_context& p_context) override;
		
		void before_reset(const game::bc_render_pass_reset_context& p_context) override;
		
		void after_reset(const game::bc_render_pass_reset_context& p_context) override;
		
		void destroy(game::bc_render_system& p_render_system) override;

	private:
		void _update_light_queries(const game::bc_render_pass_render_context& p_context);
		
		void _query_light(const game::bc_render_pass_render_context& p_context, game::bc_light_instance& p_light, _bc_light_flare_query_instance& p_query);
		
		_bc_light_flare_query_instance* _find_free_query(graphic::bc_device& p_device);

		void _draw_flares(const game::bc_render_pass_render_context& p_context);
		
		constexpr static bcUINT32 s_per_draw_flare_count = 50U;
		constexpr static bcUINT32 s_per_draw_texture_count = 10U;
		constexpr static bcFLOAT s_flare_size_ratio = 0.9;
		constexpr static bcFLOAT s_flare_size_distance = 5;
		bcFLOAT m_flare_size_distance;
		
		graphic::bc_sampler_state_ref m_linear_sampler;
		graphic::bc_buffer_ref m_cube_vb;
		graphic::bc_buffer_ref m_cube_ib;
		graphic::bc_buffer_ref m_flare_instances_buffer;
		graphic::bc_resource_view_ref m_flare_instances_view;
		graphic::bc_shader_parameter_link m_flare_texture_parameters[s_per_draw_texture_count];
		graphic::bc_device_pipeline_state_ref m_query_device_pipeline_state;
		game::bc_render_pass_state_ptr m_query_render_pass_state;
		game::bc_render_state_ptr m_query_render_state;
		graphic::bc_device_pipeline_state_ref m_flare_device_pipeline_state;
		game::bc_render_pass_state_ptr m_flare_render_pass_state;
		game::bc_render_state_ptr m_flare_render_state;
		
		core::bc_query_result<game::bc_main_camera_scene_light_query> m_scene_light_query;
		core::bc_vector<game::bc_light_instance> m_scene_light_query_result;
		core::bc_vector<_bc_light_flare_query_instance> m_occlusion_queries;
		core::bc_vector_frame<_bc_light_flare_instance> m_ready_to_draw_instances;
	};
}