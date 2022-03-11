 // [10/15/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/bcType.h"
#include "Platform/Script/bcScriptRef.h"
#include "Platform/Script/bcScriptReference.h"
#include "Platform/Script/bcScriptContext.h"

namespace black_cat
{
	namespace platform
	{
		template<platform::bc_platform TPlatform, typename T>
		struct bc_platform_script_prototype_pack
		{
		};

		template<platform::bc_platform TPlatform, typename T>
		class bc_platform_script_prototype : public bc_platform_script_reference<TPlatform>
		{
		public:
			using platform_pack = bc_platform_script_prototype_pack<TPlatform, T>;
			using type = T;
			friend bc_script_context;

		public:
			bc_platform_script_prototype();

			bc_platform_script_prototype(const bc_platform_script_prototype&) noexcept;

			~bc_platform_script_prototype();

			bc_platform_script_prototype& operator=(const bc_platform_script_prototype&) noexcept;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			bc_platform_script_prototype(bc_script_context& p_context);

			platform_pack m_pack;
		};

		template<typename T>
		using bc_script_prototype = bc_platform_script_prototype<platform::g_current_platform, T>;
		template<typename T>
		using bc_script_prototype_ref = bc_script_ref<bc_script_prototype<T>>;
	}
}