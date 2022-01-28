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
			bc_checkpoint_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept;

			bc_checkpoint_component(bc_checkpoint_component&&) noexcept = default;

			~bc_checkpoint_component() override = default;

			bc_checkpoint_component& operator=(bc_checkpoint_component&&) noexcept = default;

			bc_actor get_actor() const noexcept override;
		};

		inline void bc_mark_actor_for_checkpoint(bc_actor& p_actor)
		{
			p_actor.get_create_component<bc_checkpoint_component>();
		}

		inline void bc_unmark_actor_for_checkpoint(bc_actor& p_actor)
		{
			const auto* l_checkpoint_component = p_actor.get_component<bc_checkpoint_component>();
			if (l_checkpoint_component)
			{
				p_actor.remove_component<bc_checkpoint_component>();
			}
		}
	}	
}