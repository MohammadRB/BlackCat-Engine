// [10/12/2016 MRB]

#pragma once

#include "Platform/Script/bcScriptRef.h"
#include "Platform/Script/bcScriptContext.h"
#include "Platform/Script/bcScriptVariable.h"
#include "Platform/Script/bcScriptObject.h"

namespace black_cat
{
	namespace platform
	{
		// == bc_script_array_base =================================================================================

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_array_base_element;

		template< core_platform::bc_platform TPlatform >
		struct bc_platform_script_array_base_pack
		{
		};

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_array_base : public bc_platform_script_object< TPlatform >
		{
		public:
			using platform_pack = bc_platform_script_array_base_pack< TPlatform >;
			using element = bc_platform_script_array_base_element< TPlatform >;
			friend bc_script_context;

		public:
			bc_platform_script_array_base();

			bc_platform_script_array_base(const bc_platform_script_array_base&) noexcept;

			~bc_platform_script_array_base();

			bc_platform_script_array_base& operator=(const bc_platform_script_array_base&) noexcept;

			bcSIZE size() const noexcept;

			element operator[](bcINT p_index);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			bc_platform_script_array_base(bc_script_context& p_context, bcSIZE p_length);

			bc_platform_script_array_base(bc_script_context& p_context, std::initializer_list< bc_script_variable > p_array);

			platform_pack m_pack;
		};

		template< core_platform::bc_platform TPlatform >
		struct bc_platform_script_array_base_element_pack
		{
		};

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_array_base_element
		{
		public:
			using platform_pack = bc_platform_script_array_base_element_pack< TPlatform >;

		public:
			bc_platform_script_array_base_element(bc_platform_script_array_base< TPlatform >& p_array, bcINT p_index);

			bc_platform_script_array_base_element& operator=(bc_script_variable p_value);

			operator bc_script_variable();

		protected:

		private:
			platform_pack m_pack;
		};

		// == bc_script_array =====================================================================================

		template< core_platform::bc_platform TPlatform, typename T >
		class bc_platform_script_array_element;

		template< core_platform::bc_platform TPlatform, typename T >
		class bc_platform_script_array : public bc_platform_script_array_base< TPlatform >
		{
		public:
			using element = bc_platform_script_array_element< TPlatform, T >;
			friend bc_script_context;

		public:
			bc_platform_script_array();

			bc_platform_script_array(const bc_platform_script_array&) noexcept;

			~bc_platform_script_array();

			bc_platform_script_array& operator=(const bc_platform_script_array&) noexcept;

			element operator[](bcINT p_index);

		protected:

		private:
			bc_platform_script_array(bc_script_context& p_context, bcSIZE p_length);

			bc_platform_script_array(bc_script_context& p_context, std::initializer_list< T > p_array);
		};

		template< core_platform::bc_platform TPlatform, typename T >
		class bc_platform_script_array_element : public bc_platform_script_array_base_element< TPlatform >
		{
		public:
			bc_platform_script_array_element(bc_script_array< T >& p_array, bcINT p_index);

			bc_platform_script_array_element& operator=(T p_value);

			operator T();

		protected:

		private:
		};

		using bc_script_array_base = bc_platform_script_array_base< core_platform::g_current_platform >;
		using bc_script_array_base_ref = bc_script_ref< bc_script_array_base >;

		template< typename T >
		using bc_script_array = bc_platform_script_array< core_platform::g_current_platform, T >;
		template< typename T >
		using bc_script_array_ref = bc_script_ref< bc_script_array< T > >;

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >::bc_platform_script_array() = default;

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >::bc_platform_script_array(bc_script_context& p_context, bcSIZE p_length)
			: bc_platform_script_array_base(p_context, p_length)
		{
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >::bc_platform_script_array(bc_script_context& p_context, std::initializer_list< T > p_array)
			: bc_platform_script_array_base(p_context, p_array.size())
		{
			bcSIZE l_index = 0;

			for (auto& l_item : p_array)
			{
				(*this)[l_index++] = l_item;
			}
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >::bc_platform_script_array(const bc_platform_script_array&) noexcept = default;

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >::~bc_platform_script_array() = default;

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array< TPlatform, T >& bc_platform_script_array< TPlatform, T >::operator=(const bc_platform_script_array&) noexcept = default;

		template< core_platform::bc_platform TPlatform, typename T >
		typename bc_platform_script_array< TPlatform, T >::element bc_platform_script_array< TPlatform, T >::operator[](bcINT p_index)
		{
			return element(*this, p_index);
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array_element< TPlatform, T >::bc_platform_script_array_element(bc_script_array< T >& p_array, bcINT p_index)
			: bc_platform_script_array_base_element(p_array, p_index)
		{
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array_element< TPlatform, T >& bc_platform_script_array_element< TPlatform, T >::operator=(T p_value)
		{
			(*this) = bc_script_variable::_pack_arg(p_value);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename T >
		bc_platform_script_array_element< TPlatform, T >::operator T()
		{
			T l_result;
			bc_script_variable::_unpack_arg(static_cast< bc_script_variable >(*this), &l_result);

			return l_result;
		}
	}
}
