// [11/29/2018 MRB]

#pragma once

#include "Core/Container/bcIterator.h"
#include "Core/Utility/bcDelegate.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/System/Render/bcShapeDrawer.h"
#include "Game/System/Input/bcCameraFrustum.h"

namespace black_cat
{
	namespace game
	{
		struct bc_scene_graph_ray_query_hit
		{
			bc_actor m_actor;
			physics::bc_ray_hit m_hit;
		};

		using bc_scene_graph_ray_query_filter_callback = core::bc_delegate<bool(const bc_scene_graph_ray_query_hit&)>;

		class bci_scene_graph_node_entry
		{
		public:
			explicit bci_scene_graph_node_entry(const bc_actor& p_actor)
				: m_actor(p_actor)
			{
			}

			bc_actor m_actor;
		};

		class bci_scene_graph_node
		{
			bc_make_iterators_friend(bci_scene_graph_node)

		public:
			using this_type = bci_scene_graph_node;
			using value_type = bc_actor;
			using node_type = bci_scene_graph_node_entry;
			using pointer = value_type*;
			using reference = value_type&;
			using difference_type = bcSIZE;
			using iterator = core::bc_bidirectional_iterator<this_type>;
			using const_iterator = core::bc_const_bidirectional_iterator<this_type>;

		public:
			virtual ~bci_scene_graph_node() = default;

			virtual iterator begin() noexcept = 0;

			virtual const_iterator begin() const noexcept = 0;

			virtual const_iterator cbegin() const noexcept = 0;

			virtual iterator end() noexcept = 0;

			virtual const_iterator end() const noexcept = 0;

			virtual const_iterator cend() const noexcept = 0;

			virtual physics::bc_bound_box get_bound_box() const noexcept = 0;

			virtual bool contains_actor(bc_actor& p_actor) const noexcept = 0;

			virtual bool intersects_actor(bc_actor& p_actor) const noexcept = 0;

			virtual void get_actor(const physics::bc_ray& p_ray, bc_scene_graph_ray_query_hit& p_result, bc_scene_graph_ray_query_filter_callback* p_filter = nullptr) const noexcept = 0;

			virtual void get_actors(const bc_camera_frustum& p_camera_frustum, bc_scene_graph_buffer& p_buffer) const noexcept = 0;

			/**
			 * \brief ThreadSafe
			 * \param p_actor 
			 * \return 
			 */
			virtual bool add_actor(bc_actor& p_actor) noexcept = 0;

			/**
			 * \brief ThreadSafe
			 * \param p_actor 
			 * \return 
			 */
			virtual bool update_actor(bc_actor& p_actor) noexcept = 0;

			/**
			 * \brief ThreadSafe
			 * \param p_actor 
			 * \return 
			 */
			virtual bool remove_actor(bc_actor& p_actor) noexcept = 0;

			virtual void update(const platform::bc_clock::update_param& p_clock) noexcept = 0;
			
			virtual void clear() noexcept = 0;

			virtual void draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const = 0;

		protected:
			virtual bool iterator_validate(const node_type* p_node) const noexcept = 0;

			virtual bcINT32 iterator_compare(const node_type* p_first, const node_type* p_second) const noexcept = 0;

			virtual value_type* iterator_dereference(node_type* p_node) const noexcept = 0;

			virtual node_type* iterator_increment(node_type* p_node) const noexcept = 0;

			virtual node_type* iterator_decrement(node_type* p_node) const noexcept = 0;

			virtual void iterator_swap(node_type** p_first, node_type** p_second) const noexcept = 0;
		};
	}
}