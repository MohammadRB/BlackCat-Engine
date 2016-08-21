// [01/16/2016 MRB]

#pragma once

#include "Graphic/Resource/View/bcShaderView.h"
#include "Graphic/Shader/Parameter/bcShaderParameter.h"

namespace black_cat
{
	namespace graphic
	{
		class bc_shader_view_parameter : public bc_ishader_parameter
		{
		public:
			bc_shader_view_parameter();

			bc_shader_view_parameter(bcINT p_register, bc_shader_type p_shader_types, const bc_shader_view_ptr& p_shader_resource);

			bc_shader_view_parameter(bc_shader_view_parameter&) = default;

			~bc_shader_view_parameter() = default;

			bc_shader_view_parameter& operator=(bc_shader_view_parameter&) = default;

			const bc_shader_view_ptr& get_shader_resource() const;

			void set_shader_resource(const bc_shader_view_ptr& p_shader_resource);

			bc_shader_parameter_type get_parameter_type() const override;

			void set_parameter_data(void* p_data) override;

			bool is_valid() const override;
		protected:

		private:
			bc_shader_view_ptr m_shader_resource;
		};

		inline bc_shader_view_parameter::bc_shader_view_parameter()
			: bc_ishader_parameter(),
			m_shader_resource(nullptr) 
		{
		}

		inline bc_shader_view_parameter::bc_shader_view_parameter(bcINT p_register, bc_shader_type p_shader_types, const bc_shader_view_ptr& p_shader_resource)
			: bc_ishader_parameter(p_register, p_shader_types),
			m_shader_resource(p_shader_resource)
		{
		}

		inline const bc_shader_view_ptr& bc_shader_view_parameter::get_shader_resource() const
		{
			return m_shader_resource;
		}

		inline void bc_shader_view_parameter::set_shader_resource(const bc_shader_view_ptr& p_shader_resource)
		{
			m_shader_resource = p_shader_resource;
		}

		inline bc_shader_parameter_type bc_shader_view_parameter::get_parameter_type() const
		{
			return bc_shader_parameter_type::shader_resource;
		}

		inline void bc_shader_view_parameter::set_parameter_data(void* p_data)
		{
			set_shader_resource(reinterpret_cast<const bc_shader_view_ptr&>(p_data));
		}

		inline bool bc_shader_view_parameter::is_valid() const
		{
			return m_register_index != -1 && m_shader_resource != nullptr;
		}
	}
}