// [24/12/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Game/System/Network/Server/bcNetworkClient.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"

namespace black_cat::game
{
	class bc_explosion_actor_event : public bc_actor_event
	{
		BC_EVENT(explos)

	public:
		bc_explosion_actor_event(const core::bc_vector3f& p_center, 
		                         bcFLOAT p_radius, 
		                         bcFLOAT p_force, 
		                         bc_network_client_id p_player_id = bc_network_client::invalid_id);

		bc_explosion_actor_event(const bc_explosion_actor_event&) noexcept = default;

		~bc_explosion_actor_event() override = default;

		bc_explosion_actor_event& operator=(const bc_explosion_actor_event&) noexcept = default;

		bc_network_client_id get_player_id() const noexcept;

		const core::bc_vector3f& get_center() const noexcept;

		bcFLOAT get_radius() const noexcept;

		bcFLOAT get_force() const noexcept;

		std::pair<core::bc_vector3f, bcFLOAT> calculate_applied_force(const core::bc_vector3f& p_position) const;

	private:
		bc_network_client_id m_player_id;
		core::bc_vector3f m_center;
		bcFLOAT m_radius;
		bcFLOAT m_force;
	};

	inline bc_explosion_actor_event::bc_explosion_actor_event(const core::bc_vector3f& p_center,
	                                                          bcFLOAT p_radius,
	                                                          bcFLOAT p_force,
	                                                          bc_network_client_id p_player_id)
		: bc_actor_event(message_name()),
		  m_player_id(p_player_id),
		  m_center(p_center),
		  m_radius(p_radius),
		  m_force(p_force)
	{
	}

	inline bc_network_client_id bc_explosion_actor_event::get_player_id() const noexcept
	{
		return m_player_id;
	}

	inline const core::bc_vector3f& bc_explosion_actor_event::get_center() const noexcept
	{
		return m_center;
	}

	inline bcFLOAT bc_explosion_actor_event::get_radius() const noexcept
	{
		return m_radius;
	}

	inline bcFLOAT bc_explosion_actor_event::get_force() const noexcept
	{
		return m_force;
	}

	inline std::pair<core::bc_vector3f, bcFLOAT> bc_explosion_actor_event::calculate_applied_force(const core::bc_vector3f& p_position) const
	{
		const auto l_force_dir = p_position - m_center;
		const auto l_force_amount = std::max(0.f, m_radius - l_force_dir.magnitude()) / m_radius * m_force;

		return std::make_pair(core::bc_vector3f::normalize(l_force_dir), l_force_amount);
	}
}
