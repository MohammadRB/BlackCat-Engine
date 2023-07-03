// [04/10/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "Platform/Script/bcScriptRef.h"
#include "Platform/Script/bcScriptReference.h"
#include "Platform/Script/bcScriptRuntime.h"

namespace black_cat
{
	namespace platform
	{
		template<bc_platform TPlatform >
		struct bc_platform_script_bytecode_pack
		{
		};

		template<bc_platform TPlatform >
		class bc_platform_script_bytecode : public bc_platform_script_reference< TPlatform >
		{
		public:
			using platform_pack = bc_platform_script_bytecode_pack< TPlatform >;
			friend bc_script_runtime;

		public:
			bc_platform_script_bytecode(const bc_platform_script_bytecode&) noexcept;

			~bc_platform_script_bytecode();

			bc_platform_script_bytecode& operator=(const bc_platform_script_bytecode&) noexcept;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			bc_platform_script_bytecode();

			platform_pack m_pack;
		};

		using bc_script_bytecode = bc_platform_script_bytecode<g_current_platform>;
		using bc_script_bytecode_ref = bc_script_ref< bc_script_bytecode >;
	}
}