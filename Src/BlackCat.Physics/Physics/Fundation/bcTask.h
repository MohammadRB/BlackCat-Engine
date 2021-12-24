// [11/29/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Physics/bcPhysicsApi.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		struct bc_platform_task_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_task
		{
		public:
			using platform_pack = bc_platform_task_pack<TApi>;

		public:
			explicit bc_platform_task(const platform_pack& p_pack);

			bc_platform_task(bc_platform_task&&) noexcept;

			~bc_platform_task();

			bc_platform_task& operator=(bc_platform_task&&) noexcept;

			void run();

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_task = bc_platform_task<g_current_physics_api>;
	}
}