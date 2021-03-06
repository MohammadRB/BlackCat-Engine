// [04/03/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcListPool.h"
#include "Core/Concurrency/bcTask.h"
#include "Core/Utility/bcNullable.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "Game/Object/Scene/Bullet/bcBullet.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;
		class bc_physics_system;
		
		class BC_GAME_DLL bc_bullet_manager
		{
		public:
			explicit bc_bullet_manager(bc_physics_system& p_physics_system);

			bc_bullet_manager(bc_bullet_manager&&) noexcept;

			~bc_bullet_manager();

			bc_bullet_manager& operator=(bc_bullet_manager&&) noexcept;

			/**
			 * \brief ThreadSafe
			 * \param p_bullet 
			 */
			void add_bullet(const bc_bullet& p_bullet);

			void update(bc_scene& p_scene, const core_platform::bc_clock::update_param& p_clock);
			
			core::bc_task<void> update_async(bc_scene& p_scene, const core_platform::bc_clock::update_param& p_clock);
			
		private:
			core::bc_nullable<physics::bc_ray_hit> _scene_ray_query(const physics::bc_scene& p_px_scene, 
				const physics::bc_ray& p_ray, 
				physics::bc_scene_ray_query_buffer& p_buffer,
				core::bc_vector_frame<physics::bc_ray_hit>& p_modified_hits_buffer);
			
			bc_physics_system* m_physics_system;			
			core_platform::bc_spin_mutex m_bullets_mutex;
			core::bc_list_pool<bc_bullet> m_bullets;
		};
	}	
}