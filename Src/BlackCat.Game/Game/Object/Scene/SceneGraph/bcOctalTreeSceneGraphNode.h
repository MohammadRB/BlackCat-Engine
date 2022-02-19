// [12/19/2018 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Container/bcList.h"
#include "Core/Utility/bcObjectPoolAllocator.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphNode.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_octal_tree_graph_node;

		class _bc_octal_tree_graph_node_entry : public bci_scene_graph_node_entry
		{
		public:
			using graph_node_entry_allocator = core::bc_memory_pool_allocator<_bc_octal_tree_graph_node_entry>;
			using graph_node_entry_list = core::bc_list<_bc_octal_tree_graph_node_entry, graph_node_entry_allocator>;
			using internal_iterator = graph_node_entry_list::iterator;

		public:
			explicit _bc_octal_tree_graph_node_entry(const bc_actor& p_actor, const bc_octal_tree_graph_node* p_graph_node)
				: bci_scene_graph_node_entry(p_actor),
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

		class BC_GAME_DLL bc_octal_tree_graph_node : public bci_scene_graph_node
		{
		public:
			using graph_node_entry_allocator = _bc_octal_tree_graph_node_entry::graph_node_entry_allocator;
			using graph_node_entry_list = _bc_octal_tree_graph_node_entry::graph_node_entry_list;

		public:
			bc_octal_tree_graph_node(const physics::bc_bound_box& p_box, bcSIZE p_max_actors_count, bcSIZE p_min_size);

			bc_octal_tree_graph_node(bc_octal_tree_graph_node& p_parent, bc_octal_tree_node_position p_my_position);

			bc_octal_tree_graph_node(bc_octal_tree_graph_node&&) noexcept = default;

			~bc_octal_tree_graph_node() override;

			bc_octal_tree_graph_node& operator=(bc_octal_tree_graph_node&&) noexcept = default;

			iterator begin() noexcept override;

			const_iterator begin() const noexcept override;

			const_iterator cbegin() const noexcept override;

			iterator end() noexcept override;

			const_iterator end() const noexcept override;

			const_iterator cend() const noexcept override;

			physics::bc_bound_box get_bound_box() const noexcept override;

			bool contains_actor(bc_actor& p_actor) const noexcept override;

			bool intersects_actor(bc_actor& p_actor) const noexcept override;

			void get_actor(const physics::bc_ray& p_ray, std::pair<bcFLOAT, bc_actor>& p_result) const noexcept override;

			void get_actors(const bc_camera_frustum& p_camera_frustum, bc_scene_graph_buffer& p_buffer) const noexcept override;

			bool is_leaf_node() const noexcept;

			bool add_actor(bc_actor& p_actor) noexcept override;

			bool update_actor(bc_actor& p_actor) noexcept override;

			bool remove_actor(bc_actor& p_actor) noexcept override;

			void update(const core_platform::bc_clock::update_param& p_clock) noexcept override;
			
			void clear() noexcept override;

			void draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const override;

		protected:
			bool iterator_validate(const node_type* p_node) const noexcept override;

			bcINT32 iterator_compare(const node_type* p_first, const node_type* p_second) const noexcept override;

			value_type* iterator_dereference(node_type* p_node) const noexcept override;

			node_type* iterator_increment(node_type* p_node) const noexcept override;

			node_type* iterator_decrement(node_type* p_node) const noexcept override;

			void iterator_swap(node_type** p_first, node_type** p_second) const noexcept override;

		private:
			bool _add_actor(bc_actor& p_actor, const physics::bc_bound_box& p_actor_bound_box);

			bool _remove_actor(bc_actor& p_actor, const physics::bc_bound_box& p_actor_bound_box);

			void _reform_graph();
			
			void _split();

			void _merge();

			bc_octal_tree_graph_node* _get_min_node() const;

			bc_octal_tree_graph_node* _get_max_node() const;

			bc_octal_tree_graph_node* _find_containing_node(const physics::bc_bound_box& p_box) const;

			bc_octal_tree_graph_node* _get_next_sibling_node() const;

			bc_octal_tree_graph_node* _get_prev_sibling_node() const;

			static const physics::bc_bound_box& _get_actor_bound_box(bc_actor& p_actor);

			bcSIZE m_max_actors_count;
			bcSIZE m_min_size;
			bcFLOAT m_update_interval_seconds;
			core_platform::bc_atomic<bcSIZE> m_actors_count;

			bc_octal_tree_graph_node* m_parent;
			bc_octal_tree_graph_node* m_top_left_back;
			bc_octal_tree_graph_node* m_top_left_front;
			bc_octal_tree_graph_node* m_top_right_front;
			bc_octal_tree_graph_node* m_top_right_back;
			bc_octal_tree_graph_node* m_bottom_left_back;
			bc_octal_tree_graph_node* m_bottom_left_front;
			bc_octal_tree_graph_node* m_bottom_right_front;
			bc_octal_tree_graph_node* m_bottom_right_back;

			bc_octal_tree_node_position m_my_position;
			physics::bc_bound_box m_bound_box;
			core::bc_concurrent_object_pool<bc_octal_tree_graph_node>* m_child_nodes_pool;
			core::bc_concurrent_memory_pool* m_actors_pool;
			graph_node_entry_list m_actors;
			mutable core_platform::bc_mutex m_actors_lock;
		};
	}
}