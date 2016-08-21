// [04/08/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"

#include "Core/bcConstant.h"
#include "Core/File/bcPath.h"
#include "GraphicImp/Loader/bcVertexShaderLoader.h"

namespace black_cat
{
	namespace graphic
	{
		bc_vertex_shader_loader::bc_vertex_shader_loader()
		{
		}

		bc_vertex_shader_loader::bc_vertex_shader_loader(bc_vertex_shader_loader&& p_other)
			: bc_base_content_loader(std::move(p_other))
		{
		}

		bc_vertex_shader_loader::~bc_vertex_shader_loader()
		{
		}

		bc_vertex_shader_loader& bc_vertex_shader_loader::operator=(bc_vertex_shader_loader&& p_other)
		{
			bc_base_content_loader::operator=(std::move(p_other));

			return *this;
		}

		void bc_vertex_shader_loader::content_offline_processing(core::bc_content_loader_context& p_context)
		{
			core::bc_string l_file_path = core::bc_to_exclusive_string(core::bc_path::get_absolute_path(p_context.m_file_path.c_str()));
			bc_device* l_device = p_context.m_parameter.get_value_throw< bc_device* >(core::g_param_device);
			core::bc_string& l_function = p_context.m_parameter.get_value_throw< core::bc_string >(core::g_param_shader_function);

			bc_compiled_shader_ptr l_compiled_shader = l_device->compile_vertex_shader
				(
					p_context.m_data.data(),
					p_context.m_data.size(),
					l_function.c_str(),
					l_file_path.c_str()
				);

			p_context.m_data.clear();
			p_context.m_data.reserve(l_compiled_shader->get_platform_pack().m_blob->GetBufferSize());
			std::memcpy
				(
					p_context.m_data.data(), l_compiled_shader->get_platform_pack().m_blob->GetBufferPointer(),
					l_compiled_shader->get_platform_pack().m_blob->GetBufferSize()
				);
		}

		void bc_vertex_shader_loader::content_processing(core::bc_content_loader_context& p_context)
		{
			bc_device* l_device = p_context.m_parameter.get_value_throw< bc_device* >(core::g_param_device);
			core::bc_string& l_function = p_context.m_parameter.get_value_throw< core::bc_string >(core::g_param_shader_function);

			bc_vertex_shader_ptr l_result = l_device->create_vertex_shader
				(
					p_context.m_data.data(), 
					p_context.m_data.size(), 
					l_function.c_str()
				);

			p_context.set_result(bc_vertex_shader_content(std::move(l_result)));
		}
	}
}
