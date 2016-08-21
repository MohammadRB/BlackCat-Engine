// [01/17/2016 MRB]

#include "Graphic/GraphicPCH.h"
#include  "Graphic/Shader/Parameter/bcShaderParameterManager.h"

namespace black_cat
{
	namespace graphic
	{
		bc_shader_parameter_manager::_parameter_entry::_parameter_entry(const bc_shader_type p_shader_type, const bcINT p_register)
			: m_shader_type(p_shader_type),
			m_register(p_register)
		{
		}

		bool bc_shader_parameter_manager::_parameter_entry::operator<(const _parameter_entry& p_other) const
		{
			return ((static_cast<bcUINT>(m_shader_type) * 100) + m_register) < ((static_cast<bcUINT>(p_other.m_shader_type) * 100) + p_other.m_register);
		}

		bc_sampler_parameter* bc_shader_parameter_manager::get_sampler_parameter_ref(bc_shader_type p_shader_type, bcINT p_register) const
		{
			bc_sampler_parameter* l_param = _get_param_ref<bc_shader_parameter_type::sampler, bc_sampler_parameter>(p_shader_type, p_register);
			return l_param;
		}

		bc_shader_view_parameter* bc_shader_parameter_manager::get_shader_resource_parameter_ref(bc_shader_type p_shader_type, bcINT p_register) const
		{
			bc_shader_view_parameter* l_param = _get_param_ref<bc_shader_parameter_type::shader_resource, bc_shader_view_parameter>(p_shader_type, p_register);
			return l_param;
		}

		bc_constant_buffer_parameter* bc_shader_parameter_manager::get_constant_buffer_parameter_ref(bc_shader_type p_shader_type, bcINT p_register) const
		{
			bc_constant_buffer_parameter* l_param = _get_param_ref<bc_shader_parameter_type::cbuffer, bc_constant_buffer_parameter>(p_shader_type, p_register);
			return l_param;
		}

		bc_sampler_state* bc_shader_parameter_manager::get_sampler_parameter(bc_shader_type p_shader_type, bcINT p_register) const
		{
			bc_sampler_parameter* l_parameter = get_sampler_parameter_ref(p_shader_type, p_register);

			if (l_parameter != nullptr)
				return l_parameter->get_sampler();

			return nullptr;
		}

		bc_shader_view* bc_shader_parameter_manager::get_shader_resource_parameter(bc_shader_type p_shader_type, bcINT p_register) const
		{
			bc_shader_view_parameter* l_parameter = get_shader_resource_parameter_ref(p_shader_type, p_register);

			if (l_parameter != nullptr)
				return l_parameter->get_shader_resource();

			return nullptr;
		}

		bc_buffer* bc_shader_parameter_manager::get_constant_buffer_parameter(bc_shader_type p_shader_type, bcINT p_register) const
		{
			bc_constant_buffer_parameter* l_parameter = get_constant_buffer_parameter_ref(p_shader_type, p_register);

			if (l_parameter != nullptr)
				return l_parameter->get_buffer();

			return nullptr;
		}

		bc_sampler_state* bc_shader_parameter_manager::get_sampler_parameter(bc_ishader_parameter* p_parameter) const
		{
			bc_sampler_state* l_parameter = nullptr;

			if (p_parameter->get_parameter_type() == bc_shader_parameter_type::sampler)
				l_parameter = static_cast<bc_sampler_parameter*>(p_parameter)->get_sampler();

			return l_parameter;
		}

		bc_shader_view* bc_shader_parameter_manager::get_shader_resource_parameter(bc_ishader_parameter* p_parameter) const
		{
			bc_shader_view* l_parameter = nullptr;

			if (p_parameter->get_parameter_type() == bc_shader_parameter_type::sampler)
				l_parameter = static_cast<bc_shader_view_parameter*>(p_parameter)->get_shader_resource();

			return l_parameter;
		}

		bc_buffer* bc_shader_parameter_manager::get_constant_buffer_parameter(bc_ishader_parameter* p_parameter) const
		{
			bc_buffer* l_parameter = nullptr;

			if (p_parameter->get_parameter_type() == bc_shader_parameter_type::sampler)
				l_parameter = static_cast<bc_constant_buffer_parameter*>(p_parameter)->get_buffer();

			return l_parameter;
		}

		void bc_shader_parameter_manager::set_sampler_parameter(bc_shader_type p_shader_type, bcINT p_register, bc_sampler_state* p_sampler)
		{
			bc_ishader_parameter* l_parameter = _set_param_value<bc_sampler_parameter>(p_shader_type, p_register, p_sampler);
			bcAssert(l_parameter->get_parameter_type() == bc_shader_parameter_type::sampler);
		}

		void bc_shader_parameter_manager::set_shader_resource_parameter(bc_shader_type p_shader_type, bcINT p_register, bc_shader_view* p_resource)
		{
			bc_ishader_parameter* l_parameter = _set_param_value<bc_shader_view_parameter>(p_shader_type, p_register, p_resource);
			bcAssert(l_parameter->get_parameter_type() == bc_shader_parameter_type::shader_resource);
		}

		void bc_shader_parameter_manager::set_constant_buffer_parameter(bc_shader_type p_shader_type, bcINT p_register, bc_buffer* p_resource)
		{
			bc_ishader_parameter* l_parameter = _set_param_value<bc_constant_buffer_parameter>(p_shader_type, p_register, p_resource);
			bcAssert(l_parameter->get_parameter_type() == bc_shader_parameter_type::cbuffer);
		}

		void bc_shader_parameter_manager::set_sampler_parameter(bc_ishader_parameter* p_parameter, bc_sampler_state* p_sampler)
		{
			if (p_parameter->get_parameter_type() == bc_shader_parameter_type::sampler)
			{
				p_parameter->set_parameter_data(p_sampler);
				return;
			}
			
			bcAssert(false);
		}

		void bc_shader_parameter_manager::set_shader_resource_parameter(bc_ishader_parameter* p_parameter, bcINT p_register, bc_shader_view* p_resource)
		{
			if (p_parameter->get_parameter_type() == bc_shader_parameter_type::shader_resource)
			{
				p_parameter->set_parameter_data(p_resource);
				return;
			}

			bcAssert(false);
		}

		void bc_shader_parameter_manager::set_constant_buffer_parameter(bc_ishader_parameter* p_parameter, bcINT p_register, bc_buffer* p_resource)
		{
			if (p_parameter->get_parameter_type() == bc_shader_parameter_type::cbuffer)
			{
				p_parameter->set_parameter_data(p_resource);
				return;
			}

			bcAssert(false);
		}
	}
}