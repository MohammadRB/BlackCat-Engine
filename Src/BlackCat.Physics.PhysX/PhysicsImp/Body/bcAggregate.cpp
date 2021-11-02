// [12/10/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/Body/bcActor.h"
#include "PhysicsImp/Body/bcAggregate.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_aggregate< g_api_physx >::bc_platform_aggregate() noexcept
			: bc_platform_physics_reference(),
			m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_aggregate<g_api_physx>::bc_platform_aggregate(platform_pack& p_pack) noexcept
			: bc_platform_physics_reference(),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_aggregate< g_api_physx >::bc_platform_aggregate(const bc_platform_aggregate& p_other) noexcept
			: bc_platform_physics_reference(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_aggregate< g_api_physx >::~bc_platform_aggregate()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_aggregate< g_api_physx >& bc_platform_aggregate< g_api_physx >::operator=(const bc_platform_aggregate& p_other) noexcept
		{
			bc_platform_physics_reference::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_aggregate< g_api_physx >::add_actor(bc_actor& p_actor) noexcept
		{
			auto* l_pointer = static_cast<physx::PxAggregate*>(m_pack.m_px_object);
			auto* l_actor_pointer = static_cast<physx::PxActor*>(p_actor.get_platform_pack().m_px_object);

			return l_pointer->addActor(*l_actor_pointer);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_aggregate< g_api_physx >::remove_actor(bc_actor& p_actor) noexcept
		{
			auto* l_px_aggregate = static_cast<physx::PxAggregate*>(m_pack.m_px_object);
			auto* l_px_actor = static_cast<physx::PxActor*>(p_actor.get_platform_pack().m_px_object);

			return l_px_aggregate->removeActor(*l_px_actor);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_aggregate< g_api_physx >::get_actor_count() const noexcept
		{
			auto* l_px_aggregate = static_cast<physx::PxAggregate*>(m_pack.m_px_object);
			
			return l_px_aggregate->getNbActors();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_aggregate< g_api_physx >::get_actors(bc_actor* p_buffer, bcUINT32 p_buffer_size, bcUINT32 p_start_index) const noexcept
		{
			auto* l_px_aggregate = static_cast<physx::PxAggregate*>(m_pack.m_px_object);
			const bcUINT32 l_written_count = l_px_aggregate->getActors(reinterpret_cast<physx::PxActor**>(p_buffer), p_buffer_size, p_start_index);

			bc_overwrite_output_array<bc_actor, physx::PxActor*>(p_buffer, l_written_count, [](physx::PxActor*& p_px_actor)
			{
				bc_actor::platform_pack l_pack;
				l_pack.m_px_object = p_px_actor;

				return bc_actor(l_pack);
			});

			return l_written_count;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_aggregate<g_api_physx>::is_valid() const noexcept
		{
			return m_pack.m_px_object != nullptr;
		}
	}
}