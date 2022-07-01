// [12/16/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Collision/bcContactModifyCallback.h"
#include "PhysicsImp/bcUtility.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		bc_platform_contact_modify_pair<g_api_physx>& bc_platform_contact_modify_pair<g_api_physx>::operator=(const bc_platform_contact_modify_pair& p_other) noexcept;

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_modify_set<g_api_physx>::bc_platform_contact_modify_set() noexcept
			: m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_modify_set<g_api_physx>::bc_platform_contact_modify_set(platform_pack& p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_modify_set<g_api_physx>::bc_platform_contact_modify_set(const bc_platform_contact_modify_set& p_other) noexcept
		{
			m_pack.m_px_set = p_other.m_pack.m_px_set;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_modify_set<g_api_physx>::~bc_platform_contact_modify_set()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_modify_set<g_api_physx>& bc_platform_contact_modify_set<g_api_physx>::operator=(const bc_platform_contact_modify_set& p_other) noexcept
		{
			m_pack.m_px_set = p_other.m_pack.m_px_set;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_contact_modify_set<g_api_physx>::get_point(bcUINT32 p_index) const noexcept
		{
			const auto& l_px_vec = m_pack.m_px_set->getPoint(p_index);
			return bc_to_game_hand(l_px_vec);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_contact_modify_set<g_api_physx>::set_point(bcUINT32 p_index, const core::bc_vector3f& p_point) noexcept
		{
			m_pack.m_px_set->setPoint(p_index, bc_to_right_hand(p_point));
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_contact_modify_set<g_api_physx>::get_normal(bcUINT32 p_index) const noexcept
		{
			const auto& l_px_vec = m_pack.m_px_set->getNormal(p_index);
			return bc_to_game_hand(l_px_vec);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_contact_modify_set<g_api_physx>::set_normal(bcUINT32 p_index, const core::bc_vector3f& p_normal) noexcept
		{
			m_pack.m_px_set->setNormal(p_index, bc_to_right_hand(p_normal));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_contact_modify_set<g_api_physx>::get_separation(bcUINT32 p_index) const noexcept
		{
			return m_pack.m_px_set->getSeparation(p_index);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_contact_modify_set<g_api_physx>::set_separation(bcUINT32 p_index, bcFLOAT p_sep) noexcept
		{
			m_pack.m_px_set->setSeparation(p_index, p_sep);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_contact_modify_set<g_api_physx>::getInternalFaceIndex0(bcUINT32 p_index) const noexcept
		{
			return const_cast< bc_platform_contact_modify_set& >(*this).m_pack.m_px_set->getInternalFaceIndex0(p_index);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_contact_modify_set<g_api_physx>::getInternalFaceIndex1(bcUINT32 p_index) const noexcept
		{
			return const_cast< bc_platform_contact_modify_set& >(*this).m_pack.m_px_set->getInternalFaceIndex1(p_index);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_contact_modify_set<g_api_physx>::ignore(bcUINT32 p_index) noexcept
		{
			m_pack.m_px_set->ignore(p_index);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_contact_modify_set<g_api_physx>::size() const noexcept
		{
			return m_pack.m_px_set->size();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_modify_pair<g_api_physx>::bc_platform_contact_modify_pair() noexcept
		{
			m_pack.m_is_ccd = false;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_modify_pair<g_api_physx>::bc_platform_contact_modify_pair(platform_pack& p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_modify_pair<g_api_physx>::bc_platform_contact_modify_pair(const bc_platform_contact_modify_pair& p_other) noexcept
		{
			operator=(p_other);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_modify_pair<g_api_physx>::~bc_platform_contact_modify_pair()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_contact_modify_pair<g_api_physx>& bc_platform_contact_modify_pair<g_api_physx>::operator=(const bc_platform_contact_modify_pair& p_other) noexcept
		{
			m_pack.m_px_pair = p_other.m_pack.m_px_pair;
			m_pack.m_actor0 = p_other.m_pack.m_actor0;
			m_pack.m_actor1 = p_other.m_pack.m_actor1;
			m_pack.m_shape_0 = p_other.m_pack.m_shape_0;
			m_pack.m_shape_1 = p_other.m_pack.m_shape_1;
			m_pack.m_contacts = p_other.m_pack.m_contacts;
			m_pack.m_is_ccd = p_other.m_pack.m_is_ccd;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_rigid_actor& bc_platform_contact_modify_pair<g_api_physx>::get_actor0() const noexcept
		{
			bc_rigid_actor::platform_pack l_pack;
			l_pack.m_px_object = const_cast<physx::PxRigidActor*>(m_pack.m_px_pair->actor[0]);
			
			m_pack.m_actor0 = bc_rigid_actor(l_pack);

			return m_pack.m_actor0;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_rigid_actor& bc_platform_contact_modify_pair<g_api_physx>::get_actor1() const noexcept
		{
			bc_rigid_actor::platform_pack l_pack;
			l_pack.m_px_object = const_cast<physx::PxRigidActor*>(m_pack.m_px_pair->actor[1]);

			m_pack.m_actor1 = bc_rigid_actor(l_pack);

			return m_pack.m_actor1;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_shape& bc_platform_contact_modify_pair<g_api_physx>::get_shape0() const noexcept
		{
			bc_shape::platform_pack l_pack;
			l_pack.m_px_object = const_cast<physx::PxShape*>(m_pack.m_px_pair->shape[0]);
			
			m_pack.m_shape_0 = bc_shape(l_pack);

			return m_pack.m_shape_0;
		}

		template<>
		BC_PHYSICSIMP_DLL
		const bc_shape& bc_platform_contact_modify_pair<g_api_physx>::get_shape1() const noexcept
		{
			bc_shape::platform_pack l_pack;
			l_pack.m_px_object = const_cast<physx::PxShape*>(m_pack.m_px_pair->shape[1]);

			m_pack.m_shape_1 = bc_shape(l_pack);

			return m_pack.m_shape_1;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_contact_modify_pair<g_api_physx>::is_ccd_contact() const noexcept
		{
			return m_pack.m_is_ccd;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_contact_modify_set& bc_platform_contact_modify_pair<g_api_physx>::get_contacts() noexcept
		{
			bc_contact_modify_set::platform_pack l_pack;
			l_pack.m_px_set = &m_pack.m_px_pair->contacts;
			
			m_pack.m_contacts = bc_contact_modify_set(l_pack);

			return m_pack.m_contacts;
		}
	}
}