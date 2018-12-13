// [11/29/2018 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphNode.h"
#include "GraphicImp/bcRenderApiInfo.h"

namespace black_cat
{
	namespace game
	{
		bc_octal_tree_graph_node::bc_octal_tree_graph_node(physics::bc_shape_box p_box)
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
			m_bound_box_transform()
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
			m_bound_box(p_parent.m_bound_box.get_half_extends() / 2),
			m_bound_box_transform()
		{
			core::bc_vector3f l_bound_box_position = p_parent.m_bound_box_transform.get_position();
			core::bc_vector3f l_half_extend = p_parent.m_bound_box.get_half_extends() / 2;
			const auto l_z_sign = graphic::bc_render_api_info::is_left_handed() ? +1 : -1;

			switch (p_my_position)
			{
			case bc_octal_tree_node_position::top_left_back:
				l_bound_box_position.x -= l_half_extend.x;
				l_bound_box_position.y += l_half_extend.y;
				l_bound_box_position.z += (l_half_extend.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::top_left_front:
				l_bound_box_position.x -= l_half_extend.x;
				l_bound_box_position.y += l_half_extend.y;
				l_bound_box_position.z -= (l_half_extend.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::top_right_front:
				l_bound_box_position.x += l_half_extend.x;
				l_bound_box_position.y += l_half_extend.y;
				l_bound_box_position.z -= (l_half_extend.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::top_right_back:
				l_bound_box_position.x += l_half_extend.x;
				l_bound_box_position.y += l_half_extend.y;
				l_bound_box_position.z += (l_half_extend.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::bottom_left_back:
				l_bound_box_position.x -= l_half_extend.x;
				l_bound_box_position.y -= l_half_extend.y;
				l_bound_box_position.z += (l_half_extend.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::bottom_left_front:
				l_bound_box_position.x -= l_half_extend.x;
				l_bound_box_position.y -= l_half_extend.y;
				l_bound_box_position.z -= (l_half_extend.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::bottom_right_front:
				l_bound_box_position.x += l_half_extend.x;
				l_bound_box_position.y -= l_half_extend.y;
				l_bound_box_position.z -= (l_half_extend.z * l_z_sign);
				break;
			case bc_octal_tree_node_position::bottom_right_back:
				l_bound_box_position.x += l_half_extend.x;
				l_bound_box_position.y -= l_half_extend.y;
				l_bound_box_position.z += (l_half_extend.z * l_z_sign);
				break;
			}

			m_bound_box_transform = physics::bc_transform(l_bound_box_position);
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