// [12/05/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Math/bcVector3f.h"
#include "Physics/bcPhysicsApi.h"

namespace black_cat
{
	namespace physics
	{
		class bc_isimulation_event_callback;
		class bc_icontact_modify_callback;
		class bc_icontact_filter_callback;

		struct bc_scene_hint
		{
		public:
			bc_scene_hint()
				: m_num_static_bodies(0),
				m_num_dynamic_bodies(0),
				m_num_constraints(0)
				//m_num_broadphase_regions(0)
			{
			}

			bc_scene_hint(bcUINT32 p_num_static_bodies,
				bcUINT32 p_num_dynamic_bodies,
				bcUINT32 p_num_constraints)
				: m_num_static_bodies(p_num_static_bodies),
				m_num_dynamic_bodies(p_num_dynamic_bodies),
				m_num_constraints(p_num_constraints)
				//m_num_broadphase_regions(p_num_broadphase_regions)
			{
			}

			bcUINT32 m_num_static_bodies;
			bcUINT32 m_num_dynamic_bodies;
			bcUINT32 m_num_constraints;
		};

		template< bc_physics_api TApi >
		struct bc_platform_scene_builder_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_scene_builder
		{
		public:
			using platform_pack = bc_platform_scene_builder_pack< TApi >;
			
		public:
			bc_platform_scene_builder();

			bc_platform_scene_builder(bc_platform_scene_builder&&) noexcept;

			~bc_platform_scene_builder();

			bc_platform_scene_builder& operator=(bc_platform_scene_builder&&) noexcept;

			bc_platform_scene_builder& use_hint(const bc_scene_hint& p_hint) noexcept;

			bc_platform_scene_builder& gravity(const core::bc_vector3f& p_gravity);

			bc_platform_scene_builder& use_simulation_callback(core::bc_unique_ptr<bc_isimulation_event_callback> p_callback);

			bc_platform_scene_builder& use_contact_modify_callback(core::bc_unique_ptr<bc_icontact_modify_callback> p_callback);

			bc_platform_scene_builder& use_contact_filter_callback(core::bc_unique_ptr<bc_icontact_filter_callback> p_callback);

			bc_platform_scene_builder& enable_ccd(bcUINT32 p_max_ccd_pass = 1);

			bc_platform_scene_builder& enable_locking();

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}
			
		protected:

		private:
			platform_pack m_pack;
		};

		using bc_scene_builder = bc_platform_scene_builder< g_current_physics_api >;
	}
}