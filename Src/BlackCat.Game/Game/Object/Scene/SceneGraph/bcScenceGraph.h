// [07/14/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/Shape/bcShapeBox.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/bcActor.hpp"
#include "bcSceneGraphNode.h"

namespace black_cat
{
	namespace game
	{
		class bc_input_system;
		class bc_render_system;
		class bc_render_thread;

		class BC_GAME_DLL bc_scene_graph
		{
		public:
			bc_scene_graph(core::bc_unique_ptr<bc_iscene_graph_node> p_scene_graph);

			bc_scene_graph(bc_scene_graph&&) = default;

			~bc_scene_graph();

			bc_scene_graph& operator=(bc_scene_graph&&) = default;

			bool add_actor(bc_actor& p_actor);

			bool update_actor(bc_actor& p_actor, const physics::bc_bound_box& p_previous_box);

			bool remove_actor(bc_actor& p_actor);

			core::bc_vector_frame< bc_actor > get_height_maps() const;

			void update(core_platform::bc_clock::update_param p_clock_update_param);

			void render_heightmaps(bc_render_system& p_render_system, bc_render_thread& p_render_thread);

			void render_meshes(bc_render_system& p_render_system, bc_render_thread& p_render_thread, bool p_preserve_render_instances);

			void render_debug_shapes(bc_shape_drawer& p_shape_drawer) const;

			void clear();

		protected:

		private:
			core::bc_unique_ptr<bc_iscene_graph_node> m_graph_node;
		};
	}
}