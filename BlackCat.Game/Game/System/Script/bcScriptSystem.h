 // [10/21/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcString.h"
#include "PlatformImp/Script/bcScriptRuntime.h"
#include "PlatformImp/Script/bcScriptContext.hpp"
#include "PlatformImp/Script/bcScriptByteCode.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptString.h"
#include "PlatformImp/Script/bcScriptArray.h"
#include "PlatformImp/Script/bcScriptObject.h"
#include "PlatformImp/Script/bcScriptFunction.h"
#include "PlatformImp/Script/bcScriptError.h"
#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptPrototype.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"
#include "PlatformImp/Script/bcScriptRef.h"
#include "Game/bcExport.h"
#include "Game/System/Script/bcScriptUIContext.h"

namespace black_cat
{
	namespace game
	{
		enum class bc_script_context : bcUBYTE
		{
			ui = 0
		};

		class BC_GAME_DLL bc_script_system
		{
		public:
			bc_script_system();

			bc_script_system(bc_script_system&&) noexcept;

			~bc_script_system();

			bc_script_system& operator=(bc_script_system&&) noexcept;

			platform::bc_script_bytecode load_script(bc_script_context p_context, const bcECHAR* p_script_file);

			platform::bc_script_variable run_script(bc_script_context p_context, const bcWCHAR* p_script);

			platform::bc_script_variable run_script(bc_script_context p_context, platform::bc_script_bytecode& p_script);

			core::bc_wstring stringify(platform::bc_script_variable& p_value);

			void interupt_script_execuation();

			void collect_garbage();

			bcSIZE memory_usage() const;

			void update(core_platform::bc_clock::update_param p_clock_update_param);

		protected:

		private:
			platform::bc_script_context& _get_context(bc_script_context p_context)
			{
				switch (p_context)
				{
				case bc_script_context::ui:
					return m_ui_context.get_context();
				}
			}

			platform::bc_script_runtime m_script_runtime;
			bc_script_ui_context m_ui_context;
			platform::bc_script_ref< platform::bc_script_function< platform::bc_script_string(platform::bc_script_variable) > > m_stringify_function;
		};
	}
}