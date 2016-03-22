// [01/16/2016 MRB]

#pragma once

#include "Graphic/Resource/View/bcShaderView.h"
#include "Graphic/Shader/Parameter/bcShaderParameter.h"

namespace black_cat
{
	namespace graphic
	{
		class bc_shader_resource_parameter : public bc_ishader_parameter
		{
		public:
			bc_shader_resource_parameter();

			bc_shader_resource_parameter(bcINT p_register, bc_shader_view* p_shader_resource);

			bc_shader_resource_parameter(bc_shader_resource_parameter&) = default;

			~bc_shader_resource_parameter() = default;

			bc_shader_resource_parameter& operator=(bc_shader_resource_parameter&) = default;

			bc_shader_view* get_shader_resource() const;

			void set_shader_resource(bc_shader_view* p_shader_resource);

			bc_shader_parameter_type get_parameter_type() const override;

			void set_parameter_data(void* p_data) override;

		protected:

		private:
			bc_shader_view* m_shader_resource;
		};

		inline bc_shader_resource_parameter::bc_shader_resource_parameter() : m_shader_resource(nullptr)
		{
		}

		inline bc_shader_resource_parameter::bc_shader_resource_parameter(bcINT p_register, bc_shader_view* p_shader_resource)
			: bc_ishader_parameter(p_register),
			m_shader_resource(p_shader_resource)
		{
		}

		inline bc_shader_view* bc_shader_resource_parameter::get_shader_resource() const
		{
			return m_shader_resource;
		}

		inline void bc_shader_resource_parameter::set_shader_resource(bc_shader_view* p_shader_resource)
		{
			m_shader_resource = p_shader_resource;
		}

		inline bc_shader_parameter_type bc_shader_resource_parameter::get_parameter_type() const
		{
			return bc_shader_parameter_type::shader_resource;
		}

		inline void bc_shader_resource_parameter::set_parameter_data(void* p_data)
		{
			set_shader_resource(reinterpret_cast<bc_shader_view*>(p_data));
		}
	}
}