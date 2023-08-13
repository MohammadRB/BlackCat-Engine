// [04/04/2022 MRB]

#pragma once

#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	enum class bc_pickup_proxy_state
	{
		no_result,
		no_pickup,
		pickup
	};

	struct bc_pickup_proxy_result
	{
		bc_pickup_proxy_state m_state;
		physics::bc_ray_hit m_ray_hit;
	};

	class BC_GAME_DLL bc_pickup_proxy_component : public bci_actor_abstract_component
	{
		BC_ABSTRACT_COMPONENT(pck_up)

	public:
		/**
		 * \brief Test to see if a ray collide with actor
		 * \param p_ray 
		 * \return 
		 */
		virtual bc_pickup_proxy_result ray_pickup(const physics::bc_ray& p_ray) const = 0;

	protected:
		bc_pickup_proxy_component() = default;

		bc_pickup_proxy_component(bc_pickup_proxy_component&&) noexcept = default;

		~bc_pickup_proxy_component() override = default;

		bc_pickup_proxy_component& operator=(bc_pickup_proxy_component&&) noexcept = default;
	};
}
