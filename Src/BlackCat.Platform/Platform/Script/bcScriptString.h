 // [10/10/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "Core/Container/bcString.h"
#include "Platform/Script/bcScriptRef.h"
#include "Platform/Script/bcScriptReference.h"
#include "Platform/Script/bcScriptContext.h"

namespace black_cat
{
	namespace platform
	{
		template<platform::bc_platform TPlatform>
		struct bc_platform_script_string_pack
		{
		};

		template<platform::bc_platform TPlatform>
		class bc_platform_script_string : public bc_platform_script_reference<TPlatform>
		{
		public:
			using platform_pack = bc_platform_script_string_pack<TPlatform>;
			friend bc_script_context;

		public:
			bc_platform_script_string();

			bc_platform_script_string(const bc_platform_script_string&) noexcept;

			~bc_platform_script_string();

			bc_platform_script_string& operator=(const bc_platform_script_string&) noexcept;

			bcSIZE length() const noexcept;

			core::bc_wstring data() const;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			bc_platform_script_string(bc_script_context& p_context, const core::bc_wstring& p_value);

			platform_pack m_pack;
		};

		using bc_script_string = bc_platform_script_string<platform::g_current_platform>;
		using bc_script_string_ref = bc_script_ref<bc_script_string>;
	}
}