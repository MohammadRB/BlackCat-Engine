// [23/02/2019 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcListPool.h"
#include "Game/bcExport.h"
#include "Game/System/Render/Light/bcLight.h"
#include "Game/System/Render/Light/bcLightIteratorBuffer.h"

namespace black_cat::game
{
	class BC_GAME_DLL bc_light_manager
	{
	private:
		using container_t = core::bc_list_pool<bc_light>;

	public:
		using iterator_buffer = bc_light_iterator_buffer<container_t>;
			
	public:
		bc_light_manager();

		bc_light_manager(bc_light_manager&& p_other) noexcept;

		~bc_light_manager();

		bc_light_manager& operator=(bc_light_manager&& p_other) noexcept;

		bc_light_ptr add_light(const bc_direct_light& p_light);

		bc_light_ptr add_light(const bc_point_light& p_light);

		bc_light_ptr add_light(const bc_spot_light& p_light);

		void update(const platform::bc_clock::update_param& p_clock);

		iterator_buffer get_iterator_buffer() const noexcept;
			
		void destroy_light(bc_light* p_light);

	private:
		mutable platform::bc_shared_mutex m_lights_lock;
		bc_light::id_t m_id_counter;
		container_t m_lights;
	};
}
