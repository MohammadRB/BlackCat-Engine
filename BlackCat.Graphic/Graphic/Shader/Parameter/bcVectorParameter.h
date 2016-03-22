// [01/16/2016 MRB]

#pragma once

#include "Graphic/Math/bcVector4f.h"
#include "Graphic/Shader/Parameter/bcShaderParameter.h"

namespace black_cat
{
	namespace graphic
	{
		class bc_vector_parameter : public bc_ishader_parameter
		{
		public:
			bc_vector_parameter() = default;

			bc_vector_parameter(bcINT p_register, bc_vector4f& p_vector);

			bc_vector_parameter(bc_vector_parameter&) = default;

			~bc_vector_parameter() = default;

			bc_vector_parameter& operator=(bc_vector_parameter&) = default;

			bc_vector4f get_vector() const;

			void set_vector(bc_vector4f& p_vector);

			bc_shader_parameter_type get_parameter_type() const override;

			void set_parameter_data(void* p_data) override;

		protected:

		private:
			bc_vector4f m_vector;
		};

		inline bc_vector_parameter::bc_vector_parameter(bcINT p_register, bc_vector4f& p_vector)
			: bc_ishader_parameter(p_register),
			m_vector(p_vector)
		{
		}

		inline bc_vector4f bc_vector_parameter::get_vector() const
		{
			return m_vector;
		}

		inline void bc_vector_parameter::set_vector(bc_vector4f& p_matrix)
		{
			m_vector = p_matrix;
		}

		inline bc_shader_parameter_type bc_vector_parameter::get_parameter_type() const
		{
			return bc_shader_parameter_type::vector;
		}

		inline void bc_vector_parameter::set_parameter_data(void* p_data)
		{
			if (!p_data)
				set_vector(bc_vector4f(0, 0, 0, 0));
			else
				set_vector(*reinterpret_cast<bc_vector4f*>(p_data));
		}
	}
}