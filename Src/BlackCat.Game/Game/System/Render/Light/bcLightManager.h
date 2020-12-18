// [02/23/2019 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Concurrency/bcMutexTest.h"
#include "Core/Utility/bcObjectPool.h"
#include "Core/Utility/bcObjectPoolAllocator.h"
#include "Core/Container/bcListPool.h"
#include "Core/Messaging/Query/bcQueryContext.h"
#include "Core/Messaging/Query/bcQueryProviderHandle.h"
#include "Game/bcExport.h"
#include "Game/System/Render/Light/bcLight.h"
#include "Game/System/Render/Light/bcLightIteratorBuffer.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_light_manager
		{
		private:
			using container_t = core::bc_list_pool< bc_light >;

		public:
			using iterator_buffer = bc_light_iterator_buffer< container_t >;
			
		public:
			bc_light_manager();

			bc_light_manager(bc_light_manager&& p_other) noexcept;

			~bc_light_manager();

			bc_light_manager& operator=(bc_light_manager&& p_other) noexcept;

			bc_light_ptr add_light(const bc_direct_light& p_light);

			bc_light_ptr add_light(const bc_point_light& p_light);

			bc_light_ptr add_light(const bc_spot_light& p_light);

			iterator_buffer get_iterator() const noexcept;

			void update(const core_platform::bc_clock::update_param& p_clock);

			void destroy_light(bc_light* p_light);

		private:
			core::bc_query_context_ptr _lights_query_context_provider() const;
			
			container_t m_lights;
			mutable core::bc_mutex_test m_lights_lock;

			core::bc_query_provider_handle m_lights_query_handle;
		};
	}
}