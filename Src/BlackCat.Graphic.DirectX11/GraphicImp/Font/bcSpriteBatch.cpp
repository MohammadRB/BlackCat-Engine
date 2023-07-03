// [08/02/2022 MRB]

#include "GraphicImp/GraphicImpPCH.h"

#include "Core/Utility/bcEnumOperand.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Font/bcSpriteBatch.h"
#include "GraphicImp/bcExport.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_batch<g_api_dx11>::bc_platform_sprite_batch(platform_pack p_parameter) noexcept
			: m_pack(std::move(p_parameter))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_batch<g_api_dx11>::bc_platform_sprite_batch(bc_platform_sprite_batch&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_batch<g_api_dx11>::~bc_platform_sprite_batch()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_batch<g_api_dx11>& bc_platform_sprite_batch<g_api_dx11>::operator=(bc_platform_sprite_batch&& p_other) noexcept
		{
			m_pack = std::move(p_other.m_pack);
			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bcSIZE bc_platform_sprite_batch<g_api_dx11>::create_blend_state(bc_device& p_device, const bc_blend_state_config& p_blend_state)
		{
			D3D11_BLEND_DESC l_blend_state_desc;

			l_blend_state_desc.AlphaToCoverageEnable = p_blend_state.m_alpha_to_coverage_enable;
			l_blend_state_desc.IndependentBlendEnable = p_blend_state.m_independent_blend_enable;
			for (bcUINT i = 0; i < bc_render_api_info::number_of_om_render_target_slots(); ++i)
			{
				l_blend_state_desc.RenderTarget[i].BlendEnable = p_blend_state.m_render_target[i].m_blend_enable;
				l_blend_state_desc.RenderTarget[i].SrcBlend = bc_graphic_cast(p_blend_state.m_render_target[i].m_src_blend);
				l_blend_state_desc.RenderTarget[i].DestBlend = bc_graphic_cast(p_blend_state.m_render_target[i].m_dest_blend);
				l_blend_state_desc.RenderTarget[i].BlendOp = bc_graphic_cast(p_blend_state.m_render_target[i].m_blend_op);
				l_blend_state_desc.RenderTarget[i].SrcBlendAlpha = bc_graphic_cast(p_blend_state.m_render_target[i].m_src_blend_alpha);
				l_blend_state_desc.RenderTarget[i].DestBlendAlpha = bc_graphic_cast(p_blend_state.m_render_target[i].m_dest_blend_alpha);
				l_blend_state_desc.RenderTarget[i].BlendOpAlpha = bc_graphic_cast(p_blend_state.m_render_target[i].m_blend_op_alpha);
				l_blend_state_desc.RenderTarget[i].RenderTargetWriteMask = p_blend_state.m_render_target[0].m_render_target_write_mask;
			}

			Microsoft::WRL::ComPtr<ID3D11BlendState> l_blend_state;
			dx_call(p_device.get_platform_pack().m_device->CreateBlendState(&l_blend_state_desc, l_blend_state.GetAddressOf()));

			m_pack.m_blend_states.push_back(l_blend_state);
			return m_pack.m_blend_states.size() - 1;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_sprite_batch<g_api_dx11>::begin(bc_device_pipeline& p_device_pipeline, const bc_texture2d& p_render_texture, bc_render_target_view& p_render_target, bcSIZE p_blend_state) noexcept
		{
			const auto l_back_buffer_width = p_render_texture.get_width();
			const auto l_back_buffer_height = p_render_texture.get_height();

			bc_viewport l_viewport;
			l_viewport.m_top_left_x = 0;
			l_viewport.m_top_left_y = 0;
			l_viewport.m_width = l_back_buffer_width;
			l_viewport.m_height = l_back_buffer_height;
			l_viewport.m_min_depth = 0;
			l_viewport.m_max_depth = 1;
			
			p_device_pipeline.start_command_list();
			p_device_pipeline.bind_om_render_targets(1, &p_render_target, bc_depth_stencil_view());
			p_device_pipeline.bind_rs_viewports(1, &l_viewport);
			p_device_pipeline.pipeline_apply_states(core::bc_enum::mask_or({ bc_pipeline_stage::output_merger_stage, bc_pipeline_stage::rasterizer_stage }));
			m_pack.m_sprite_batch->Begin(DirectX::SpriteSortMode_Deferred, p_blend_state != -1 ? m_pack.m_blend_states[p_blend_state].Get() : nullptr);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_sprite_batch<g_api_dx11>::end(bc_device_pipeline& p_device_pipeline) noexcept
		{
			bc_device_command_list l_command_list;

			m_pack.m_sprite_batch->End();
			p_device_pipeline.bind_om_render_targets(0, nullptr, bc_depth_stencil_view());
			p_device_pipeline.bind_rs_viewports(0, nullptr);
			p_device_pipeline.pipeline_apply_states(core::bc_enum::mask_or({ bc_pipeline_stage::output_merger_stage, bc_pipeline_stage::rasterizer_stage }));
			p_device_pipeline.finish_command_list(l_command_list);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_sprite_batch<g_api_dx11>::draw(bc_resource_view& p_texture, const core::bc_vector2i& p_position, const core::bc_vector2i& p_origin, bcFLOAT p_rotation)
		{
			const auto l_position = DirectX::XMFLOAT2(p_position.x, p_position.y);
			const auto l_origin = DirectX::XMFLOAT2(p_origin.x, p_origin.y);

			m_pack.m_sprite_batch->Draw(p_texture.get_platform_pack().m_shader_view, l_position, nullptr, DirectX::Colors::White, p_rotation, l_origin);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_sprite_batch<g_api_dx11>::draw(bc_resource_view& p_texture, const bc_sprite_rect& p_destination_rect, bcFLOAT p_rotation)
		{
			const auto l_destination_rect = RECT{ p_destination_rect.m_left, p_destination_rect.m_top, p_destination_rect.m_right, p_destination_rect.m_bottom };

			m_pack.m_sprite_batch->Draw(p_texture.get_platform_pack().m_shader_view, l_destination_rect, nullptr, DirectX::Colors::White, p_rotation);
		}
	}
}
