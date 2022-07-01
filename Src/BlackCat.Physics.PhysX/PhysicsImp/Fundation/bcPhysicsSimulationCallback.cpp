// [12/16/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/Fundation/bcPhysicsSimulationCallback.h"

namespace black_cat
{
	namespace physics
	{
		// provide definitions at first to prevent CLang 'explicit specialization of '' after instantiation' error

		template<>
		bc_platform_trigger_pair<g_api_physx>& bc_platform_trigger_pair<g_api_physx>::operator=(const bc_platform_trigger_pair& p_other) noexcept;

		template<>
		bc_platform_contact_pair<g_api_physx>& bc_platform_contact_pair<g_api_physx>::operator=(const bc_platform_contact_pair& p_other) noexcept;

		template<>
		bc_platform_contact_shape_pair<g_api_physx>& bc_platform_contact_shape_pair<g_api_physx>::operator=(const bc_platform_contact_shape_pair& p_other) noexcept;

		template<>
		bc_platform_contact_point<g_api_physx>& bc_platform_contact_point<g_api_physx>::operator=(const bc_platform_contact_point& p_other) noexcept;

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_trigger_pair<g_api_physx>::bc_platform_trigger_pair() noexcept
			: m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_trigger_pair<g_api_physx>::bc_platform_trigger_pair(platform_pack& p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_trigger_pair<g_api_physx>::bc_platform_trigger_pair(const bc_platform_trigger_pair& p_other) noexcept
		{
			operator=(p_other);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_trigger_pair<g_api_physx>::~bc_platform_trigger_pair()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_trigger_pair<g_api_physx>& bc_platform_trigger_pair<g_api_physx>::operator=(const bc_platform_trigger_pair& p_other) noexcept
		{
			m_pack.m_px_pair = p_other.m_pack.m_px_pair;
			m_pack.m_trigger_actor = p_other.m_pack.m_trigger_actor;
			m_pack.m_other_actor = p_other.m_pack.m_other_actor;
			m_pack.m_trigger_shape = p_other.m_pack.m_trigger_shape;
			m_pack.m_other_actor = p_other.m_pack.m_other_actor;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_rigid_actor* bc_platform_trigger_pair<g_api_physx>::get_trigger_actor() const noexcept
		{
			bc_rigid_actor::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_pair->triggerActor;
			
			m_pack.m_trigger_actor = bc_rigid_actor(l_pack);

			return &m_pack.m_trigger_actor;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_shape* bc_platform_trigger_pair<g_api_physx>::get_trigger_shape() const noexcept
		{
			bc_shape::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_pair->triggerShape;
			
			m_pack.m_trigger_shape = bc_shape(l_pack);

			return &m_pack.m_trigger_shape;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_rigid_actor* bc_platform_trigger_pair<g_api_physx>::get_other_actor() const noexcept
		{
			bc_rigid_actor::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_pair->otherActor;

			m_pack.m_other_actor = bc_rigid_actor(l_pack);
			
			return &m_pack.m_other_actor;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_shape* bc_platform_trigger_pair<g_api_physx>::get_other_shape() const noexcept
		{
			bc_shape::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_pair->otherShape;

			m_pack.m_other_shape = bc_shape(l_pack);
			
			return &m_pack.m_other_shape;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_trigger_type bc_platform_trigger_pair<g_api_physx>::trigger_type() const noexcept
		{
			if (m_pack.m_px_pair->status & physx::PxPairFlag::Enum::eNOTIFY_TOUCH_FOUND)
			{
				return bc_trigger_type::in_touch;
			}

			if (m_pack.m_px_pair->status & physx::PxPairFlag::Enum::eNOTIFY_TOUCH_LOST)
			{
				return bc_trigger_type::lost_touch;
			}

			BC_ASSERT(false);
			return bc_trigger_type::lost_touch;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_pair<g_api_physx>::bc_platform_contact_pair() noexcept
			: m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_pair<g_api_physx>::bc_platform_contact_pair(platform_pack& p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_pair<g_api_physx>::bc_platform_contact_pair(const bc_platform_contact_pair& p_other) noexcept
		{
			operator=(p_other);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_pair<g_api_physx>::~bc_platform_contact_pair()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_pair<g_api_physx>& bc_platform_contact_pair<g_api_physx>::operator=(const bc_platform_contact_pair& p_other) noexcept
		{
			m_pack.m_px_pair = p_other.m_pack.m_px_pair;
			m_pack.m_actor0 = p_other.m_pack.m_actor0;
			m_pack.m_actor1 = p_other.m_pack.m_actor1;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_rigid_actor* bc_platform_contact_pair<g_api_physx>::get_actor0() const noexcept
		{
			bc_rigid_actor::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_pair->actors[0];

			m_pack.m_actor0 = bc_rigid_actor(l_pack);

			return &m_pack.m_actor0;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_rigid_actor* bc_platform_contact_pair<g_api_physx>::get_actor1() const noexcept
		{
			bc_rigid_actor::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_pair->actors[1];

			m_pack.m_actor1 = bc_rigid_actor(l_pack);

			return &m_pack.m_actor1;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_point<g_api_physx>::bc_platform_contact_point() noexcept
			: m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_point<g_api_physx>::bc_platform_contact_point(platform_pack& p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_point<g_api_physx>::bc_platform_contact_point(const bc_platform_contact_point& p_other) noexcept
		{
			operator=(p_other);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_point<g_api_physx>::~bc_platform_contact_point()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_point<g_api_physx>& bc_platform_contact_point<g_api_physx>::operator=(const bc_platform_contact_point& p_other) noexcept
		{
			m_pack.m_px_point = p_other.m_pack.m_px_point;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_contact_point<g_api_physx>::get_position() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_point.position);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_contact_point<g_api_physx>::get_separation() const noexcept
		{
			return m_pack.m_px_point.separation;
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_contact_point<g_api_physx>::get_normal() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_point.normal);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_contact_point<g_api_physx>::get_face_index0() const noexcept
		{
			return m_pack.m_px_point.internalFaceIndex0;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_contact_point<g_api_physx>::get_face_index1() const noexcept
		{
			return m_pack.m_px_point.internalFaceIndex1;
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_contact_point<g_api_physx>::get_impulse() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_point.impulse);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_shape_pair<g_api_physx>::bc_platform_contact_shape_pair() noexcept
			: m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_shape_pair<g_api_physx>::bc_platform_contact_shape_pair(platform_pack& p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_shape_pair<g_api_physx>::bc_platform_contact_shape_pair(const bc_platform_contact_shape_pair& p_other) noexcept
		{
			operator=(p_other);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_shape_pair<g_api_physx>::~bc_platform_contact_shape_pair()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_shape_pair<g_api_physx>& bc_platform_contact_shape_pair<g_api_physx>::operator=(const bc_platform_contact_shape_pair& p_other) noexcept
		{
			m_pack.m_px_pair = p_other.m_pack.m_px_pair;
			m_pack.m_shape0 = p_other.m_pack.m_shape0;
			m_pack.m_shape1 = p_other.m_pack.m_shape1;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_shape* bc_platform_contact_shape_pair<g_api_physx>::get_shape0() const noexcept
		{
			bc_shape::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_pair->shapes[0];

			m_pack.m_shape0 = bc_shape(l_pack);
			
			return &m_pack.m_shape0;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_shape* bc_platform_contact_shape_pair<g_api_physx>::get_shape1() const noexcept
		{
			bc_shape::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_pair->shapes[1];

			m_pack.m_shape1 = bc_shape(l_pack);

			return &m_pack.m_shape1;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_contact_shape_pair<g_api_physx>::get_contacts(bc_contact_point* p_buffer, bcUINT32 p_buffer_size) const noexcept
		{
			const bcUINT32 l_written_count = m_pack.m_px_pair->extractContacts(reinterpret_cast<physx::PxContactPairPoint*>(p_buffer), p_buffer_size);

			bc_overwrite_output_array<bc_contact_point, physx::PxContactPairPoint>(p_buffer, l_written_count, [](physx::PxContactPairPoint& p_px_point)
			{
				bc_contact_point::platform_pack l_pack;
				l_pack.m_px_point = p_px_point;

				return bc_contact_point(l_pack);
			});

			return l_written_count;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_contact_shape_pair<g_api_physx>::get_contact_count() const noexcept
		{
			return m_pack.m_px_pair->contactCount;
		}
	}
}