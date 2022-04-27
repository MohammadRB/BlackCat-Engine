// [04/08/2016 MRB]

#include "App/AppPCH.h"

#include "Core/bcException.h"
#include "Core/Container/bcVector.h"
#include "Core/File/bcPath.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcJsonParse.h"
#include "App/bcConstant.h"
#include "App/Loader/bcTextureLoader.h"

namespace black_cat
{
	graphic::bc_texture_config bc_texture_loader::s_default_config = graphic::bc_graphic_resource_builder().as_resource().as_texture2d
	(
		0,
		0,
		false, // DX11 requires shader_bind_flag & render_target_bind_flag to be set for auto mip generation
		0,
		graphic::bc_format::unknown,
		graphic::bc_resource_usage::gpu_r,
		graphic::bc_resource_view_type::shader
	)
	.as_normal_texture();

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

	bool bc_texture_loader::support_offline_processing() const
	{
		return false;
	}

	void bc_texture_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
		const auto l_path = core::bc_path(p_context.m_file_path);
		auto l_extension = l_path.get_file_extension();
		graphic::bc_image_format l_format;

		std::transform(std::begin(l_extension), std::end(l_extension), std::begin(l_extension), towlower);

		if (l_extension == bcL(".dds"))
		{
			l_format = graphic::bc_image_format::dds;
		}
		else if (l_extension == bcL(".bmp"))
		{
			l_format = graphic::bc_image_format::bmp;
		}
		else if (l_extension == bcL(".png"))
		{
			l_format = graphic::bc_image_format::png;
		}
		else if (l_extension == bcL(".gif"))
		{
			l_format = graphic::bc_image_format::gif;
		}
		else if (l_extension == bcL(".tiff"))
		{
			l_format = graphic::bc_image_format::tiff;
		}
		else if (l_extension == bcL(".jpg"))
		{
			l_format = graphic::bc_image_format::jpg;
		}
		else
		{
			throw bc_invalid_argument_exception((core::bc_to_string(p_context.m_file_path.data()) + " Unknown image file format").c_str());
		}

		auto& l_device = core::bc_get_service<game::bc_game_system>()->get_render_system().get_device();
		const auto* l_config = static_cast<graphic::bc_texture_config*>(nullptr);
		json_parse::bc_load(p_context.m_instance_parameters, constant::g_param_texture_config, l_config);

		l_config = l_config ? l_config : &s_default_config;

		auto l_result = l_device.create_texture2d
		(
			*const_cast<graphic::bc_texture_config*>(l_config),
			p_context.m_file_buffer.get(),
			p_context.m_file_buffer_size,
			l_format
		);

		p_context.set_result(graphic::bc_texture2d_content(std::move(l_result)));
	}

	void bc_texture_loader::content_processing(core::bc_content_saving_context& p_context) const
	{
		auto* l_game_system = core::bc_get_service<game::bc_game_system>();
		auto& l_device = l_game_system->get_render_system().get_device();
		auto& l_texture_content = static_cast<graphic::bc_texture2d_content&>(p_context.m_content);

		// Close file handle so device can write to the file
		p_context.m_file.close();

		l_device.save_texture2d(l_texture_content.get_resource(), graphic::bc_image_format::dds, p_context.m_file_path.data());
	}
}