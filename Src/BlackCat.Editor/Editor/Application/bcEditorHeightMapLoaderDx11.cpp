// [02/27/2017 MRB]

#include "Editor/EditorPCH.h"

#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/bcGameSystem.h"
#include "Editor/Application/bcEditorHeightMapLoaderDx11.h"

namespace black_cat
{
	namespace editor
	{
		void bc_editor_height_map_loader_dx11::content_processing(core::bc_content_loading_context& p_context) const
		{
			game::bc_game_system& l_game_system = *core::bc_get_service<game::bc_game_system>();
			game::bc_render_system& l_render_system = l_game_system.get_render_system();

			bc_height_map_loader_dx11::content_processing(p_context);

			auto l_loaded_height_map = p_context.m_result.get().release_result<bc_height_map_dx11>();
			auto l_height_map = l_loaded_height_map->get_height_map();
			auto l_texture_map = l_loaded_height_map->get_texture_map();

			auto l_height_map_unordered_view_config = graphic::bc_graphic_resource_builder().as_resource_view()
				.as_texture_view(l_height_map.get_format())
				.as_tex2d_unordered_shader_view(0)
				.on_texture2d();
			auto l_texture_map_unordered_view_config = graphic::bc_graphic_resource_builder().as_resource_view()
				.as_texture_view(l_texture_map.get_format())
				.as_tex2d_unordered_shader_view(0)
				.on_texture2d();

			auto l_height_map_unordered_view = l_render_system.get_device()
				.create_resource_view(l_height_map, l_height_map_unordered_view_config);
			auto l_texture_map_unordered_view = l_render_system.get_device()
				.create_resource_view(l_texture_map, l_texture_map_unordered_view_config);

			p_context.set_result(bc_editor_height_map_dx11
			(
				std::move(*l_loaded_height_map.release()),
				l_height_map_unordered_view,
				l_texture_map_unordered_view
			));
		}

		std::pair< graphic::bc_texture_config, graphic::bc_resource_view_config > bc_editor_height_map_loader_dx11::get_height_map_texture_config(bcUINT32 p_width, bcUINT32 p_height) const
		{
			// Because in editor we need unordered view on height map texture and DX11.0 doesn't support typed UAV load on R16_SINT format, we use R32_SINT
			auto l_texture_config = graphic::bc_graphic_resource_builder()
				.as_resource()
				.as_texture2d
				(
					p_width,
					p_height,
					false,
					1,
					graphic::bc_format::R32_FLOAT,
					graphic::bc_resource_usage::gpu_rw,
					core::bc_enum::mask_or({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::unordered })
				)
				.as_normal_texture();
			auto l_view_config = graphic::bc_graphic_resource_builder()
				.as_resource_view()
				.as_texture_view(l_texture_config.get_format())
				.as_tex2d_shader_view(0, 1)
				.on_texture2d();

			return std::make_pair(l_texture_config, l_view_config);
		}

		std::pair< graphic::bc_texture_config, graphic::bc_resource_view_config > bc_editor_height_map_loader_dx11::get_texture_map_texture_config(bcUINT32 p_width, bcUINT32 p_height) const
		{
			auto l_texture_config = graphic::bc_graphic_resource_builder().as_resource()
				.as_texture2d
				(
					p_width,
					p_height,
					false,
					1,
					graphic::bc_format::R16G16B16A16_UINT,
					graphic::bc_resource_usage::gpu_rw,
					core::bc_enum::mask_or({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::unordered })
				)
				.as_normal_texture();
			auto l_view_config = graphic::bc_graphic_resource_builder()
				.as_resource_view()
				.as_texture_view(l_texture_config.get_format())
				.as_tex2d_shader_view(0, 1)
				.on_texture2d();

			return std::make_pair(l_texture_config, l_view_config);
		}
	}
}