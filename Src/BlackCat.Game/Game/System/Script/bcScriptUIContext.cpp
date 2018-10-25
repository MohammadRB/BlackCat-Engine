 // [11/01/2016 MRB]

#include "Game/GamePCH.h"
#include "PlatformImp/Script/bcScriptGlobalPrototypeBuilder.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"
#include "Game/System/Script/bcScriptUIContext.h"

namespace black_cat
{
	namespace game
	{
		bc_script_ui_context::bc_script_ui_context()
			: m_context()
		{
		}

		bc_script_ui_context::bc_script_ui_context(bc_script_ui_context&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_script_ui_context::~bc_script_ui_context()
		{
		}

		bc_script_ui_context& bc_script_ui_context::operator=(bc_script_ui_context&& p_other) noexcept
		{
			m_context = std::move(p_other.m_context);

			return *this;
		}

		void bc_script_ui_context::_initialize(platform::bc_script_runtime& p_runtime)
		{
			m_context = p_runtime.create_context();
		}

		void bc_script_ui_context::_destroy()
		{
			m_context.reset();
		}
	}
}