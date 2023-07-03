// [22/03/2021 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/Fundation/bcCController.h"
#include "PhysicsImp/Fundation/bcCControllerSimulationCallback.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/bcExport.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ccontroller_shape_hit<g_api_physx>::bc_platform_ccontroller_shape_hit(platform_pack& p_pack)
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL bc_platform_ccontroller<g_api_physx> bc_platform_ccontroller_shape_hit<g_api_physx>::get_ccontroller() const noexcept
		{
			bc_platform_ccontroller<g_api_physx>::platform_pack l_pack;
			l_pack.m_controller = m_pack.m_px_hit->controller;
			
			return bc_platform_ccontroller<g_api_physx>(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_ccontroller_shape_hit<g_api_physx>::get_world_pos() const noexcept
		{
			return bc_to_game_hand_ex(m_pack.m_px_hit->worldPos);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_ccontroller_shape_hit<g_api_physx>::get_world_normal() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_hit->worldNormal);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_ccontroller_shape_hit<g_api_physx>::get_move_dir() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_hit->dir);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_ccontroller_shape_hit<g_api_physx>::get_move_length() const noexcept
		{
			return m_pack.m_px_hit->length;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_actor bc_platform_ccontroller_shape_hit<g_api_physx>::get_actor() const noexcept
		{
			bc_platform_rigid_actor<g_api_physx> l_actor;
			static_cast<bc_physics_reference&>(l_actor).get_platform_pack().m_px_object = m_pack.m_px_hit->actor;

			return l_actor;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape bc_platform_ccontroller_shape_hit<g_api_physx>::get_shape() const noexcept
		{
			bc_platform_shape<g_api_physx>::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_hit->shape;
			
			return bc_platform_shape<g_api_physx>(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_ccontroller_shape_hit<g_api_physx>::get_triangle_index() const noexcept
		{
			return m_pack.m_px_hit->triangleIndex;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ccontroller_controller_hit<g_api_physx>::bc_platform_ccontroller_controller_hit(platform_pack& p_pack)
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ccontroller<g_api_physx> bc_platform_ccontroller_controller_hit<g_api_physx>::get_ccontroller() const noexcept
		{
			bc_platform_ccontroller<g_api_physx>::platform_pack l_pack;
			l_pack.m_controller = m_pack.m_px_hit->controller;
			
			return bc_platform_ccontroller<g_api_physx>(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_ccontroller_controller_hit<g_api_physx>::get_world_pos() const noexcept
		{
			return bc_to_game_hand_ex(m_pack.m_px_hit->worldPos);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_ccontroller_controller_hit<g_api_physx>::get_world_normal() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_hit->worldNormal);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_ccontroller_controller_hit<g_api_physx>::get_move_dir() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_hit->dir);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_ccontroller_controller_hit<g_api_physx>::get_move_length() const noexcept
		{
			return m_pack.m_px_hit->length;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ccontroller<g_api_physx> bc_platform_ccontroller_controller_hit<g_api_physx>::get_other_ccontroller() const noexcept
		{
			bc_platform_ccontroller<g_api_physx>::platform_pack l_pack;
			l_pack.m_controller = m_pack.m_px_hit->other;
			
			return bc_platform_ccontroller<g_api_physx>(l_pack);
		}
	}	
}