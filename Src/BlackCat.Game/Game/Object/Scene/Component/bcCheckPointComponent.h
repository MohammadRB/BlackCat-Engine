// [01/27/2022 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		/**
		 * \brief Tag component which indicates actor state must be preserved in scene checkpoint process
		 */
		class BC_GAME_DLL bc_checkpoint_component : public bci_actor_component
		{
			BC_COMPONENT(chk_pnt, false, false)

		public:
			bc_checkpoint_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept;

			bc_checkpoint_component(bc_checkpoint_component&&) noexcept = default;

			~bc_checkpoint_component() override = default;

			bc_checkpoint_component& operator=(bc_checkpoint_component&&) noexcept = default;

			bc_actor get_actor() const noexcept override;
		};
	}	
}