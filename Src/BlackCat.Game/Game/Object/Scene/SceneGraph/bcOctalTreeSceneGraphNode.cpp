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
			m_bound_box(p_box),
			m_actors()
		{
			const auto l_half_extends = p_box.get_half_extends();
			const bool l_is_power_of_two = bc_is_power_of_two(l_half_extends.x) &&
				bc_is_power_of_two(l_half_extends.y) &&
				bc_is_power_of_two(l_half_extends.z);
			if(!l_is_power_of_two)
			{
				throw bc_invalid_argument_exception("size of scene bound box must be power of two");
			}
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
			m_bound_box(),
			m_actors()
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
			}

			m_bound_box = physics::bc_bound_box(l_bound_box_center, l_half_extends);
		}

		bc_octal_tree_graph_node::~bc_octal_tree_graph_node()
		{
		}

		bc_iscene_graph_node::iterator bc_octal_tree_graph_node::begin() noexcept
		{
			return iterator(this, &*m_actors.begin());
		}

		bc_iscene_graph_node::const_iterator bc_octal_tree_graph_node::begin() const noexcept
		{
			return const_iterator(this, &*m_actors.begin());
		}

		bc_iscene_graph_node::const_iterator bc_octal_tree_graph_node::cbegin() const noexcept
		{
			return const_iterator(this, &*m_actors.begin());
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

		void bc_octal_tree_graph_node::add_actor(bc_actor& p_actor)
		{
			if(!contains_actor(p_actor))
			{
				return;
			}

			m_actors_count++;

			if(!is_leaf_node())
			{
				m_top_left_back->add_actor(p_actor);
				m_top_left_front->add_actor(p_actor);
				m_top_right_front->add_actor(p_actor);
				m_top_right_back->add_actor(p_actor);
				m_bottom_left_back->add_actor(p_actor);
				m_bottom_left_front->add_actor(p_actor);
				m_bottom_right_front->add_actor(p_actor);
				m_bottom_right_back->add_actor(p_actor);
			}
			else
			{
				m_actors.push_back(bc_octal_tree_graph_node_entry(p_actor));

				auto l_iterator = m_actors.rbegin();
				l_iterator->set_internal_iterator(l_iterator.base());

				if (m_actors_count == m_max_actors_count)
				{
					_split();
				}
			}
		}

		void bc_octal_tree_graph_node::update_actor(bc_actor& p_actor, const physics::bc_bound_box& p_previous_box)
		{
			core::bc_vector_frame<bc_octal_tree_graph_node*> l_intersecting_nodes;
			_find_intersecting_nodes(p_previous_box, l_intersecting_nodes);

			for(bc_octal_tree_graph_node* l_node : l_intersecting_nodes)
			{
				const physics::bc_bound_box& l_actor_bound_box = _get_actor_bound_box(p_actor);
				auto& l_containing_node = l_node->_find_containing_node(l_actor_bound_box);

				if(&l_containing_node != l_node)
				{
					l_containing_node.remove_actor(p_actor);
					l_containing_node.add_actor(p_actor);
				}
			}
 		}

		void bc_octal_tree_graph_node::remove_actor(bc_actor& p_actor)
		{
			if(!contains_actor(p_actor))
			{
				return;
			}

			--m_actors_count;

			if(!is_leaf_node())
			{
				m_top_left_back->remove_actor(p_actor);
				m_top_left_front->remove_actor(p_actor);
				m_top_right_front->remove_actor(p_actor);
				m_top_right_back->remove_actor(p_actor);
				m_bottom_left_back->remove_actor(p_actor);
				m_bottom_left_front->remove_actor(p_actor);
				m_bottom_right_front->remove_actor(p_actor);
				m_bottom_right_back->remove_actor(p_actor);

				if (m_actors_count <= m_max_actors_count)
				{
					_merge();
				}
			}
			else
			{
				m_actors.remove(bc_octal_tree_graph_node_entry(p_actor));
			}
		}

		bool bc_octal_tree_graph_node::contains_actor(bc_actor& p_actor) const noexcept
		{
			const physics::bc_bound_box& l_actor_mesh_bound_box = _get_actor_bound_box(p_actor);
			return m_bound_box.intersect(l_actor_mesh_bound_box);
		}

		bool bc_octal_tree_graph_node::is_leaf_node() const noexcept
		{
			return m_top_left_back == nullptr;
		}

		void bc_octal_tree_graph_node::clear()
		{
			if(is_leaf_node())
			{
				m_actors.clear();
			}
			else
			{
				m_top_left_back->clear();
				m_top_left_front->clear();
				m_top_right_front->clear();
				m_top_right_back->clear();
				m_bottom_left_back->clear();
				m_bottom_left_front->clear();
				m_bottom_right_front->clear();
				m_bottom_right_back->clear();
			}
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
			auto* l_octal_tree_node = static_cast<bc_octal_tree_graph_node_entry*>(p_node);
			auto l_next_internal_iterator = l_octal_tree_node->m_internal_iterator;
			++l_next_internal_iterator;

			if(std::end(m_actors) != l_next_internal_iterator)
			{
				return &*l_next_internal_iterator;
			}

			bc_octal_tree_graph_node* l_sibling_node = nullptr;
			while (l_sibling_node != nullptr && !l_sibling_node->is_leaf_node()) // find a leaf sibling node
			{
				l_sibling_node = _get_next_sibling_node();
			}

			if(l_sibling_node == nullptr)
			{
				return nullptr;
			}

			return &*l_sibling_node->m_actors.begin();
		}

		bc_iscene_graph_node::node_type* bc_octal_tree_graph_node::iterator_decrement(node_type* p_node) const noexcept
		{
			auto* l_octal_tree_node = static_cast<bc_octal_tree_graph_node_entry*>(p_node);
			auto l_next_internal_iterator = l_octal_tree_node->m_internal_iterator;
			--l_next_internal_iterator;

			if (std::end(m_actors) != l_next_internal_iterator)
			{
				return &*l_next_internal_iterator;
			}

			bc_octal_tree_graph_node* l_sibling_node = nullptr;
			while (l_sibling_node != nullptr && !l_sibling_node->is_leaf_node()) // find a leaf sibling node
			{
				l_sibling_node = _get_prev_sibling_node();
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

			for(bc_iscene_graph_node_entry& l_entry : m_actors)
			{
				m_top_left_back->add_actor(l_entry.m_actor);
				m_top_left_front->add_actor(l_entry.m_actor);
				m_top_right_front->add_actor(l_entry.m_actor);
				m_top_right_back->add_actor(l_entry.m_actor);
				m_bottom_left_back->add_actor(l_entry.m_actor);
				m_bottom_left_front->add_actor(l_entry.m_actor);
				m_bottom_right_front->add_actor(l_entry.m_actor);
				m_bottom_right_back->add_actor(l_entry.m_actor);
			}

			m_actors.clear();
		}

		void bc_octal_tree_graph_node::_merge()
		{
			m_actors.splice(std::rbegin(m_actors).base(), m_top_left_back->m_actors);
			m_actors.splice(std::rbegin(m_actors).base(), m_top_left_front->m_actors);
			m_actors.splice(std::rbegin(m_actors).base(), m_top_right_front->m_actors);
			m_actors.splice(std::rbegin(m_actors).base(), m_top_right_back->m_actors);
			m_actors.splice(std::rbegin(m_actors).base(), m_bottom_left_back->m_actors);
			m_actors.splice(std::rbegin(m_actors).base(), m_bottom_left_front->m_actors);
			m_actors.splice(std::rbegin(m_actors).base(), m_bottom_right_front->m_actors);
			m_actors.splice(std::rbegin(m_actors).base(), m_bottom_right_back->m_actors);

			m_top_left_back.reset();
			m_top_left_front.reset();
			m_top_right_front.reset();
			m_top_right_back.reset();
			m_bottom_left_back.reset();
			m_bottom_left_front.reset();
			m_bottom_right_front.reset();
			m_bottom_right_back.reset();
		}

		void bc_octal_tree_graph_node::_find_intersecting_nodes(const physics::bc_bound_box& p_box, core::bc_vector_frame<bc_octal_tree_graph_node*>& p_result)
		{
			if(!m_bound_box.intersect(p_box))
			{
				return;
			}

			if(m_top_left_back == nullptr)
			{
				p_result.push_back(this);
				return;
			}

			m_top_left_back->_find_intersecting_nodes(p_box, p_result);
			m_top_left_front->_find_intersecting_nodes(p_box, p_result);
			m_top_right_front->_find_intersecting_nodes(p_box, p_result);
			m_top_right_back->_find_intersecting_nodes(p_box, p_result);
			m_bottom_left_back->_find_intersecting_nodes(p_box, p_result);
			m_bottom_left_front->_find_intersecting_nodes(p_box, p_result);
			m_bottom_right_front->_find_intersecting_nodes(p_box, p_result);
			m_bottom_right_back->_find_intersecting_nodes(p_box, p_result);
		}

		bc_octal_tree_graph_node* bc_octal_tree_graph_node::_get_next_sibling_node() const
		{
			if(m_parent == nullptr)
			{
				return nullptr;
			}

			if(this == m_parent->m_top_left_back.get())
			{
				return m_parent->m_top_left_front.get();
			}
			if (this == m_parent->m_top_left_front.get())
			{
				return m_parent->m_top_right_front.get();
			}
			if (this == m_parent->m_top_right_front.get())
			{
				return m_parent->m_top_right_back.get();
			}
			if (this == m_parent->m_top_right_back.get())
			{
				return m_parent->m_bottom_left_back.get();
			}
			if (this == m_parent->m_bottom_left_back.get())
			{
				return m_parent->m_bottom_left_front.get();
			}
			if (this == m_parent->m_bottom_left_front.get())
			{
				return m_parent->m_bottom_right_front.get();
			}
			if (this == m_parent->m_bottom_right_front.get())
			{
				return m_parent->m_bottom_right_back.get();
			}
			if (this == m_parent->m_bottom_right_back.get())
			{
				return m_parent->_get_next_sibling_node();
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
				return m_parent->m_bottom_right_front.get();
			}
			if (this == m_parent->m_bottom_right_front.get())
			{
				return m_parent->m_bottom_left_front.get();
			}
			if (this == m_parent->m_bottom_left_front.get())
			{
				return m_parent->m_bottom_left_back.get();
			}
			if (this == m_parent->m_bottom_left_back.get())
			{
				return m_parent->m_top_right_back.get();
			}
			if (this == m_parent->m_top_right_back.get())
			{
				return m_parent->m_top_right_front.get();
			}
			if (this == m_parent->m_top_right_front.get())
			{
				return m_parent->m_top_left_front.get();
			}
			if (this == m_parent->m_top_left_front.get())
			{
				return m_parent->m_top_left_back.get();
			}
			if (this == m_parent->m_top_left_back.get())
			{
				return m_parent->_get_next_sibling_node();
			}

			bcAssert(false);
			return nullptr;
		}

		bc_octal_tree_graph_node& bc_octal_tree_graph_node::_find_containing_node(const physics::bc_bound_box& p_box)
		{
			if(m_bound_box.contains(p_box))
			{
				return *this;
			}

			bcAssert(m_parent != nullptr);

			return m_parent->_find_containing_node(p_box);
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