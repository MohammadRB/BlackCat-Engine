// [04/08/2016 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/bcConstant.h"
#include "Core/bcException.h"
#include "Core/Container/bcVector.h"
#include "Core/File/bcPath.h"
#include "GraphicImp/Resource/bcResourceConfig.h"
#include "Game/System/bcGameSystem.h"
#include "BlackCat/Loader/bcTextureLoader.h"

namespace black_cat
{
	bc_texture_loader::bc_texture_loader()
		: bc_base_content_loader()
	{
	}

	bc_texture_loader::bc_texture_loader(bc_texture_loader&& p_other) noexcept
		: bc_base_content_loader(std::move(p_other))
	{
	}

	bc_texture_loader::~bc_texture_loader()
	{
	}

	bc_texture_loader& bc_texture_loader::operator=(bc_texture_loader&& p_other) noexcept
	{
		bc_base_content_loader::operator=(std::move(p_other));

		return *this;
	}

	void bc_texture_loader::content_offline_processing(core::bc_content_loader_context& p_context)
	{
	}

	void bc_texture_loader::content_processing(core::bc_content_loader_context& p_context)
	{
		core::bc_path l_path(p_context.m_file_path.c_str());
		core::bc_estring l_extension = l_path.get_file_extension();
		graphic::bc_image_format l_format;

		// Bug tolower operate only on single byte chars
		std::transform(std::begin(l_extension), std::end(l_extension), std::begin(l_extension), tolower);

		if (l_extension.compare(bcL(".dds")) == 0)
		{
			l_format = graphic::bc_image_format::dds;
		}
		else if (l_extension.compare(bcL(".bmp")) == 0)
		{
			l_format = graphic::bc_image_format::bmp;
		}
		else if (l_extension.compare(bcL(".png")) == 0)
		{
			l_format = graphic::bc_image_format::png;
		}
		else if (l_extension.compare(bcL(".gif")) == 0)
		{
			l_format = graphic::bc_image_format::gif;
		}
		else if (l_extension.compare(bcL(".tiff")) == 0)
		{
			l_format = graphic::bc_image_format::tiff;
		}
		else if (l_extension.compare(bcL(".jpg")) == 0)
		{
			l_format = graphic::bc_image_format::jpg;
		}
		else
		{
			throw bc_invalid_argument_exception((core::bc_to_string(p_context.m_file_path) + " Unknown image file format").c_str());
		}

		graphic::bc_device* l_device = &core::bc_service_manager::get().get_service< game::bc_game_system >()->get_render_system().get_device();
		graphic::bc_texture_config l_config = graphic::bc_graphic_resource_configure().as_resource().as_texture2d
			(
				0,
				0,
				false, // DX11 requires shader_bind_flag & render_target_bind_flag to be set for auto mip generation
				0,
				graphic::bc_format::R8G8B8A8_UNORM,
				graphic::bc_resource_usage::gpu_r,
				graphic::bc_resource_view_type::shader
			)
			.as_normal_texture();

		graphic::bc_texture2d_ptr l_result = l_device->create_texture2d
			(
				l_config,
				p_context.m_data.data(),
				p_context.m_data.size(),
				l_format
			);

		p_context.set_result(graphic::bc_texture2d_content(std::move(l_result)));
	}
}
