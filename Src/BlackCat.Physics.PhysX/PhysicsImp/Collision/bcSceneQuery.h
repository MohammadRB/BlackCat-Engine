// [16/12/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "PhysicsImp/Body/bcRigidActor.h"
#include "PhysicsImp/Shape/bcShape.h"

#include "Physics/Collision/bcSceneQuery.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_scene_query_buffer_pack<g_api_physx, bc_platform_ray_hit<g_api_physx>>
		{
			core::bc_vector<physx::PxRaycastHit> m_touches;
			physx::PxRaycastBuffer m_px_query;

			bc_platform_scene_query_buffer_pack() = default;

			bc_platform_scene_query_buffer_pack(bcUINT32 p_touching_hits_count)
				: m_touches(p_touching_hits_count),
				m_px_query(m_touches.data(), p_touching_hits_count)
			{
			}
		};

		template<>
		struct bc_platform_scene_query_buffer_pack<g_api_physx, bc_platform_overlap_hit<g_api_physx>>
		{
			core::bc_vector<physx::PxOverlapHit> m_touches;
			physx::PxOverlapBuffer m_px_query;
			
			bc_platform_scene_query_buffer_pack() = default;

			bc_platform_scene_query_buffer_pack(bcUINT32 p_touching_hits_count)
				: m_touches(p_touching_hits_count),
				m_px_query(m_touches.data(), p_touching_hits_count)
			{
			}
		};

		template<>
		struct bc_platform_scene_query_buffer_pack<g_api_physx, bc_platform_sweep_hit<g_api_physx>>
		{
			core::bc_vector<physx::PxSweepHit> m_touches;
			physx::PxSweepBuffer m_px_query;

			bc_platform_scene_query_buffer_pack() = default;

			bc_platform_scene_query_buffer_pack(bcUINT32 p_touching_hits_count)
				: m_touches(p_touching_hits_count),
				m_px_query(m_touches.data(), p_touching_hits_count)
			{
			}
		};

		template<bc_physics_api TApi, class THit>
		bc_platform_scene_query_buffer<TApi, THit>::bc_platform_scene_query_buffer(bcUINT32 p_touching_hits_count) noexcept
			: m_pack(p_touching_hits_count)
		{
		}

		template<bc_physics_api TApi, class THit>
		bc_platform_scene_query_buffer<TApi, THit>::bc_platform_scene_query_buffer(platform_pack& p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<bc_physics_api TApi, class THit>
		bc_platform_scene_query_buffer<TApi, THit>::bc_platform_scene_query_buffer(const bc_platform_scene_query_buffer& p_other) noexcept
			: m_pack(p_other.m_pack)
		{
		}

		template<bc_physics_api TApi, class THit>
		bc_platform_scene_query_buffer<TApi, THit>::bc_platform_scene_query_buffer(bc_platform_scene_query_buffer&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack))
		{
		}

		template<bc_physics_api TApi, class THit>
		bc_platform_scene_query_buffer<TApi, THit>::~bc_platform_scene_query_buffer()
		{
		}

		template<bc_physics_api TApi, class THit>
		bc_platform_scene_query_buffer<TApi, THit>& bc_platform_scene_query_buffer<TApi, THit>::operator=(const bc_platform_scene_query_buffer& p_other) noexcept
		{
			m_pack = p_other.m_pack;
			return *this;
		}

		template<bc_physics_api TApi, class THit>
		bc_platform_scene_query_buffer<TApi, THit>& bc_platform_scene_query_buffer<TApi, THit>::operator=(bc_platform_scene_query_buffer&& p_other) noexcept
		{
			m_pack = std::move(p_other.m_pack);
			return *this;
		}

		template<bc_physics_api TApi, class THit>
		bool bc_platform_scene_query_buffer<TApi, THit>::has_block() const noexcept
		{
			return m_pack.m_px_query.hasBlock;
		}

		template<bc_physics_api TApi, class THit>
		THit bc_platform_scene_query_buffer<TApi, THit>::get_block() const noexcept
		{
			THit l_hit;
			l_hit.get_platform_pack().m_px_hit = m_pack.m_px_query.block;

			return l_hit;
		}

		template<bc_physics_api TApi, class THit>
		bcUINT32 bc_platform_scene_query_buffer<TApi, THit>::get_touch_count() const noexcept
		{
			return m_pack.m_px_query.nbTouches;
		}

		template<bc_physics_api TApi, class THit>
		THit bc_platform_scene_query_buffer<TApi, THit>::get_touch(bcUINT32 p_index) const noexcept
		{
			THit l_hit;
			l_hit.get_platform_pack().m_px_hit = m_pack.m_px_query.touches[p_index];

			return l_hit;
		}
	}
}