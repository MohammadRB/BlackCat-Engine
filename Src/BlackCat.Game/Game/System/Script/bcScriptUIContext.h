 // [11/01/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptRuntime.h"
#include "PlatformImp/Script/bcScriptContext.hpp"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_script_ui_context
		{
		public:
			bc_script_ui_context(platform::bc_script_runtime& p_runtime);

			bc_script_ui_context(bc_script_ui_context&&) noexcept;

			~bc_script_ui_context();

			bc_script_ui_context& operator=(bc_script_ui_context&&) noexcept;

			platform::bc_script_context& get_context()
			{
				return m_context.get();
			}

		protected:

		private:
			platform::bc_script_context_ref m_context;
		};
	}
}