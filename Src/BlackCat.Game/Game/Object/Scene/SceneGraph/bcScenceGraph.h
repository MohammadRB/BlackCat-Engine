// [07/14/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Memory/bcPtr.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/bcActor.hpp"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphNode.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"

namespace black_cat
{
	namespace game
	{
		class bc_input_system;
		class bc_render_system;
		class bc_render_thread;
		class bc_camera_frustum;

		class BC_GAME_DLL bc_scene_graph
		{
		public:
			using value_type = bc_iscene_graph_node::value_type;
			using pointer = bc_iscene_graph_node::pointer;
			using reference = bc_iscene_graph_node::reference;
			using difference_type = bc_iscene_graph_node::difference_type;
			using iterator = bc_iscene_graph_node::iterator;
			using const_iterator = bc_iscene_graph_node::const_iterator;

		public:
			bc_scene_graph(core::bc_unique_ptr<bc_iscene_graph_node> p_scene_graph);

			bc_scene_graph(bc_scene_graph&&) = default;

			~bc_scene_graph();

			bc_scene_graph& operator=(bc_scene_graph&&) = default;

			iterator begin() noexcept;

			const_iterator begin() const noexcept;

			const_iterator cbegin() const noexcept;

			iterator end() noexcept;

			const_iterator end() const noexcept;

			const_iterator cend() const noexcept;

			bool add_actor(bc_actor& p_actor);

			bool update_actor(bc_actor& p_actor, const physics::bc_bound_box& p_previous_box);

			bool remove_actor(bc_actor& p_actor);

			template< typename TComponent >
			bc_scene_graph_buffer get_actors() const;

			bc_scene_graph_buffer get_actors(const bc_camera_frustum& p_camera_frustum) const;

			void update(core_platform::bc_clock::update_param p_clock_update_param);

			void render_debug_shapes(bc_shape_drawer& p_shape_drawer) const;

			void clear();

		protected:

		private:
			core::bc_unique_ptr<bc_iscene_graph_node> m_graph_node;
		};

		template< typename TComponent >
		bc_scene_graph_buffer bc_scene_graph::get_actors() const
		{
			bc_scene_graph_buffer l_result;

			for (const bc_actor& l_actor : *m_graph_node)
			{
				if (l_actor.has_component<TComponent>())
				{
					l_result.add_actor(l_actor);
				}
			}

			return l_result;
		}
	}
}