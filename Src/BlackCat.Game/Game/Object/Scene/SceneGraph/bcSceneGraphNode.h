// [11/29/2018 MRB]

#pragma once

#include "Core/Container/bcIterator.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Scene/bcActor.h"
#include "Game/System/Render/bcShapeDrawer.h"

namespace black_cat
{
	namespace game
	{
		class bc_iscene_graph_node_entry
		{
		public:
			explicit bc_iscene_graph_node_entry(const bc_actor& p_actor)
				: m_actor(p_actor)
			{
			}

			bc_actor m_actor;
		};

		class bc_iscene_graph_node
		{
			bc_make_iterators_friend(bc_iscene_graph_node)

		public:
			using this_type = bc_iscene_graph_node;
			using value_type = bc_actor;
			using node_type = bc_iscene_graph_node_entry;
			using pointer = value_type*;
			using reference = value_type&;
			using difference_type = bcSIZE;
			using iterator = core::bc_bidirectional_iterator<this_type>;
			using const_iterator = core::bc_const_bidirectional_iterator<this_type>;

		public:
			virtual ~bc_iscene_graph_node() = default;

			virtual iterator begin() noexcept = 0;

			virtual const_iterator begin() const noexcept = 0;

			virtual const_iterator cbegin() const noexcept = 0;

			virtual iterator end() noexcept = 0;

			virtual const_iterator end() const noexcept = 0;

			virtual const_iterator cend() const noexcept = 0;

			virtual physics::bc_bound_box get_bound_box() const noexcept = 0;

			virtual bool contains_actor(bc_actor& p_actor) const noexcept = 0;

			virtual bool intersects_actor(bc_actor& p_actor) const noexcept = 0;

			virtual bool add_actor(bc_actor& p_actor) = 0;

			virtual bool update_actor(bc_actor& p_actor, const physics::bc_bound_box& p_previous_box) = 0;

			virtual bool remove_actor(bc_actor& p_actor) = 0;

			virtual void clear() = 0;

			virtual void render_bound_boxes(bc_shape_drawer& p_shape_drawer) const = 0;

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