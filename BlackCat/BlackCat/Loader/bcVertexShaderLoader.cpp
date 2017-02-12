// [04/08/2016 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/bcConstant.h"
#include "Core/File/bcPath.h"
#include "GraphicImp/Shader/bcShader.h"
#include "GraphicImp/Shader/bcVertexShader.h"
#include "Game/System/bcGameSystem.h"
#include "BlackCat/Loader/bcVertexShaderLoader.h"

namespace black_cat
{
	bc_vertex_shader_loader::bc_vertex_shader_loader()
	{
	}

	bc_vertex_shader_loader::bc_vertex_shader_loader(bc_vertex_shader_loader&& p_other) noexcept
		: bc_base_content_loader(std::move(p_other))
	{
	}

	bc_vertex_shader_loader::~bc_vertex_shader_loader()
	{
	}

	bc_vertex_shader_loader& bc_vertex_shader_loader::operator=(bc_vertex_shader_loader&& p_other) noexcept
	{
		bc_base_content_loader::operator=(std::move(p_other));

		return *this;
	}

	void bc_vertex_shader_loader::content_offline_processing(core::bc_content_loader_context& p_context) const
	{
		graphic::bc_device& l_device = core::bc_service_manager::get().get_service< game::bc_game_system >()->get_render_system().get_device();
		core::bc_string l_file_path = core::bc_to_exclusive_string(core::bc_path::get_absolute_path(p_context.m_file_path.c_str()));
		core::bc_string& l_function = p_context.m_parameter.get_value_throw< core::bc_string >(core::g_param_shader_function);

		graphic::bc_compiled_shader_ptr l_compiled_shader = l_device.compile_vertex_shader
		(
			p_context.m_data.data(),
			p_context.m_data.size(),
			l_function.c_str(),
			l_file_path.c_str()
		);

		p_context.m_data = core::bc_vector_frame< bcBYTE >(l_compiled_shader->get_platform_pack().m_blob->GetBufferSize());

		std::memcpy
		(
			p_context.m_data.data(), 
			l_compiled_shader->get_platform_pack().m_blob->GetBufferPointer(),
			l_compiled_shader->get_platform_pack().m_blob->GetBufferSize()
		);
	}

	void bc_vertex_shader_loader::content_processing(core::bc_content_loader_context& p_context) const
	{
		graphic::bc_device& l_device = core::bc_service_manager::get().get_service< game::bc_game_system >()->get_render_system().get_device();
		core::bc_string& l_function = p_context.m_parameter.get_value_throw< core::bc_string >(core::g_param_shader_function);

		graphic::bc_vertex_shader_ptr l_result = l_device.create_vertex_shader
		(
			p_context.m_data.data(),
			p_context.m_data.size(),
			l_function.c_str()
		);

		p_context.set_result(graphic::bc_vertex_shader_content(std::move(l_result)));
	}
}
