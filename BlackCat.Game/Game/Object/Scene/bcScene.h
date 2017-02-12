// [01/12/2017 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/bcScenceGraph.h"
#include "Game/System/Physics/bcPhysicsSystem.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_scene
		{
		public:
			bc_scene(bc_scene_graph&& p_scene_graph, physics::bc_scene&& p_px_scene);

			bc_scene(bc_scene&&) noexcept;

			~bc_scene();

			bc_scene& operator=(bc_scene&&) noexcept;

			core::bc_vector_frame< bc_actor > get_heightmaps() const;

			void add_object(bc_actor p_actor);

			bc_actor remove_object(bc_actor p_actor);

			void render_heightmaps(bc_render_system& p_render_system, bc_render_thread& p_render_thread);

			void render_meshes(bc_render_system& p_render_system, bc_render_thread& p_render_thread, bool p_preserve_render_instances);

			void update(bc_physics_system& p_physics, core_platform::bc_clock::update_param p_time);

		protected:

		private:
			bc_scene_graph m_scene_graph;
			physics::bc_scene m_px_scene;
		};
	}
}
