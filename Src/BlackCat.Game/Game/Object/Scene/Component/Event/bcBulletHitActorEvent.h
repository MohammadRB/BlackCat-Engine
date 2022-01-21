// [04/04/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "PhysicsImp/Body/bcRigidActor.h"
#include "Game/System/Network/Server/bcNetworkClient.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"

namespace black_cat
{
	namespace game
	{
		class bc_bullet_hit_actor_event : public bc_actor_event
		{
			BC_EVENT(bl_hit)
			
		public:
			bc_bullet_hit_actor_event(const bc_actor& p_actor,
				physics::bc_rigid_actor p_px_actor,
				physics::bc_shape p_shape,
				const core::bc_vector3f& p_position,
				const core::bc_vector3f& p_normal,
				bcUINT32 p_face_index,
				bc_network_client_id p_bullet_player_id,
				const core::bc_vector3f& p_bullet_dir,
				bcFLOAT p_bullet_speed,
				bcFLOAT p_bullet_mass) noexcept;

			bc_bullet_hit_actor_event(const bc_bullet_hit_actor_event&) noexcept = default;

			~bc_bullet_hit_actor_event() override = default;

			bc_bullet_hit_actor_event& operator=(const bc_bullet_hit_actor_event&) noexcept = default;

			const bc_actor& get_hit_actor() const noexcept;
			
			const physics::bc_rigid_actor& get_hit_px_actor() const noexcept;
			
			physics::bc_shape get_hit_shape() const noexcept;
			
			const core::bc_vector3f& get_hit_position() const noexcept;

			const core::bc_vector3f& get_hit_normal() const noexcept;

			bcUINT32 get_hit_face_index() const noexcept;

			bc_network_client_id get_bullet_player_id() const noexcept;

			const core::bc_vector3f& get_bullet_direction() const noexcept;
			
			bcFLOAT get_bullet_speed() const noexcept;
			
			bcFLOAT get_bullet_mass() const noexcept;

			bcFLOAT calculate_applied_force() const noexcept;
		
		private:
			bc_actor m_actor;
			physics::bc_rigid_actor m_px_actor;
			physics::bc_shape m_shape;
			core::bc_vector3f m_position;
			core::bc_vector3f m_normal;
			bcUINT32 m_face_index;
			bc_network_client_id m_bullet_player_id;
			core::bc_vector3f m_bullet_dir;
			bcFLOAT m_bullet_speed;
			bcFLOAT m_bullet_mass;
		};

		inline bc_bullet_hit_actor_event::bc_bullet_hit_actor_event(const bc_actor& p_actor,
			physics::bc_rigid_actor p_px_actor,
			physics::bc_shape p_shape,
			const core::bc_vector3f& p_position,
			const core::bc_vector3f& p_normal,
			bcUINT32 p_face_index,
			bc_network_client_id p_bullet_player_id,
			const core::bc_vector3f& p_bullet_dir,
			bcFLOAT p_bullet_speed,
			bcFLOAT p_bullet_mass) noexcept
			: bc_actor_event(message_name()),
			m_actor(p_actor),
			m_px_actor(p_px_actor),
			m_shape(p_shape),
			m_position(p_position),
			m_normal(p_normal),
			m_face_index(p_face_index),
			m_bullet_player_id(p_bullet_player_id),
			m_bullet_dir(p_bullet_dir),
			m_bullet_speed(p_bullet_speed),
			m_bullet_mass(p_bullet_mass)
		{
		}

		inline const bc_actor& bc_bullet_hit_actor_event::get_hit_actor() const noexcept
		{
			return m_actor;
		}

		inline const physics::bc_rigid_actor& bc_bullet_hit_actor_event::get_hit_px_actor() const noexcept
		{
			return m_px_actor;
		}

		inline physics::bc_shape bc_bullet_hit_actor_event::get_hit_shape() const noexcept
		{
			return m_shape;
		}

		inline const core::bc_vector3f& bc_bullet_hit_actor_event::get_hit_position() const noexcept
		{
			return m_position;
		}

		inline const core::bc_vector3f& bc_bullet_hit_actor_event::get_hit_normal() const noexcept
		{
			return m_normal;
		}

		inline bcUINT32 bc_bullet_hit_actor_event::get_hit_face_index() const noexcept
		{
			return m_face_index;
		}

		inline bc_network_client_id bc_bullet_hit_actor_event::get_bullet_player_id() const noexcept
		{
			return m_bullet_player_id;
		}

		inline const core::bc_vector3f& bc_bullet_hit_actor_event::get_bullet_direction() const noexcept
		{
			return m_bullet_dir;
		}

		inline bcFLOAT bc_bullet_hit_actor_event::get_bullet_speed() const noexcept
		{
			return m_bullet_speed;
		}
		
		inline bcFLOAT bc_bullet_hit_actor_event::get_bullet_mass() const noexcept
		{
			return m_bullet_mass;
		}

		inline bcFLOAT bc_bullet_hit_actor_event::calculate_applied_force() const noexcept
		{
			return m_bullet_speed * (1.f / (1 - m_bullet_mass)) * 10;
		}
	}	
}
