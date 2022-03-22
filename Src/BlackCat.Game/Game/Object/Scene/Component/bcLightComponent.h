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
			bc_light_component(bc_actor_id p_actor_id, bc_actor_component_id p_id);
			
			bc_light_component(bc_light_component&& p_other) noexcept = default;

			~bc_light_component() override;

			bc_light_component& operator=(bc_light_component&& p_other) noexcept = default;

			bc_actor get_actor() const noexcept override;

			core::bc_vector3f get_world_position() const noexcept;

			const core::bc_matrix4f& get_world_transform() const noexcept;

			bc_light* get_light() const noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;
			
		private:
			bc_light_ptr m_light;
		};

		inline core::bc_vector3f bc_light_component::get_world_position() const noexcept
		{
			return m_light->get_transformation().get_translation();
		}

		inline const core::bc_matrix4f& bc_light_component::get_world_transform() const noexcept
		{
			return m_light->get_transformation();
		}

		inline bc_light* bc_light_component::get_light() const noexcept
		{
			return m_light.get();
		}
	}
}