// [04/28/2016 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_state_buffer;

		class BC_GAME_DLL bc_render_component : public bci_actor_component
		{
			BC_ABSTRACT_COMPONENT(render)

		public:
			virtual void render(bc_render_state_buffer& p_buffer) const = 0;

		protected:
			explicit bc_render_component(bc_actor_component_index p_index);

			bc_render_component(bc_render_component&&) noexcept;

			virtual ~bc_render_component();

			bc_render_component& operator=(bc_render_component&&) noexcept;
		};
	}
}