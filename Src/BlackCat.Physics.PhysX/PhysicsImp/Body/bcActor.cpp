// [12/08/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Body/bcActor.h"
#include "PhysicsImp/Body/bcAggregate.h"
#include "PhysicsImp/Body/bcRigidActor.h"
#include "PhysicsImp/Body/bcRigidBody.h"
#include "PhysicsImp/Body/bcRigidStatic.h"
#include "PhysicsImp/Body/bcRigidDynamic.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_actor<g_api_physx>::bc_platform_actor() noexcept
			: bc_platform_physics_reference(),
			m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_actor<g_api_physx>::bc_platform_actor(platform_pack& p_pack) noexcept
			: bc_platform_physics_reference(),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_actor<g_api_physx>::bc_platform_actor(const bc_platform_actor& p_other) noexcept
			: bc_platform_physics_reference(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_actor<g_api_physx>::~bc_platform_actor()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_actor<g_api_physx>& bc_platform_actor<g_api_physx>::operator=(const bc_platform_actor& p_other) noexcept
		{
			bc_platform_physics_reference::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_actor_type bc_platform_actor<g_api_physx>::get_type() const noexcept
		{
			auto* l_px_actor = static_cast<physx::PxActor*>(m_pack.m_px_object);

			return static_cast<bc_actor_type>(l_px_actor->getType());
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_actor bc_platform_actor<g_api_physx>::is_rigid_actor() const noexcept
		{
			auto* l_px_actor = static_cast<physx::PxActor*>(m_pack.m_px_object);
			auto* l_px_rigid_actor = l_px_actor->is<physx::PxRigidActor>();

			bc_rigid_actor::platform_pack l_rigid_actor_pack;
			l_rigid_actor_pack.m_px_object = l_px_rigid_actor;
			
			return bc_rigid_actor(l_rigid_actor_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_body bc_platform_actor<g_api_physx>::is_rigid_body() const noexcept
		{
			auto* l_px_actor = static_cast<physx::PxActor*>(m_pack.m_px_object);
			auto* l_px_rigid_body = l_px_actor->is<physx::PxRigidBody>();

			bc_rigid_body::platform_pack l_rigid_body_pack;
			l_rigid_body_pack.m_px_object = l_px_rigid_body;

			return bc_rigid_body(l_rigid_body_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_static bc_platform_actor<g_api_physx>::is_rigid_static() const noexcept
		{
			auto* l_px_actor = static_cast<physx::PxActor*>(m_pack.m_px_object);
			auto* l_px_rigid_static = l_px_actor->is<physx::PxRigidStatic>();

			bc_rigid_static::platform_pack l_rigid_static_pack;
			l_rigid_static_pack.m_px_object = l_px_rigid_static;
			
			return bc_rigid_static(l_rigid_static_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_dynamic bc_platform_actor<g_api_physx>::is_rigid_dynamic() const noexcept
		{
			auto* l_px_actor = static_cast<physx::PxActor*>(m_pack.m_px_object);
			auto* l_px_rigid_dynamic = l_px_actor->is<physx::PxRigidDynamic>();

			bc_rigid_dynamic::platform_pack l_rigid_dynamic_pack;
			l_rigid_dynamic_pack.m_px_object = l_px_rigid_dynamic;

			return bc_rigid_dynamic(l_rigid_dynamic_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_bound_box bc_platform_actor<g_api_physx>::get_world_bound() const noexcept
		{
			auto* l_px_actor = static_cast<physx::PxActor*>(m_pack.m_px_object);

			bc_bound_box::platform_pack l_pack;
			l_pack.m_bound = l_px_actor->getWorldBounds();

			return bc_bound_box(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_actor_flag bc_platform_actor<g_api_physx>::get_actor_flags() const noexcept
		{
			auto* l_px_actor = static_cast<physx::PxActor*>(m_pack.m_px_object);
			return static_cast<bc_actor_flag>(static_cast<physx::PxActorFlags::InternalType>(l_px_actor->getActorFlags()));
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_actor<g_api_physx>::set_actor_flag(bc_actor_flag p_flag, bool p_value) noexcept
		{
			auto* l_px_actor = static_cast<physx::PxActor*>(m_pack.m_px_object);
			l_px_actor->setActorFlag(static_cast<physx::PxActorFlag::Enum>(p_flag), p_value);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_aggregate bc_platform_actor<g_api_physx>::get_aggregate() const noexcept
		{
			auto* l_px_actor = static_cast<physx::PxActor*>(m_pack.m_px_object);

			bc_aggregate::platform_pack l_pack;
			l_pack.m_px_object = l_px_actor->getAggregate();
			
			return bc_aggregate(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_actor<g_api_physx>::is_valid() const noexcept
		{
			return m_pack.m_px_object != nullptr;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_actor<g_api_physx>::set_data(void* p_user_data) noexcept
		{
			auto* l_px_actor = static_cast<physx::PxActor*>(m_pack.m_px_object);

			l_px_actor->userData = p_user_data;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void* bc_platform_actor<g_api_physx>::get_data() const noexcept
		{
			auto* l_px_actor = static_cast<physx::PxActor*>(m_pack.m_px_object);

			return l_px_actor->userData;
		}
	}
}