// [11/29/2018 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphNode.h"
#include "GraphicImp/bcRenderApiInfo.h"

namespace black_cat
{
	namespace game
	{
		bc_octal_tree_graph_node::bc_octal_tree_graph_node(physics::bc_bound_box p_box)
			: m_parent(nullptr),
			m_top_left_back(nullptr),
			m_top_left_front(nullptr),
			m_top_right_front(nullptr),
			m_top_right_back(nullptr),
			m_bottom_left_back(nullptr),
			m_bottom_left_front(nullptr),
			m_bottom_right_front(nullptr),
			m_bottom_right_back(nullptr),
			m_bound_box(p_box),
			m_actors(bcNew(core::bc_vector_movale<bc_actor>(), core::bc_alloc_type::unknown))
		{
		}

		bc_octal_tree_graph_node::bc_octal_tree_graph_node(const bc_octal_tree_graph_node& p_parent, bc_octal_tree_node_position p_my_position)
			: m_parent(&p_parent),
			m_top_left_back(nullptr),
			m_top_left_front(nullptr),
			m_top_right_front(nullptr),
			m_top_right_back(nullptr),
			m_bottom_left_back(nullptr),
			m_bottom_left_front(nullptr),
			m_bottom_right_front(nullptr),
			m_bottom_right_back(nullptr),
			m_bound_box()
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

		void bc_octal_tree_graph_node::add_actor(bc_actor p_actor)
		{
		}

		void bc_octal_tree_graph_node::update_actor(bc_actor p_actor)
		{
			
		}

		void bc_octal_tree_graph_node::remove_actor(bc_actor p_actor)
		{
		}

		void bc_octal_tree_graph_node::_split()
		{
		}
	}
}