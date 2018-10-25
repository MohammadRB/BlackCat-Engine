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

			bc_sampler_parameter(bcINT p_register, bc_shader_type p_shader_types, const bc_sampler_state_ptr& p_sampler);

			bc_sampler_parameter(bc_sampler_parameter&) = default;

			~bc_sampler_parameter() = default;

			bc_sampler_parameter& operator=(bc_sampler_parameter&) = default;

			bc_sampler_state_ptr& get_sampler();

			const bc_sampler_state_ptr& get_sampler() const;

			void set_sampler(const bc_sampler_state_ptr& p_sampler);

			bc_shader_parameter_type get_parameter_type() const override;

			void set_parameter_data(void* p_data) override;

			bool is_valid() const override;
		protected:

		private:
			bc_sampler_state_ptr m_sampler;
		};

		inline bc_sampler_parameter::bc_sampler_parameter()
			: bc_ishader_parameter(),
			m_sampler() 
		{
		}

		inline bc_sampler_parameter::bc_sampler_parameter(bcINT p_register, bc_shader_type p_shader_types, bc_sampler_state p_sampler)
			: bc_sampler_parameter(p_register, p_shader_types, bc_sampler_state_ptr(p_sampler))
		{
		}

		inline bc_sampler_parameter::bc_sampler_parameter(bcINT p_register, bc_shader_type p_shader_types, const bc_sampler_state_ptr& p_sampler)
			: bc_ishader_parameter(p_register, p_shader_types),
			m_sampler(p_sampler)
		{
		}

		inline bc_sampler_state_ptr& bc_sampler_parameter::get_sampler()
		{
			return m_sampler;
		}

		inline const bc_sampler_state_ptr& bc_sampler_parameter::get_sampler() const
		{
			return m_sampler;
		}

		inline void bc_sampler_parameter::set_sampler(const bc_sampler_state_ptr& p_sampler)
		{
			m_sampler = p_sampler;
		}

		inline bc_shader_parameter_type bc_sampler_parameter::get_parameter_type() const
		{
			return bc_shader_parameter_type::sampler;
		}

		inline void bc_sampler_parameter::set_parameter_data(void* p_data)
		{
			set_sampler(reinterpret_cast<const bc_sampler_state_ptr&>(p_data));
		}

		inline bool bc_sampler_parameter::is_valid() const
		{
			return m_register_index != -1 && m_sampler != nullptr;
		}
	}
}