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
		template<platform::bc_platform TPlatform>
		class bc_platform_script_array_element;

		template<platform::bc_platform TPlatform>
		struct bc_platform_script_array_pack
		{
		};

		template<platform::bc_platform TPlatform>
		class bc_platform_script_array : public bc_platform_script_object<TPlatform>
		{
		public:
			using platform_pack = bc_platform_script_array_pack<TPlatform>;
			using element = bc_platform_script_array_element<TPlatform>;
			friend bc_script_context;

		public:
			bc_platform_script_array();

			bc_platform_script_array(const bc_platform_script_array&) noexcept;

			~bc_platform_script_array() override;

			bc_platform_script_array& operator=(const bc_platform_script_array&) noexcept;

			bcSIZE size() const noexcept;

			element operator[](bcINT p_index);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			bc_platform_script_array(bc_script_context& p_context, bcSIZE p_length);

			bc_platform_script_array(bc_script_context& p_context, std::initializer_list<bc_script_variable> p_array);

			platform_pack m_pack;
		};

		template<platform::bc_platform TPlatform>
		struct bc_platform_script_array_element_pack
		{
		};

		template<platform::bc_platform TPlatform>
		class bc_platform_script_array_element
		{
		public:
			using platform_pack = bc_platform_script_array_element_pack<TPlatform>;

		public:
			bc_platform_script_array_element(bc_platform_script_array<TPlatform>& p_array, bcINT p_index);

			bc_platform_script_array_element& operator=(bc_script_variable p_value);

			operator bc_script_variable();

		private:
			platform_pack m_pack;
		};

		template<platform::bc_platform TPlatform, typename T>
		class bc_platform_script_array_wrapper_element;

		template<platform::bc_platform TPlatform, typename T>
		class bc_platform_script_array_wrapper : public bc_platform_script_array<TPlatform>
		{
		public:
			using element = bc_platform_script_array_wrapper_element<TPlatform, T>;
			friend bc_script_context;

		public:
			bc_platform_script_array_wrapper();
			
			bc_platform_script_array_wrapper(const bc_platform_script_array<TPlatform>& p_array);

			bc_platform_script_array_wrapper(const bc_platform_script_array<TPlatform>& p_array, std::initializer_list<T> p_init);

			bc_platform_script_array_wrapper(const bc_platform_script_array_wrapper&) noexcept;

			~bc_platform_script_array_wrapper();

			bc_platform_script_array_wrapper& operator=(const bc_platform_script_array_wrapper&) noexcept;

			element operator[](bcINT p_index);
		};

		template<platform::bc_platform TPlatform, typename T>
		class bc_platform_script_array_wrapper_element : public bc_platform_script_array_element<TPlatform>
		{
		public:
			bc_platform_script_array_wrapper_element(bc_platform_script_array_element<TPlatform>& p_element);

			bc_platform_script_array_wrapper_element& operator=(T p_value);

			operator T();
		};

		using bc_script_array = bc_platform_script_array<platform::g_current_platform>;
		using bc_script_array_ref = bc_script_ref<bc_script_array>;

		template<typename T>
		using bc_script_array_wrapper = bc_platform_script_array_wrapper<platform::g_current_platform, T>;
		template<typename T>
		using bc_script_array_wrapper_ref = bc_script_ref<bc_script_array_wrapper<T>>;
	}
}