// [01/15/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"

namespace black_cat
{
	namespace graphic
	{
		enum class bc_shader_parameter_type
		{
			vector,
			matrix,
			matrix_array,
			shader_resource,
			unordered_access,
			cbuffer,
			sampler
		};

		class bc_ishader_parameter
		{
		public:
			bc_ishader_parameter();

			bc_ishader_parameter(bcINT p_register_index);

			bc_ishader_parameter(const bc_ishader_parameter& p_other) = default;

			virtual ~bc_ishader_parameter() = 0;

			bc_ishader_parameter& operator=(const bc_ishader_parameter& p_other) = default;

			bcINT get_register_index() const;

			void set_register_index(bcINT p_index);

			virtual bc_shader_parameter_type get_parameter_type() const = 0;

			virtual void set_parameter_data(void* p_data) = 0;

		protected:
			bcINT m_register_index;

		private:
		};

		inline bc_ishader_parameter::bc_ishader_parameter() : m_register_index(-1)
		{
		}

		inline bc_ishader_parameter::bc_ishader_parameter(bcINT p_register_index) : m_register_index(p_register_index)
		{
		}

		inline bc_ishader_parameter::~bc_ishader_parameter()
		{
		}

		inline bcINT bc_ishader_parameter::get_register_index() const
		{
			return m_register_index;
		}

		inline void bc_ishader_parameter::set_register_index(bcINT p_index)
		{
			m_register_index = p_index;
		}
	}
}