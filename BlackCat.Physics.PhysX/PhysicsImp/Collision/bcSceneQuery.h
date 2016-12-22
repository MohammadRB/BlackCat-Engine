// [12/16/2016 MRB]

#pragma once

#include "PhysicsImp/Collision/bcShapeQuery.h"

#include "Core/Container/bcVector.h"
#include "Physics/Collision/bcSceneQuery.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_scene_query_buffer_pack< g_api_physx, bc_platform_ray_hit< g_api_physx > >
		{
			physx::PxRaycastBuffer m_px_query;
			mutable bc_platform_ray_hit< g_api_physx > m_block;
			mutable core::bc_vector< bc_platform_ray_hit< g_api_physx > > m_touches;
		};

		template<>
		struct bc_platform_scene_query_buffer_pack< g_api_physx, bc_platform_overlap_hit< g_api_physx > >
		{
			physx::PxOverlapBuffer m_px_query;
			mutable bc_platform_overlap_hit< g_api_physx > m_block;
			mutable core::bc_vector< bc_platform_overlap_hit< g_api_physx > > m_touches;
		};

		template<>
		struct bc_platform_scene_query_buffer_pack< g_api_physx, bc_platform_sweep_hit< g_api_physx > >
		{
			physx::PxSweepBuffer m_px_query;
			mutable bc_platform_sweep_hit< g_api_physx > m_block;
			mutable core::bc_vector< bc_platform_sweep_hit< g_api_physx > > m_touches;
		};

		template<bc_physics_api TApi, class THit>
		bc_platform_scene_query_buffer< TApi, THit >::bc_platform_scene_query_buffer(bcUINT32 p_touching_hit_count)
		{
			m_pack.m_touches.reserve(p_touching_hit_count);
		}

		template<bc_physics_api TApi, class THit>
		bc_platform_scene_query_buffer< TApi, THit >::bc_platform_scene_query_buffer(const bc_platform_scene_query_buffer& p_other) noexcept
		{
			operator=(p_other);
		}

		template<bc_physics_api TApi, class THit>
		bc_platform_scene_query_buffer< TApi, THit >::~bc_platform_scene_query_buffer()
		{
		}

		template<bc_physics_api TApi, class THit>
		bc_platform_scene_query_buffer< TApi, THit >& bc_platform_scene_query_buffer< TApi, THit >::operator=(const bc_platform_scene_query_buffer& p_other) noexcept
		{
			m_pack.m_px_query = p_other.m_pack.m_px_query;
			m_pack.m_block = p_other.m_pack.m_block;
			m_pack.m_touches = p_other.m_pack.m_touches;

			return *this;
		}

		template<bc_physics_api TApi, class THit>
		bool bc_platform_scene_query_buffer< TApi, THit >::has_block() const noexcept
		{
			return m_pack.m_px_query.hasBlock;
		}

		template<bc_physics_api TApi, class THit>
		const THit* bc_platform_scene_query_buffer< TApi, THit >::get_block() const noexcept
		{
			m_pack.m_block.get_platform_pack().m_px_hit = m_pack.m_px_query.block;

			return &m_pack.m_block;
		}

		template<bc_physics_api TApi, class THit>
		bcUINT32 bc_platform_scene_query_buffer< TApi, THit >::get_touch_count() const noexcept
		{
			return m_pack.m_px_query.nbTouches;
		}

		template<bc_physics_api TApi, class THit>
		const THit* bc_platform_scene_query_buffer< TApi, THit >::get_touches() const noexcept
		{
			if (m_pack.m_touches.size() == 0)
			{
				m_pack.m_touches.resize(m_pack.m_touches.capacity());

				for (bcUINT32 i = 0; i < m_pack.m_touches.size(); ++i)
				{
					m_pack.m_touches[i].get_platform_pack().m_px_hit = m_pack.m_px_query.touches[i];
				}
			}

			return m_pack.m_touches.data();
		}
	}
}