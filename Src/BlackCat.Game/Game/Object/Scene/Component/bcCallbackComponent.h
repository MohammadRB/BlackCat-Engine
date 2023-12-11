// [24/02/2022 MRB]

#pragma once

#include "Core/Utility/bcDelegate.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class BC_GAME_DLL bc_callback_component : public bci_actor_component
	{
		BC_COMPONENT(cll_bck, true, true)

	public:
		enum class call_mode { once, loop };
		using callback = core::bc_delegate<void(const bc_actor&)>;

	public:
		bc_callback_component(bc_actor_id p_actor_id, bc_actor_component_id p_id);

		bc_callback_component(bc_callback_component&& p_other) noexcept = default;

		~bc_callback_component() override = default;

		bc_callback_component& operator=(bc_callback_component&&) noexcept = default;
		
		void update(const bc_actor_component_update_content& p_context) override;
			
		void set_callback(call_mode p_call_mode, bcFLOAT p_delay_ms, callback p_callback) noexcept;

		void set_as_auto_remove(bcFLOAT p_delay_ms);

	private:
		call_mode m_call_mode;
		bcFLOAT m_delay_ms;
		bcFLOAT m_current_delay;
		callback m_callback;
	};
}
