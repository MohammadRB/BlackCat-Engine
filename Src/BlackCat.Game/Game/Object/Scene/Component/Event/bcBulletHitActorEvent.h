// [04/04/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "PhysicsImp/Shape/bcShape.h"
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
			bc_bullet_hit_actor_event(const bc_actor& p_hit_actor,
				physics::bc_shape p_hit_shape,
				const core::bc_vector3f& p_hit_position,
				const core::bc_vector3f& p_hit_normal,
				bcUINT32 p_hit_face_index,
				bcFLOAT p_bullet_mass,
				bcFLOAT p_bullet_speed) noexcept;

			bc_bullet_hit_actor_event(const bc_bullet_hit_actor_event&) noexcept;

			~bc_bullet_hit_actor_event() override;

			bc_bullet_hit_actor_event& operator=(const bc_bullet_hit_actor_event&) noexcept;

			const bc_actor& get_hit_actor() const noexcept;
			
			physics::bc_shape get_hit_shape() const noexcept;
			
			const core::bc_vector3f& get_hit_position() const noexcept;

			const core::bc_vector3f& get_hit_normal() const noexcept;

			bcUINT32 get_hit_face_index() const noexcept;

			bcFLOAT get_bullet_mass() const noexcept;

			bcFLOAT get_bullet_speed() const noexcept;
		
		private:
			bc_actor m_hit_actor;
			physics::bc_shape m_hit_shape;
			core::bc_vector3f m_hit_position;
			core::bc_vector3f m_hit_normal;
			bcUINT32 m_hit_face_index;
			bcFLOAT m_bullet_mass;
			bcFLOAT m_bullet_speed;
		};

		inline bc_bullet_hit_actor_event::bc_bullet_hit_actor_event(const bc_actor& p_hit_actor,
			physics::bc_shape p_hit_shape,
			const core::bc_vector3f& p_hit_position,
			const core::bc_vector3f& p_hit_normal,
			bcUINT32 p_hit_face_index,
			bcFLOAT p_bullet_mass,
			bcFLOAT p_bullet_speed) noexcept
			: bc_actor_event(get_message_name()),
			m_hit_actor(p_hit_actor),
			m_hit_shape(p_hit_shape),
			m_hit_position(p_hit_position),
			m_hit_normal(p_hit_normal),
			m_hit_face_index(p_hit_face_index),
			m_bullet_mass(p_bullet_mass),
			m_bullet_speed(p_bullet_speed)
		{
		}

		inline bc_bullet_hit_actor_event::bc_bullet_hit_actor_event(const bc_bullet_hit_actor_event&) noexcept = default;

		inline bc_bullet_hit_actor_event::~bc_bullet_hit_actor_event() = default;

		inline bc_bullet_hit_actor_event& bc_bullet_hit_actor_event::operator=(const bc_bullet_hit_actor_event&) noexcept = default;

		inline const bc_actor& bc_bullet_hit_actor_event::get_hit_actor() const noexcept
		{
			return m_hit_actor;
		}

		inline physics::bc_shape bc_bullet_hit_actor_event::get_hit_shape() const noexcept
		{
			return m_hit_shape;
		}

		inline const core::bc_vector3f& bc_bullet_hit_actor_event::get_hit_position() const noexcept
		{
			return m_hit_position;
		}

		inline const core::bc_vector3f& bc_bullet_hit_actor_event::get_hit_normal() const noexcept
		{
			return m_hit_normal;
		}

		inline bcUINT32 bc_bullet_hit_actor_event::get_hit_face_index() const noexcept
		{
			return m_hit_face_index;
		}

		inline bcFLOAT bc_bullet_hit_actor_event::get_bullet_mass() const noexcept
		{
			return m_bullet_mass;
		}

		inline bcFLOAT bc_bullet_hit_actor_event::get_bullet_speed() const noexcept
		{
			return m_bullet_speed;
		}
	}	
}