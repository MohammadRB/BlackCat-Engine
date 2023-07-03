// [08/04/2016 MRB]

#include "App/AppPCH.h"

#include "Core/File/bcPath.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"
#include "App/Loader/bcGeometryShaderLoader.h"

namespace black_cat
{
	bc_geometry_shader_loader::bc_geometry_shader_loader()
	{
	}

	bc_geometry_shader_loader::bc_geometry_shader_loader(bc_geometry_shader_loader&& p_other) noexcept
		: bc_base_content_loader(std::move(p_other))
	{
	}

	bc_geometry_shader_loader::~bc_geometry_shader_loader()
	{
	}

	bc_geometry_shader_loader& bc_geometry_shader_loader::operator=(bc_geometry_shader_loader&& p_other) noexcept
	{
		bc_base_content_loader::operator=(std::move(p_other));

		return *this;
	}

	bool bc_geometry_shader_loader::support_offline_processing() const
	{
		return true;
	}

	void bc_geometry_shader_loader::content_offline_processing(core::bc_content_loading_context& p_context) const
	{
		graphic::bc_device& l_device = core::bc_get_service<game::bc_game_system>()->get_render_system().get_device();
		const auto l_file_path = core::bc_to_exclusive_string(p_context.m_file_path.data());
		const auto* l_function = static_cast<core::bc_string*>(nullptr);
		const auto* l_macros_value = static_cast<core::bc_json_key_value*>(nullptr);

		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_shader_function, l_function);
		json_parse::bc_load(p_context.m_parameters, constant::g_param_shader_macro, l_macros_value);

		core::bc_vector<graphic::bc_shader_macro> l_macros_buffer;
		const graphic::bc_shader_macro* l_macros = nullptr;
		bcUINT32 l_macros_count = 0;

		if (l_macros_value)
		{
			l_macros_buffer.reserve(l_macros_value->size());

			for (core::bc_json_key_value::value_type& l_key_value : *l_macros_value)
			{
				l_macros_buffer.push_back
				(
					graphic::bc_shader_macro
					{
						l_key_value.first.c_str(),
						l_key_value.second.as_throw< core::bc_string >().c_str()
					}
				);
			}

			l_macros = l_macros_buffer.data();
			l_macros_count = l_macros_buffer.size();
		}

		graphic::bc_compiled_shader_ptr l_compiled_shader = l_device.compile_geometry_shader
		(
			p_context.m_file_buffer.get(),
			p_context.m_file_buffer_size,
			l_function->c_str(),
			l_file_path.c_str(),
			l_macros,
			l_macros_count
		);

		p_context.m_file.write
		(
			static_cast<bcBYTE*>(l_compiled_shader->get_platform_pack().m_blob->GetBufferPointer()),
			l_compiled_shader->get_platform_pack().m_blob->GetBufferSize()
		);
	}

	void bc_geometry_shader_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
		auto& l_device = core::bc_get_service<game::bc_game_system>()->get_render_system().get_device();
		const auto* l_function = static_cast<core::bc_string*>(nullptr);

		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_shader_function, l_function);

		graphic::bc_geometry_shader_ref l_result = l_device.create_geometry_shader
		(
			p_context.m_file_buffer.get(),
			p_context.m_file_buffer_size,
			l_function->c_str()
		);

		p_context.set_result(graphic::bc_geometry_shader_content(std::move(l_result)));
	}
}