// [16/01/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Graphic/Resource/Buffer/bcBuffer.h"
#include "Graphic/Shader/Parameter/bcShaderParameter.h"

namespace black_cat::graphic
{
	class bc_constant_buffer_parameter : public bc_ishader_parameter
	{
	public:
		bc_constant_buffer_parameter();

		bc_constant_buffer_parameter(bcINT16 p_register, bc_shader_type p_shader_types, bc_buffer p_buffer);

		bc_constant_buffer_parameter(bcINT16 p_register, bc_shader_type p_shader_types, bc_shader_parameter_link p_parameter_link);

		bc_constant_buffer_parameter(const bc_constant_buffer_parameter&) = default;

		~bc_constant_buffer_parameter() override;

		bc_constant_buffer_parameter& operator=(const bc_constant_buffer_parameter&) = default;

		bc_buffer get_buffer() const;

		void set_buffer(bc_buffer p_buffer);

		bc_shader_parameter_type get_parameter_type() const override;

		bool is_valid() const override;
			
	private:
		bc_buffer m_buffer;
	};

	inline bc_constant_buffer_parameter::bc_constant_buffer_parameter()
		: bc_ishader_parameter(),
		  m_buffer() 
	{
	}

	inline bc_constant_buffer_parameter::bc_constant_buffer_parameter(bcINT16 p_register, bc_shader_type p_shader_types, bc_buffer p_buffer)
		: bc_ishader_parameter(p_register, p_shader_types),
		  m_buffer(p_buffer)
	{
	}

	inline bc_constant_buffer_parameter::bc_constant_buffer_parameter(bcINT16 p_register, bc_shader_type p_shader_types, bc_shader_parameter_link p_parameter_link)
		: bc_ishader_parameter(p_register, p_shader_types, std::move(p_parameter_link))
	{
	}

	inline bc_constant_buffer_parameter::~bc_constant_buffer_parameter() = default;

	inline bc_buffer bc_constant_buffer_parameter::get_buffer() const
	{
		return m_buffer.is_valid() ? m_buffer : m_link.get_as_cbuffer();
	}

	inline void bc_constant_buffer_parameter::set_buffer(bc_buffer p_buffer)
	{
		m_buffer = p_buffer;
	}

	inline bc_shader_parameter_type bc_constant_buffer_parameter::get_parameter_type() const
	{
		return bc_shader_parameter_type::cbuffer;
	}

	inline bool bc_constant_buffer_parameter::is_valid() const
	{
		return m_register_index != -1 && get_buffer().is_valid();
	}
}
