 // [10/12/2016 MRB]

#pragma once

#include "Platform/Script/bcScriptContext.h"
#include "Platform/Script/bcScriptObject.h"

namespace black_cat
{
	namespace platform
	{
		template< core_platform::bc_platform TPlatform, typename T >
		class bc_platform_script_array_element;

		template< core_platform::bc_platform TPlatform, typename T >
		struct bc_platform_script_array_pack
		{
		};

		template< core_platform::bc_platform TPlatform, typename T >
		class bc_platform_script_array : public bc_platform_script_object< TPlatform >
		{
		public:
			using platform_pack = bc_platform_script_array_pack< TPlatform, T >;
			using element = bc_platform_script_array_element< TPlatform, T >;
			friend bc_script_context;

		public:
			bc_platform_script_array();

			bc_platform_script_array(const bc_platform_script_array&) noexcept;

			~bc_platform_script_array();

			bc_platform_script_array& operator=(const bc_platform_script_array&) noexcept;

			bcSIZE size() const noexcept;

			element operator[](bcINT p_index);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			bc_platform_script_array(bc_script_context& p_context, bcSIZE p_length);

			bc_platform_script_array(bc_script_context& p_context, std::initializer_list< T > p_array);

			platform_pack m_pack;
		};

		template< typename T>
		using bc_script_array = bc_platform_script_array< core_platform::g_current_platform, T >;

		template< core_platform::bc_platform TPlatform, typename T >
		struct bc_platform_script_array_element_pack
		{
		};

		template< core_platform::bc_platform TPlatform, typename T >
		class bc_platform_script_array_element
		{
		public:
			using platform_pack = bc_platform_script_array_element_pack< TPlatform, T >;

		public:
			bc_platform_script_array_element(bc_script_array<T>& p_array, bcINT p_index);

			bc_platform_script_array_element& operator=(bc_script_variable p_value);

			bc_platform_script_array_element& operator=(T p_value);

			operator bc_script_variable();

			operator T();

		protected:

		private:
			platform_pack m_pack;
		};
	}
}