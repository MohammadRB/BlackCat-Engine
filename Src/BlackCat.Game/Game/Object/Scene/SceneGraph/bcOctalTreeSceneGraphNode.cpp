// [11/29/2018 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcUtility.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/bcException.h"
#include "Game/Object/Scene/SceneGraph/bcOctalTreeSceneGraphNode.h"
#include "Game/Object/Scene/bcActor.hpp"
#include "Game/Object/Scene/Component/bcMediateComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_octal_tree_graph_node::bc_octal_tree_graph_node(physics::bc_bound_box p_box, 
			bcSIZE p_max_actors_count, 
			bcSIZE p_min_size)
			: m_max_actors_count(p_max_actors_count),
			m_min_size(p_min_size),
			m_actors_count(0),
			m_parent(nullptr),
			m_top_left_back(nullptr),
			m_top_left_front(nullptr),
			m_top_right_front(nullptr),
			m_top_right_back(nullptr),
			m_bottom_left_back(nullptr),
			m_bottom_left_front(nullptr),
			m_bottom_right_front(nullptr),
			m_bottom_right_back(nullptr),
			m_my_position(bc_octal_tree_node_position::null),
			m_bound_box(p_box),
			m_entry_pool(bcNew(core::bc_concurrent_memory_pool(), core::bc_alloc_type::unknown)),
			m_actors(graph_node_entry_allocator(*m_entry_pool))
		{
			const auto l_half_extends = p_box.get_half_extends();
			const bool l_is_power_of_two = bc_is_power_of_two(l_half_extends.x) &&
				bc_is_power_of_two(l_half_extends.y) &&
				bc_is_power_of_two(l_half_extends.z);
			if(!l_is_power_of_two)
			{
				throw bc_invalid_argument_exception("size of scene bound box must be power of two");
			}

			// TODO get size of list internal node
			m_entry_pool->initialize(1000, sizeof(_bc_octal_tree_graph_node_entry) + sizeof(void*) * 2, core::bc_alloc_type::unknown);
		}

		bc_octal_tree_graph_node::bc_octal_tree_graph_node(bc_octal_tree_graph_node& p_parent, 
			bc_octal_tree_node_position p_my_position, 
			bcSIZE p_max_actors_count,
			bcSIZE p_min_size)
			: m_max_actors_count(p_max_actors_count),
			m_min_size(p_min_size),
			m_actors_count(0),
			m_parent(&p_parent),
			m_top_left_back(nullptr),
			m_top_left_front(nullptr),
			m_top_right_front(nullptr),
			m_top_right_back(nullptr),
			m_bottom_left_back(nullptr),
			m_bottom_left_front(nullptr),
			m_bottom_right_front(nullptr),
			m_bottom_right_back(nullptr),
			m_my_position(p_my_position),
			m_bound_box(),
			m_entry_pool(p_parent.m_entry_pool),
			m_actors(graph_node_entry_allocator(*m_entry_pool))
		{
			core::bc_vector3f l_bound_box_center = p_parent.m_bound_box.get_center();
			const core::bc_vector3f l_half_extends = p_parent.m_bound_box.get_half_extends() / 2;
			const auto l_z_sign = graphic::bc_render_api_info::is_left_handed() ? +1 : -1;

			switch (p_my_position)
			{
			case bc_octal_tree_node_position::top_left_back:
				l_bound_box_center.x -= l_half_extends.x;
				l_bound_box_center.y += l_half_extends.y;
				l_bound_box_center.z += (l_half_extends.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::top_left_front:
				l_bound_box_center.x -= l_half_extends.x;
				l_bound_box_center.y += l_half_extends.y;
				l_bound_box_center.z -= (l_half_extends.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::top_right_front:
				l_bound_box_center.x += l_half_extends.x;
				l_bound_box_center.y += l_half_extends.y;
				l_bound_box_center.z -= (l_half_extends.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::top_right_back:
				l_bound_box_center.x += l_half_extends.x;
				l_bound_box_center.y += l_half_extends.y;
				l_bound_box_center.z += (l_half_extends.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::bottom_left_back:
				l_bound_box_center.x -= l_half_extends.x;
				l_bound_box_center.y -= l_half_extends.y;
				l_bound_box_center.z += (l_half_extends.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::bottom_left_front:
				l_bound_box_center.x -= l_half_extends.x;
				l_bound_box_center.y -= l_half_extends.y;
				l_bound_box_center.z -= (l_half_extends.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::bottom_right_front:
				l_bound_box_center.x += l_half_extends.x;
				l_bound_box_center.y -= l_half_extends.y;
				l_bound_box_center.z -= (l_half_extends.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::bottom_right_back:
				l_bound_box_center.x += l_half_extends.x;
				l_bound_box_center.y -= l_half_extends.y;
				l_bound_box_center.z += (l_half_extends.z * l_z_sign);
				break;
			default:
				bcAssert(false);
			}

			m_bound_box = physics::bc_bound_box(l_bound_box_center, l_half_extends);
		}

		bc_octal_tree_graph_node::~bc_octal_tree_graph_node()
		{
			if(!m_parent)
			{
				bcDelete(m_entry_pool);
			}
		}

		bc_iscene_graph_node::iterator bc_octal_tree_graph_node::begin() noexcept
		{
			bc_octal_tree_graph_node* l_min_node = _get_min_node();

			if (!l_min_node->m_actors.empty())
			{
				return iterator(this, &*l_min_node->m_actors.begin());
			}

			bc_octal_tree_graph_node* l_next_node = l_min_node->_get_next_sibling_node();
			while (l_next_node != nullptr && l_next_node->m_actors.empty())
			{
				l_next_node = l_next_node->_get_next_sibling_node();
			}

			if (l_next_node)
			{
				return iterator(l_next_node, &*l_next_node->m_actors.begin());
			}

			return iterator(this, nullptr);
		}

		bc_iscene_graph_node::const_iterator bc_octal_tree_graph_node::begin() const noexcept
		{
			return begin();
		}

		bc_iscene_graph_node::const_iterator bc_octal_tree_graph_node::cbegin() const noexcept
		{
			return begin();
		}

		bc_iscene_graph_node::iterator bc_octal_tree_graph_node::end() noexcept
		{
			return iterator(this, nullptr);
		}

		bc_iscene_graph_node::const_iterator bc_octal_tree_graph_node::end() const noexcept
		{
			return const_iterator(this, nullptr);
		}

		bc_iscene_graph_node::const_iterator bc_octal_tree_graph_node::cend() const noexcept
		{
			return const_iterator(this, nullptr);
		}

		physics::bc_bound_box bc_octal_tree_graph_node::get_bound_box() const noexcept
		{
			return m_bound_box;
		}

		bool bc_octal_tree_graph_node::contains_actor(bc_actor& p_actor) const noexcept
		{
			const physics::bc_bound_box& l_actor_mesh_bound_box = _get_actor_bound_box(p_actor);
			return m_bound_box.contains(l_actor_mesh_bound_box);
		}

		bool bc_octal_tree_graph_node::intersects_actor(bc_actor& p_actor) const noexcept
		{
			const physics::bc_bound_box& l_actor_mesh_bound_box = _get_actor_bound_box(p_actor);
			return m_bound_box.intersect(l_actor_mesh_bound_box);
		}

		bool bc_octal_tree_graph_node::is_leaf_node() const noexcept
		{
			return m_top_left_back == nullptr;
		}

		bool bc_octal_tree_graph_node::add_actor(bc_actor& p_actor)
		{
			bool l_added = false;

			if (!contains_actor(p_actor))
			{
				return l_added;
			}

			if (!is_leaf_node())
			{
				l_added = m_top_left_back->add_actor(p_actor);
				l_added = l_added || m_top_left_front->add_actor(p_actor);
				l_added = l_added || m_top_right_front->add_actor(p_actor);
				l_added = l_added || m_top_right_back->add_actor(p_actor);
				l_added = l_added || m_bottom_left_back->add_actor(p_actor);
				l_added = l_added || m_bottom_left_front->add_actor(p_actor);
				l_added = l_added || m_bottom_right_front->add_actor(p_actor);
				l_added = l_added || m_bottom_right_back->add_actor(p_actor);
			}

			if(!l_added)
			{
				m_actors.push_back(_bc_octal_tree_graph_node_entry(p_actor, this));
				l_added = true;

				auto l_iterator = m_actors.rbegin();
				l_iterator->set_internal_iterator(l_iterator.base());
			}

			if(l_added)
			{
				++m_actors_count;
				if (m_actors_count > m_max_actors_count && is_leaf_node())
				{
					_split();
				}
			}

			return l_added;
		}

		bool bc_octal_tree_graph_node::update_actor(bc_actor& p_actor, const physics::bc_bound_box& p_previous_box)
		{
			bool l_updated = false;
			bc_octal_tree_graph_node* l_prev_containing_node = _find_containing_node(p_previous_box);
			bc_octal_tree_graph_node* l_containing_node = _find_containing_node(_get_actor_bound_box(p_actor));

			if(!l_containing_node)
			{
				l_updated = false;
				return l_updated;
			}

			if(l_prev_containing_node == l_containing_node)
			{
				l_updated = true;
				return l_updated;
			}

			auto* l_actor_mediate_component = p_actor.get_component<bc_mediate_component>();
			auto l_actor_bound_box = l_actor_mediate_component->get_bound_box();

			l_actor_mediate_component->set_bound_box(p_previous_box);
			remove_actor(p_actor);

			l_actor_mediate_component->set_bound_box(l_actor_bound_box);
			l_updated = add_actor(p_actor);

			return l_updated;
		}

		bool bc_octal_tree_graph_node::remove_actor(bc_actor& p_actor)
		{
			bool l_removed = false;

			if (!contains_actor(p_actor))
			{
				return l_removed;
			}

			const auto l_exist = std::find(std::cbegin(m_actors), std::cend(m_actors), _bc_octal_tree_graph_node_entry(p_actor, this));
			if(l_exist != std::cend(m_actors))
			{
				m_actors.erase(l_exist);
				l_removed = true;
			}

			if(!l_removed && !is_leaf_node())
			{
				l_removed = m_top_left_back->remove_actor(p_actor);
				l_removed = l_removed || m_top_left_front->remove_actor(p_actor);
				l_removed = l_removed || m_top_right_front->remove_actor(p_actor);
				l_removed = l_removed || m_top_right_back->remove_actor(p_actor);
				l_removed = l_removed || m_bottom_left_back->remove_actor(p_actor);
				l_removed = l_removed || m_bottom_left_front->remove_actor(p_actor);
				l_removed = l_removed || m_bottom_right_front->remove_actor(p_actor);
				l_removed = l_removed || m_bottom_right_back->remove_actor(p_actor);
			}

			if(l_removed)
			{
				--m_actors_count;
				if (m_actors_count <= m_max_actors_count && !is_leaf_node())
				{
					_merge();
				}
			}

			return l_removed;
		}

		void bc_octal_tree_graph_node::clear()
		{
			if(!is_leaf_node())
			{
				m_top_left_back->clear();
				m_top_left_front->clear();
				m_top_right_front->clear();
				m_top_right_back->clear();
				m_bottom_left_back->clear();
				m_bottom_left_front->clear();
				m_bottom_right_front->clear();
				m_bottom_right_back->clear();

				m_top_left_back.reset();
				m_top_left_front.reset();
				m_top_right_front.reset();
				m_top_right_back.reset();
				m_bottom_left_back.reset();
				m_bottom_left_front.reset();
				m_bottom_right_front.reset();
				m_bottom_right_back.reset();
			}

			m_actors.clear();
			m_actors_count = 0;
		}

		void bc_octal_tree_graph_node::render_bound_boxes(bc_shape_drawer& p_shape_drawer) const
		{
			p_shape_drawer.draw_wired_box(m_bound_box);

			if(is_leaf_node())
			{
				return;
			}

			m_top_left_back->render_bound_boxes(p_shape_drawer);
			m_top_left_front->render_bound_boxes(p_shape_drawer);
			m_top_right_front->render_bound_boxes(p_shape_drawer);
			m_top_right_back->render_bound_boxes(p_shape_drawer);
			m_bottom_left_back->render_bound_boxes(p_shape_drawer);
			m_bottom_left_front->render_bound_boxes(p_shape_drawer);
			m_bottom_right_front->render_bound_boxes(p_shape_drawer);
			m_bottom_right_back->render_bound_boxes(p_shape_drawer);
		}

		bool bc_octal_tree_graph_node::iterator_validate(const node_type* p_node) const noexcept
		{
			return p_node;
		}

		bcINT32 bc_octal_tree_graph_node::iterator_compare(const node_type* p_first, const node_type* p_second) const noexcept
		{
			return p_first == p_second ? 0 : p_first > p_second ? 1 : -1;
		}

		bc_iscene_graph_node::value_type* bc_octal_tree_graph_node::iterator_dereference(node_type* p_node) const noexcept
		{
			return &p_node->m_actor;
		}

		bc_iscene_graph_node::node_type* bc_octal_tree_graph_node::iterator_increment(node_type* p_node) const noexcept
		{
			auto* l_octal_tree_node_entry = static_cast<_bc_octal_tree_graph_node_entry*>(p_node);
			auto l_next_internal_iterator = l_octal_tree_node_entry->m_internal_iterator;
			++l_next_internal_iterator;

			if(std::end(m_actors) != l_next_internal_iterator)
			{
				return &*l_next_internal_iterator;
			}

			bc_octal_tree_graph_node* l_sibling_node = l_octal_tree_node_entry->m_graph_node->_get_next_sibling_node();
			while (l_sibling_node != nullptr && l_sibling_node->m_actors.empty())
			{
				l_sibling_node = l_sibling_node->_get_next_sibling_node();
			}

			if (l_sibling_node == nullptr)
			{
				return nullptr;
			}

			return &*l_sibling_node->m_actors.begin();
		}

		bc_iscene_graph_node::node_type* bc_octal_tree_graph_node::iterator_decrement(node_type* p_node) const noexcept
		{
			auto* l_octal_tree_node_entry = static_cast<_bc_octal_tree_graph_node_entry*>(p_node);
			auto l_next_internal_iterator = l_octal_tree_node_entry->m_internal_iterator;
			--l_next_internal_iterator;

			if (std::end(m_actors) != l_next_internal_iterator)
			{
				return &*l_next_internal_iterator;
			}

			bc_octal_tree_graph_node* l_sibling_node = l_octal_tree_node_entry->m_graph_node->_get_prev_sibling_node();
			while (l_sibling_node != nullptr && l_sibling_node->m_actors.empty())
			{
				l_sibling_node = l_sibling_node->_get_prev_sibling_node();
			}

			if (l_sibling_node == nullptr)
			{
				return nullptr;
			}

			return &*l_sibling_node->m_actors.rbegin();
		}

		void bc_octal_tree_graph_node::iterator_swap(node_type** p_first, node_type** p_second) const noexcept
		{
			using std::swap;
			swap(*p_first, *p_second);
		}

		void bc_octal_tree_graph_node::_split()
		{
			m_top_left_back = core::bc_make_unique< bc_octal_tree_graph_node >
			(
				*this,
				bc_octal_tree_node_position::top_left_back,
				m_max_actors_count,
				m_min_size
			);
			m_top_left_front = core::bc_make_unique< bc_octal_tree_graph_node >
			(
				*this,
				bc_octal_tree_node_position::top_left_front,
				m_max_actors_count,
				m_min_size
			);
			m_top_right_front = core::bc_make_unique< bc_octal_tree_graph_node >
			(
				*this,
				bc_octal_tree_node_position::top_right_front,
				m_max_actors_count,
				m_min_size
			);
			m_top_right_back = core::bc_make_unique< bc_octal_tree_graph_node >
			(
				*this,
				bc_octal_tree_node_position::top_right_back,
				m_max_actors_count,
				m_min_size
			);
			m_bottom_left_back = core::bc_make_unique< bc_octal_tree_graph_node >
			(
				*this,
				bc_octal_tree_node_position::bottom_left_back,
				m_max_actors_count,
				m_min_size
			);
			m_bottom_left_front = core::bc_make_unique< bc_octal_tree_graph_node >
			(
				*this,
				bc_octal_tree_node_position::bottom_left_front,
				m_max_actors_count,
				m_min_size
			);
			m_bottom_right_front = core::bc_make_unique< bc_octal_tree_graph_node >
			(
				*this,
				bc_octal_tree_node_position::bottom_right_front,
				m_max_actors_count,
				m_min_size
			);
			m_bottom_right_back = core::bc_make_unique< bc_octal_tree_graph_node >
			(
				*this,
				bc_octal_tree_node_position::bottom_right_back,
				m_max_actors_count,
				m_min_size
			);

 			graph_node_entry_list l_actors{ graph_node_entry_allocator(*m_entry_pool) };
			m_actors.swap(l_actors);
			m_actors_count = 0;

			for(bc_iscene_graph_node_entry& l_entry : l_actors)
			{
				add_actor(l_entry.m_actor);
			}
		}

		void bc_octal_tree_graph_node::_merge()
		{
			core::bc_array<bc_octal_tree_graph_node*, 8> l_children
			{
				&*m_top_left_back,
				&*m_top_left_front,
				&*m_top_right_front,
				&*m_top_right_back,
				&*m_bottom_left_back,
				&*m_bottom_left_front,
				&*m_bottom_right_front,
				&*m_bottom_right_back
			};

			for(bc_octal_tree_graph_node* l_child : l_children)
			{
				auto l_child_actor_count = l_child->m_actors.size();
				if(l_child_actor_count == 0)
				{
					continue;
				}

				m_actors.splice(std::rbegin(m_actors).base(), l_child->m_actors);

				auto l_last_entry = std::rbegin(m_actors);
				while(l_child_actor_count-- > 0)
				{
					l_last_entry->m_graph_node = this;
					l_last_entry->m_internal_iterator = l_last_entry.base();
					++l_last_entry;
				}
			}

			m_top_left_back.reset();
			m_top_left_front.reset();
			m_top_right_front.reset();
			m_top_right_back.reset();
			m_bottom_left_back.reset();
			m_bottom_left_front.reset();
			m_bottom_right_front.reset();
			m_bottom_right_back.reset();
		}

		bc_octal_tree_graph_node* bc_octal_tree_graph_node::_get_min_node() const
		{
			bc_octal_tree_graph_node* l_node = const_cast<bc_octal_tree_graph_node*>(this);
			while(l_node->m_top_left_back)
			{
				l_node = l_node->m_top_left_back.get();
			}

			return l_node;
		}

		bc_octal_tree_graph_node* bc_octal_tree_graph_node::_get_max_node() const
		{
			bc_octal_tree_graph_node* l_node = const_cast<bc_octal_tree_graph_node*>(this);
			while (l_node->m_bottom_right_back)
			{
				l_node = l_node->m_bottom_right_back.get();
			}

			return l_node;
		}

		bc_octal_tree_graph_node* bc_octal_tree_graph_node::_find_containing_node(const physics::bc_bound_box& p_box) const
		{
			bc_octal_tree_graph_node* l_node = nullptr;

			if (!m_bound_box.contains(p_box))
			{
				return l_node;
			}

			if (!is_leaf_node())
			{
				l_node = m_top_left_back->_find_containing_node(p_box);
				l_node = l_node != nullptr ? l_node : m_top_left_front->_find_containing_node(p_box);
				l_node = l_node != nullptr ? l_node : m_top_right_front->_find_containing_node(p_box);
				l_node = l_node != nullptr ? l_node : m_top_right_back->_find_containing_node(p_box);
				l_node = l_node != nullptr ? l_node : m_bottom_left_back->_find_containing_node(p_box);
				l_node = l_node != nullptr ? l_node : m_bottom_left_front->_find_containing_node(p_box);
				l_node = l_node != nullptr ? l_node : m_bottom_right_front->_find_containing_node(p_box);
				l_node = l_node != nullptr ? l_node : m_bottom_right_back->_find_containing_node(p_box);
			}

			if (!l_node)
			{
				l_node = const_cast<bc_octal_tree_graph_node*>(this);
			}

			return l_node;
		}

		bc_octal_tree_graph_node* bc_octal_tree_graph_node::_get_next_sibling_node() const
		{
			if(m_parent == nullptr)
			{
				return nullptr;
			}

			if(this == m_parent->m_top_left_back.get())
			{
				return m_parent->m_top_left_front->_get_min_node();
			}
			if (this == m_parent->m_top_left_front.get())
			{
				return m_parent->m_top_right_front->_get_min_node();
			}
			if (this == m_parent->m_top_right_front.get())
			{
				return m_parent->m_top_right_back->_get_min_node();
			}
			if (this == m_parent->m_top_right_back.get())
			{
				return m_parent->m_bottom_left_back->_get_min_node();
			}
			if (this == m_parent->m_bottom_left_back.get())
			{
				return m_parent->m_bottom_left_front->_get_min_node();
			}
			if (this == m_parent->m_bottom_left_front.get())
			{
				return m_parent->m_bottom_right_front->_get_min_node();
			}
			if (this == m_parent->m_bottom_right_front.get())
			{
				return m_parent->m_bottom_right_back->_get_min_node();
			}
			if (this == m_parent->m_bottom_right_back.get())
			{
				return m_parent;
			}

			bcAssert(false);
			return nullptr;
		}

		bc_octal_tree_graph_node* bc_octal_tree_graph_node::_get_prev_sibling_node() const
		{
			if (m_parent == nullptr)
			{
				return nullptr;
			}

			if (this == m_parent->m_bottom_right_back.get())
			{
				return m_parent->m_bottom_right_front->_get_max_node();
			}
			if (this == m_parent->m_bottom_right_front.get())
			{
				return m_parent->m_bottom_left_front->_get_max_node();
			}
			if (this == m_parent->m_bottom_left_front.get())
			{
				return m_parent->m_bottom_left_back->_get_max_node();
			}
			if (this == m_parent->m_bottom_left_back.get())
			{
				return m_parent->m_top_right_back->_get_max_node();
			}
			if (this == m_parent->m_top_right_back.get())
			{
				return m_parent->m_top_right_front->_get_max_node();
			}
			if (this == m_parent->m_top_right_front.get())
			{
				return m_parent->m_top_left_front->_get_max_node();
			}
			if (this == m_parent->m_top_left_front.get())
			{
				return m_parent->m_top_left_back->_get_max_node();
			}
			if (this == m_parent->m_top_left_back.get())
			{
				return m_parent;
			}

			bcAssert(false);
			return nullptr;
		}

		const physics::bc_bound_box& bc_octal_tree_graph_node::_get_actor_bound_box(bc_actor& p_actor) const
		{
			auto* l_mediate_component = p_actor.get_component<bc_mediate_component>();
			const auto& l_bound_box = l_mediate_component->get_bound_box();

			bcAssert(!l_bound_box.is_empty());

			return l_bound_box;
		}
	}
}