 // [11/01/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Utility/bcInitializable.h"
#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptRuntime.h"
#include "PlatformImp/Script/bcScriptContext.hpp"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_script_ui_context : public core::bc_initializable< platform::bc_script_runtime& >
		{
		public:
			bc_script_ui_context();

			bc_script_ui_context(bc_script_ui_context&&) noexcept;

			~bc_script_ui_context();

			bc_script_ui_context& operator=(bc_script_ui_context&&) noexcept;

			platform::bc_script_context& get_context()
			{
				return m_context.get();
			}

			static bcINT get_count() noexcept;

			static void set_count(const bcINT& p_count) noexcept;

		protected:

		private:
			void _initialize(platform::bc_script_runtime& p_runtime) override;

			void _destroy() override;

			platform::bc_script_ref< platform::bc_script_context > m_context;

			static bcINT s_count;
		};
	}
}