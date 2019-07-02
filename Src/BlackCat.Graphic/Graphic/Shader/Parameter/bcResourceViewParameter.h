// [01/16/2016 MRB]

#pragma once

#include "Graphic/Resource/View/bcResourceView.h"
#include "Graphic/Shader/Parameter/bcShaderParameter.h"

namespace black_cat
{
	namespace graphic
	{
		class bc_resource_view_parameter : public bc_ishader_parameter
		{
		public:
			bc_resource_view_parameter();

			bc_resource_view_parameter(bcINT p_register, bc_shader_type p_shader_types, bc_resource_view p_shader_resource);

			bc_resource_view_parameter(bcINT p_register, bc_shader_type p_shader_types, const bc_shader_parameter_link* p_parameter_link);

			bc_resource_view_parameter(const bc_resource_view_parameter&) = default;

			~bc_resource_view_parameter() = default;

			bc_resource_view_parameter& operator=(const bc_resource_view_parameter&) = default;

			bc_resource_view get_resource_view() const;

			void set_resource_view(bc_resource_view p_shader_resource);

			bc_shader_parameter_type get_parameter_type() const override;

			void set_parameter_data(void* p_data) override;

			bool is_valid() const override;
		protected:

		private:
			bc_resource_view m_shader_resource;
		};

		inline bc_resource_view_parameter::bc_resource_view_parameter()
			: bc_ishader_parameter(),
			m_shader_resource() 
		{
		}

		inline bc_resource_view_parameter::bc_resource_view_parameter(bcINT p_register, bc_shader_type p_shader_types, bc_resource_view p_shader_resource)
			: bc_ishader_parameter(p_register, p_shader_types),
			m_shader_resource(p_shader_resource)
		{
		}

		inline bc_resource_view_parameter::bc_resource_view_parameter(bcINT p_register, bc_shader_type p_shader_types, const bc_shader_parameter_link* p_parameter_link)
			: bc_ishader_parameter(p_register, p_shader_types, p_parameter_link)
		{
		}

		inline bc_resource_view bc_resource_view_parameter::get_resource_view() const
		{
			return m_shader_resource.is_valid()
				       ? m_shader_resource
				       : m_link
					         ? m_link->get_as_resource_view()
					         : bc_resource_view();
		}

		inline void bc_resource_view_parameter::set_resource_view(bc_resource_view p_shader_resource)
		{
			m_shader_resource = p_shader_resource;
		}

		inline bc_shader_parameter_type bc_resource_view_parameter::get_parameter_type() const
		{
			const auto l_shader_resource = get_resource_view();
			if (l_shader_resource.is_valid() && l_shader_resource.get_view_type() == bc_resource_view_type::unordered)
			{
				return bc_shader_parameter_type::unordered_view;
			}

			return bc_shader_parameter_type::shader_view;
		}

		inline void bc_resource_view_parameter::set_parameter_data(void* p_data)
		{
			set_resource_view(*reinterpret_cast<bc_resource_view*>(p_data));
		}

		inline bool bc_resource_view_parameter::is_valid() const
		{
			return m_register_index != -1 && get_resource_view().is_valid();
		}
	}
}