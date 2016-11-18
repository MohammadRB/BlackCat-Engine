 // [10/11/2016 MRB]

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
		template< core_platform::bc_platform TPlatform >
		struct bc_platform_script_error_pack
		{
		};

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_error : public bc_platform_script_reference< TPlatform >
		{
		public:
			using platform_pack = bc_platform_script_error_pack< TPlatform >;
			friend bc_script_context;

		public:
			bc_platform_script_error();

			bc_platform_script_error(const bc_platform_script_error&) noexcept;

			~bc_platform_script_error();

			bc_platform_script_error& operator=(const bc_platform_script_error&) noexcept;

			core::bc_wstring error_message() const;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			bc_platform_script_error(bc_script_context& p_context, const core::bc_wstring& p_message);

			platform_pack m_pack;
		};

		using bc_script_error = bc_platform_script_error< core_platform::g_current_platform >;
		using bc_script_error_ref = bc_script_ref< bc_script_error >;
	}
}