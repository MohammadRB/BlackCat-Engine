// [01/16/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Graphic/Resource/Buffer/bcBuffer.h"
#include "Graphic/Shader/Parameter/bcShaderParameter.h"

namespace black_cat
{
	namespace graphic
	{
		class bc_constant_buffer_parameter : public bc_ishader_parameter
		{
		public:
			bc_constant_buffer_parameter();

			bc_constant_buffer_parameter(bcINT p_register, bc_shader_type p_shader_types, const bc_buffer_ptr& p_buffer);

			bc_constant_buffer_parameter(const bc_constant_buffer_parameter&) = default;

			~bc_constant_buffer_parameter();

			bc_constant_buffer_parameter& operator=(const bc_constant_buffer_parameter&) = default;

			const bc_buffer_ptr& get_buffer() const;

			void set_buffer(const bc_buffer_ptr& p_buffer);

			bc_shader_parameter_type get_parameter_type() const override;

			void set_parameter_data(void* p_data) override;

			bool is_valid() const override;
		protected:

		private:
			bc_buffer_ptr m_buffer;
		};

		inline bc_constant_buffer_parameter::bc_constant_buffer_parameter()
			: bc_ishader_parameter(),
			m_buffer(nullptr) 
		{
		}

		inline bc_constant_buffer_parameter::bc_constant_buffer_parameter(bcINT p_register, bc_shader_type p_shader_types, const bc_buffer_ptr& p_buffer)
			: bc_ishader_parameter(p_register, p_shader_types),
			m_buffer(p_buffer)
		{
		}

		inline bc_constant_buffer_parameter::~bc_constant_buffer_parameter()
		{
		}

		inline const bc_buffer_ptr& bc_constant_buffer_parameter::get_buffer() const
		{
			return m_buffer;
		}

		inline void bc_constant_buffer_parameter::set_buffer(const bc_buffer_ptr& p_buffer)
		{
			m_buffer = p_buffer;
		}

		inline bc_shader_parameter_type bc_constant_buffer_parameter::get_parameter_type() const
		{
			return bc_shader_parameter_type::cbuffer;
		}

		inline void bc_constant_buffer_parameter::set_parameter_data(void* p_data)
		{
			set_buffer(reinterpret_cast<const bc_buffer_ptr&>(p_data));
		}

		inline bool bc_constant_buffer_parameter::is_valid() const
		{
			return m_register_index != -1 && m_buffer != nullptr;
		}
	}
}