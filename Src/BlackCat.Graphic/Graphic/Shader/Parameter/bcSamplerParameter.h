// [01/16/2016 MRB]

#pragma once

#include "Graphic/Resource/State/bcSamplerState.h"
#include "Graphic/Shader/Parameter/bcShaderParameter.h"

namespace black_cat
{
	namespace graphic
	{
		class bc_sampler_parameter : public bc_ishader_parameter
		{
		public:
			bc_sampler_parameter();

			bc_sampler_parameter(bcINT p_register, bc_shader_type p_shader_types, bc_sampler_state p_sampler);

			bc_sampler_parameter(bcINT p_register, bc_shader_type p_shader_types, const bc_shader_parameter_link& p_parameter_link);
			
			bc_sampler_parameter(bc_sampler_parameter&) = default;

			~bc_sampler_parameter() override = default;

			bc_sampler_parameter& operator=(const bc_sampler_parameter&) = default;

			bc_sampler_state get_sampler() const;

			void set_sampler(bc_sampler_state p_sampler);

			bc_shader_parameter_type get_parameter_type() const override;

			void set_parameter_data(void* p_data) override;

			bool is_valid() const override;

		private:
			bc_sampler_state m_sampler;
		};

		inline bc_sampler_parameter::bc_sampler_parameter()
			: bc_ishader_parameter(),
			m_sampler() 
		{
		}

		inline bc_sampler_parameter::bc_sampler_parameter(bcINT p_register, bc_shader_type p_shader_types, bc_sampler_state p_sampler)
			: bc_ishader_parameter(p_register, p_shader_types),
			m_sampler(p_sampler)
		{
		}

		inline bc_sampler_parameter::bc_sampler_parameter(bcINT p_register, bc_shader_type p_shader_types, const bc_shader_parameter_link& p_parameter_link)
			: bc_ishader_parameter(p_register, p_shader_types, p_parameter_link)
		{
		}

		inline bc_sampler_state bc_sampler_parameter::get_sampler() const
		{
			return m_sampler.is_valid()
				       ? m_sampler
				       : m_link
					         ? m_link->get_as_sampler()
					         : bc_sampler_state();
		}

		inline void bc_sampler_parameter::set_sampler(bc_sampler_state p_sampler)
		{
			m_sampler = p_sampler;
		}

		inline bc_shader_parameter_type bc_sampler_parameter::get_parameter_type() const
		{
			return bc_shader_parameter_type::sampler;
		}

		inline void bc_sampler_parameter::set_parameter_data(void* p_data)
		{
			set_sampler(*reinterpret_cast<bc_sampler_state*>(p_data));
		}

		inline bool bc_sampler_parameter::is_valid() const
		{
			return m_register_index != -1 && get_sampler().is_valid();
		}
	}
}