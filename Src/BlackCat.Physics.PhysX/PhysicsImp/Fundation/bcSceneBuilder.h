// [16/12/2016 MRB]

#pragma once

#include "Physics/Fundation/bcSceneBuilder.h"
#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcImplementation.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_scene_builder_pack< g_api_physx >
		{
			bc_platform_scene_builder_pack()
				: m_px_desc(physx::PxTolerancesScale())
			{
			}

			physx::PxSceneDesc m_px_desc;
			core::bc_unique_ptr<bc_px_simulation_callback> m_simulation_callback;
			core::bc_unique_ptr<bc_px_contact_filter_callback> m_contact_filter_callback;
			core::bc_unique_ptr<bc_px_contact_modify_callback> m_contact_modify_callback;
			core::bc_unique_ptr<bc_px_filter_shader_data> m_filter_shader_data;
		};
	}
}