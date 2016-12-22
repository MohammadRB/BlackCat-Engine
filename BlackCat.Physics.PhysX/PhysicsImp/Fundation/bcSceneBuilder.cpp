// [12/16/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcImplementation.h"
#include "PhysicsImp/Fundation/bcSimulationEventCallback.h"
#include "PhysicsImp/Collision/bcContactModifyCallback.h"
#include "PhysicsImp/Collision/bcContactFilterCallback.h"
#include "PhysicsImp/Fundation/bcSceneBuilder.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_builder< g_api_physx >::bc_platform_scene_builder()
		{
			m_pack.m_fitler_shader_data = core::bc_make_unique<bc_px_fitler_shader_data>();

			m_pack.m_px_desc.filterShaderData = m_pack.m_fitler_shader_data.get();
			m_pack.m_px_desc.filterShaderDataSize = sizeof(bc_px_fitler_shader_data);
			m_pack.m_px_desc.filterShader = &bc_px_filter_shader;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_builder< g_api_physx >::bc_platform_scene_builder(bc_platform_scene_builder&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_builder< g_api_physx >::~bc_platform_scene_builder()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_builder< g_api_physx >& bc_platform_scene_builder< g_api_physx >::operator=(bc_platform_scene_builder&& p_other) noexcept
		{
			m_pack.m_px_desc = p_other.m_pack.m_px_desc;
			m_pack.m_simulation_callback = std::move(p_other.m_pack.m_simulation_callback);
			m_pack.m_contact_filter_callback = std::move(p_other.m_pack.m_contact_filter_callback);
			m_pack.m_contact_modify_callback = std::move(p_other.m_pack.m_contact_modify_callback);
			m_pack.m_fitler_shader_data = std::move(p_other.m_pack.m_fitler_shader_data);

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_builder< g_api_physx >& bc_platform_scene_builder< g_api_physx >::use_hint(const bc_scene_hint& p_hint) noexcept
		{
			m_pack.m_px_desc.limits.maxNbStaticShapes = p_hint.m_num_static_bodies;
			m_pack.m_px_desc.limits.maxNbDynamicShapes = p_hint.m_num_dynamic_bodies;
			m_pack.m_px_desc.limits.maxNbConstraints = p_hint.m_num_constraints;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_builder< g_api_physx >& bc_platform_scene_builder< g_api_physx >::gravity(const core::bc_vector3f& p_gravity)
		{
			m_pack.m_px_desc.gravity = physx::PxVec3(p_gravity.x, p_gravity.y, p_gravity.z);

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_builder< g_api_physx >& bc_platform_scene_builder< g_api_physx >::use_simulation_callback(core::bc_unique_ptr< bc_isimulation_event_callback > p_callback)
		{
			m_pack.m_simulation_callback = core::bc_make_unique<bc_px_simulation_callback>(std::move(p_callback));
			m_pack.m_px_desc.simulationEventCallback = m_pack.m_simulation_callback.get();

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_builder< g_api_physx >& bc_platform_scene_builder< g_api_physx >::use_contact_modify_callback(core::bc_unique_ptr< bc_icontact_modify_callback > p_callback)
		{
			m_pack.m_contact_modify_callback = core::bc_make_unique<bc_px_contact_modify_callback>(std::move(p_callback));
			m_pack.m_px_desc.contactModifyCallback = m_pack.m_contact_modify_callback.get();
			m_pack.m_px_desc.ccdContactModifyCallback = m_pack.m_contact_modify_callback.get();

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_builder< g_api_physx >& bc_platform_scene_builder< g_api_physx >::use_contact_filter_callback(core::bc_unique_ptr< bc_icontact_filter_callback > p_callback)
		{
			m_pack.m_contact_filter_callback = core::bc_make_unique<bc_px_contact_filter_callback>(std::move(p_callback));
			m_pack.m_px_desc.filterCallback = m_pack.m_contact_filter_callback.get();
			m_pack.m_fitler_shader_data->m_use_filter_callback = true;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_builder< g_api_physx >& bc_platform_scene_builder< g_api_physx >::enable_ccd(bcUINT32 p_max_ccd_pass)
		{
			m_pack.m_px_desc.flags |= physx::PxSceneFlag::eENABLE_CCD;
			m_pack.m_px_desc.ccdMaxPasses = p_max_ccd_pass;
			m_pack.m_fitler_shader_data->m_use_ccd = true;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_builder< g_api_physx >& bc_platform_scene_builder< g_api_physx >::enable_locking()
		{
			m_pack.m_px_desc.flags |= physx::PxSceneFlag::eREQUIRE_RW_LOCK;

			return *this;
		}
	}
}