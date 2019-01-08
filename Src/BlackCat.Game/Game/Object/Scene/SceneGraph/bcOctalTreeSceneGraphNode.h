// [12/19/2018 MRB]

#pragma once

#include "Core/Container/bcList.h"
#include "Core/Utility/bcObjectPoolAllocator.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphNode.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_octal_tree_graph_node;

		class _bc_octal_tree_graph_node_entry : public bc_iscene_graph_node_entry
		{
		public:
			using graph_node_entry_allocator = core::bc_memory_pool_allocator<_bc_octal_tree_graph_node_entry>;
			using graph_node_entry_list = core::bc_list<_bc_octal_tree_graph_node_entry, graph_node_entry_allocator>;
			using internal_iterator = graph_node_entry_list::iterator;

		public:
			explicit _bc_octal_tree_graph_node_entry(const bc_actor& p_actor, const bc_octal_tree_graph_node* p_graph_node)
				: bc_iscene_graph_node_entry(p_actor),
				m_graph_node(p_graph_node),
				m_internal_iterator(nullptr, nullptr)
			{
			}

			void set_internal_iterator(internal_iterator p_iterator)
			{
				m_internal_iterator = p_iterator;
			}

			bool operator==(const _bc_octal_tree_graph_node_entry& p_other) const noexcept
			{
				return m_actor == p_other.m_actor;
			}

			bool operator!=(const _bc_octal_tree_graph_node_entry& p_other) const noexcept
			{
				return m_actor != p_other.m_actor;
			}

			const bc_octal_tree_graph_node* m_graph_node;
			internal_iterator m_internal_iterator;
		};

		enum class bc_octal_tree_node_position
		{
			null,
			top_left_back,
			top_left_front,
			top_right_front,
			top_right_back,
			bottom_left_back,
			bottom_left_front,
			bottom_right_front,
			bottom_right_back
		};

		class BC_GAME_DLL bc_octal_tree_graph_node : public bc_iscene_graph_node
		{
		public:
			using graph_node_entry_allocator = _bc_octal_tree_graph_node_entry::graph_node_entry_allocator;
			using graph_node_entry_list = _bc_octal_tree_graph_node_entry::graph_node_entry_list;

		public:
			bc_octal_tree_graph_node(physics::bc_bound_box p_box, bcSIZE p_max_actors_count, bcSIZE p_min_size);

			bc_octal_tree_graph_node(bc_octal_tree_graph_node& p_parent,
				bc_octal_tree_node_position p_my_position,
				bcSIZE p_max_actors_count,
				bcSIZE p_min_size);

			bc_octal_tree_graph_node(bc_octal_tree_graph_node&&) = default;

			~bc_octal_tree_graph_node();

			bc_octal_tree_graph_node& operator=(bc_octal_tree_graph_node&&) = default;

			iterator begin() noexcept override;

			const_iterator begin() const noexcept override;

			const_iterator cbegin() const noexcept override;

			iterator end() noexcept override;

			const_iterator end() const noexcept override;

			const_iterator cend() const noexcept override;

			physics::bc_bound_box get_bound_box() const noexcept override;

			bool contains_actor(bc_actor& p_actor) const noexcept override;

			bool intersects_actor(bc_actor& p_actor) const noexcept override;

			bool is_leaf_node() const noexcept;

			bool add_actor(bc_actor& p_actor) override;

			bool update_actor(bc_actor& p_actor, const physics::bc_bound_box& p_previous_box) override;

			bool remove_actor(bc_actor& p_actor) override;

			void clear() override;

			void render_bound_boxes(bc_shape_drawer& p_shape_drawer) const override;

		protected:
			bool iterator_validate(const node_type* p_node) const noexcept override;

			bcINT32 iterator_compare(const node_type* p_first, const node_type* p_second) const noexcept override;

			value_type* iterator_dereference(node_type* p_node) const noexcept override;

			node_type* iterator_increment(node_type* p_node) const noexcept override;

			node_type* iterator_decrement(node_type* p_node) const noexcept override;

			void iterator_swap(node_type** p_first, node_type** p_second) const noexcept override;

		private:
			void _split();

			void _merge();

			bc_octal_tree_graph_node* _get_min_node() const;

			bc_octal_tree_graph_node* _get_max_node() const;

			bc_octal_tree_graph_node* _find_containing_node(const physics::bc_bound_box& p_box) const;

			bc_octal_tree_graph_node* _get_next_sibling_node() const;

			bc_octal_tree_graph_node* _get_prev_sibling_node() const;

			const physics::bc_bound_box& _get_actor_bound_box(bc_actor& p_actor) const;

			bcSIZE m_max_actors_count;
			bcSIZE m_min_size;
			bcSIZE m_actors_count;

			bc_octal_tree_graph_node* m_parent;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_top_left_back;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_top_left_front;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_top_right_front;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_top_right_back;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_bottom_left_back;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_bottom_left_front;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_bottom_right_front;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_bottom_right_back;

			bc_octal_tree_node_position m_my_position;
			physics::bc_bound_box m_bound_box;
			core::bc_concurrent_memory_pool* m_entry_pool;
			graph_node_entry_list m_actors;
		};
	}
}