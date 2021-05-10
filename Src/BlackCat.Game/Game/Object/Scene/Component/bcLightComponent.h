// [03/01/2019 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/System/Render/Light/bcLight.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_light_component : public bci_actor_component
		{
			BC_COMPONENT(light, true, false)

		public:
			bc_light_component(bc_actor_index p_actor_index, bc_actor_component_index p_index);
			
			bc_light_component(bc_light_component&& p_other) noexcept = default;

			~bc_light_component() override;

			bc_light_component& operator=(bc_light_component&& p_other) noexcept = default;

			bc_actor get_actor() const noexcept override;

			core::bc_vector3f get_world_position() const noexcept;

			const core::bc_matrix4f& get_world_transform() const noexcept;

			bc_light* get_light() noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;
			
		private:
			bc_light_ptr m_light;
		};
	}
}