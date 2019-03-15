// [02/23/2019 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcObjectPool.h"
#include "Core/Utility/bcObjectPoolAllocator.h"
#include "Core/Container/bcList.h"
#include "Core/Container/bcVector.h"
#include "Game/bcExport.h"
#include "Game/System/Render/Light/bcLight.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat
{
	namespace game
	{
		struct bc_light_instance
		{
			bc_light_instance(const bc_light& p_light, const core::bc_vector3f& p_top_left_min_z, const core::bc_vector3f& p_bottom_right_max_z)
				: m_instance(p_light),
				m_top_left_min_z(p_top_left_min_z),
				m_bottom_right_max_z(p_bottom_right_max_z)
			{
			}

			const bc_light& m_instance;
			core::bc_vector3f m_top_left_min_z;
			core::bc_vector3f m_bottom_right_max_z;
		};

		class BC_GAME_DLL bc_light_manager
		{
		public:
			bc_light_manager();

			bc_light_manager(bc_light_manager&& p_other) noexcept;

			~bc_light_manager();

			bc_light_manager& operator=(bc_light_manager&& p_other) noexcept;

			bc_light_ptr add_light(const bc_direct_light& p_light);

			bc_light_ptr add_light(const bc_point_light& p_light);

			bc_light_ptr add_light(const bc_spot_light& p_light);

			core::bc_vector_frame<bc_light_instance> get_light_instances(const bc_icamera& p_camera);

			void update(const core_platform::bc_clock::update_param& p_clock);

			void destroy_light(bc_light* p_light);

		private:
			bc_light_instance _get_light_bounds(const bc_icamera& p_camera, const bc_light& p_light, const bc_direct_light& p_direct_light);

			bc_light_instance _get_light_bounds(const bc_icamera& p_camera, const bc_light& p_light, const bc_point_light& p_point_light);

			bc_light_instance _get_light_bounds(const bc_icamera& p_camera, const bc_light& p_light, const bc_spot_light& p__spot_light);

			core::bc_concurrent_memory_pool m_pool;
			core::bc_list<bc_light, core::bc_memory_pool_allocator<bc_light>> m_lights;
		};
	}
}