// [01/17/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcMap.h"
#include "Graphic/bcExport.h"
#include "Graphic/Resource/bcResource.h"
#include "Graphic/Resource/State/bcSamplerState.h"
#include "Graphic/Resource/Buffer/bcBuffer.h"
#include "Graphic/Resource/View/bcResourceView.h"
#include "Graphic/Shader/bcShader.h"
#include "Graphic/Shader/Parameter/bcShaderParameter.h"
#include "Graphic/Shader/Parameter/bcVectorParameter.h"
#include "Graphic/Shader/Parameter/bcMatrix4fParameter.h"
#include "Graphic/Shader/Parameter/bcConstantBufferParameter.h"
#include "Graphic/Shader/Parameter/bcSamplerParameter.h"
#include "Graphic/Shader/Parameter/bcResourceViewParameter.h"

namespace black_cat
{
	namespace graphic
	{
		class BC_GRAPHIC_DLL bc_shader_parameter_manager
		{
		public:
			// Use this struct as an entry for map collection of parameters
			struct _parameter_entry
			{
			public:
				_parameter_entry(const bc_shader_type p_shader_type, const bcINT p_register);

				bool operator<(const _parameter_entry& p_other) const;

				const bc_shader_type m_shader_type;
				const bcINT m_register;
			};

		public:
			bc_shader_parameter_manager() = default;

			bc_shader_parameter_manager(bc_shader_parameter_manager&&) = default;

			~bc_shader_parameter_manager() = default;

			bc_shader_parameter_manager& operator=(bc_shader_parameter_manager&&) = default;


			bc_sampler_parameter* get_sampler_parameter_ref(bc_shader_type p_shader_type, bcINT p_register) const;

			bc_resource_view_parameter* get_shader_resource_parameter_ref(bc_shader_type p_shader_type, bcINT p_register) const;

			bc_constant_buffer_parameter* get_constant_buffer_parameter_ref(bc_shader_type p_shader_type, bcINT p_register) const;


			bc_sampler_state* get_sampler_parameter(bc_shader_type p_shader_type, bcINT p_register) const;

			bc_resource_view* get_shader_resource_parameter(bc_shader_type p_shader_type, bcINT p_register) const;

			/*bc_iresource* get_unordered_access_parameter(bc_shader_type p_shader_type, bcINT p_register) const;*/

			bc_buffer* get_constant_buffer_parameter(bc_shader_type p_shader_type, bcINT p_register) const;


			bc_sampler_state* get_sampler_parameter(bc_ishader_parameter* p_parameter) const;

			bc_resource_view* get_shader_resource_parameter(bc_ishader_parameter* p_parameter) const;

			/*bc_iresource* get_unordered_access_parameter(bc_ishader_parameter* p_parameter) const;*/

			bc_buffer* get_constant_buffer_parameter(bc_ishader_parameter* p_parameter) const;

			
			void set_sampler_parameter(bc_shader_type p_shader_type, bcINT p_register, bc_sampler_state* p_sampler);

			void set_shader_resource_parameter(bc_shader_type p_shader_type, bcINT p_register, bc_resource_view* p_resource);

			/*void set_unordered_access_parameter(bc_shader_type p_shader_type, bcINT p_register, bc_iresource* p_resource, bcUINT p_initial = -1);*/

			void set_constant_buffer_parameter(bc_shader_type p_shader_type, bcINT p_register, bc_buffer* p_resource);


			void set_sampler_parameter(bc_ishader_parameter* p_parameter, bc_sampler_state* p_sampler);

			void set_shader_resource_parameter(bc_ishader_parameter* p_parameter, bcINT p_register, bc_resource_view* p_resource);

			/*void set_unordered_access_parameter(bc_ishader_parameter* p_parameter, bcINT p_register, bc_iresource* p_resource, bcUINT p_initial = -1);*/

			void set_constant_buffer_parameter(bc_ishader_parameter* p_parameter, bcINT p_register, bc_buffer* p_resource);

		protected:

		private:
			template< typename TParameter, typename TParameterValue >
			bc_ishader_parameter* _set_param_value(bc_shader_type p_shader_type, bcINT p_register, TParameterValue* p_paraemter);

			template< bc_shader_parameter_type TParameterType, typename TParameter >
			TParameter* _get_param_ref(bc_shader_type p_shader_type, bcINT p_register) const;
			
			core::bc_map< _parameter_entry, core::bc_unique_ptr< bc_ishader_parameter > > m_parameters;
		};

		template< typename TParameter, typename TParameterValue >
		bc_ishader_parameter* bc_shader_parameter_manager::_set_param_value(bc_shader_type p_shader_type, bcINT p_register, TParameterValue* p_parameter)
		{
			_parameter_entry l_entry(p_shader_type, p_register);
			auto& l_value = m_parameters[l_entry];

			if (l_value.get() == nullptr)
			{
				l_value.reset(bcNew(TParameter(), core::bc_alloc_type::unknown));
			}

			l_value->set_register_index(p_register);
			l_value->set_parameter_data(p_parameter);

			return l_value.get();
		}

		template< bc_shader_parameter_type TParameterType, typename TParameter >
		TParameter* bc_shader_parameter_manager::_get_param_ref(bc_shader_type p_shader_type, bcINT p_register) const
		{
			_parameter_entry l_entry(p_shader_type, p_register);
			auto l_ite = m_parameters.find(l_entry);

			if (l_ite != m_parameters.end())
			{
				bc_ishader_parameter* l_parameter = (*l_ite).second.get();

				if (l_parameter->get_parameter_type() == TParameterType)
					return static_cast<TParameter*>(l_parameter);

				bcAssert(false);
			}

			return nullptr;
		}
	}
}