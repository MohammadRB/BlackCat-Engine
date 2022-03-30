// [07/14/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Memory/bcPtr.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphNode.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/bcExport.h"

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
			using value_type = bci_scene_graph_node::value_type;
			using pointer = bci_scene_graph_node::pointer;
			using reference = bci_scene_graph_node::reference;
			using difference_type = bci_scene_graph_node::difference_type;
			using iterator = bci_scene_graph_node::iterator;
			using const_iterator = bci_scene_graph_node::const_iterator;

		public:
			explicit bc_scene_graph(core::bc_unique_ptr<bci_scene_graph_node> p_scene_graph) noexcept;

			bc_scene_graph(bc_scene_graph&&) = default;

			~bc_scene_graph() noexcept;

			bc_scene_graph& operator=(bc_scene_graph&&) = default;

			physics::bc_bound_box get_bound_box() const noexcept;

			iterator begin() noexcept;

			const_iterator begin() const noexcept;

			const_iterator cbegin() const noexcept;

			iterator end() noexcept;

			const_iterator end() const noexcept;

			const_iterator cend() const noexcept;

			/**
			 * \brief Try to add actor to scene.
			 * \n ThreadSafe
			 * \param p_actor 
			 * \return True if the actor was added and False if actor was out of bound of scene graph
			 */
			bool add_actor(bc_actor& p_actor) noexcept;

			/**
			 * \brief Update actor in scene graph.
			 * \n ThreadSafe
			 * \param p_actor 
			 * \return True if the actor was updated and False if actor went out of bound of scene graph
			 */
			bool update_actor(bc_actor& p_actor) noexcept;

			/**
			 * \brief Try to remove actor from scene.
			 * \n ThreadSafe
			 * \param p_actor 
			 * \return True if the actor was removed and False if actor was not found
			 */
			bool remove_actor(bc_actor& p_actor) noexcept;

			bc_actor get_actor(const physics::bc_ray& p_ray, bc_scene_graph_ray_query_filter_callback* p_filter = nullptr) const noexcept;

			bc_scene_graph_buffer get_actors(const bc_camera_frustum& p_camera_frustum) const noexcept;

			template<typename TComponent>
			bc_scene_graph_buffer get_actors() const noexcept;

			template<typename TComponent>
			bc_scene_graph_buffer get_actors(const bc_camera_frustum& p_camera_frustum) const noexcept;

			void update(const platform::bc_clock::update_param& p_clock) noexcept;
			
			void draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const;

			void clear() noexcept;

		private:
			core::bc_unique_ptr<bci_scene_graph_node> m_graph_node;
		};

		template<typename TComponent>
		bc_scene_graph_buffer bc_scene_graph::get_actors() const noexcept
		{
			bc_scene_graph_buffer l_result;

			for (const bc_actor& l_actor : *m_graph_node)
			{
				if (l_actor.has_component<TComponent>())
				{
					l_result.add(l_actor);
				}
			}

			return l_result;
		}

		template<typename TComponent>
		bc_scene_graph_buffer bc_scene_graph::get_actors(const bc_camera_frustum& p_camera_frustum) const noexcept
		{
			bc_scene_graph_buffer l_in_view_actors = get_actors(p_camera_frustum);
			bc_scene_graph_buffer l_result;

			for (const bc_actor& l_actor : l_in_view_actors)
			{
				if (l_actor.has_component<TComponent>())
				{
					l_result.add(l_actor);
				}
			}

			return l_result;
		}
	}
}