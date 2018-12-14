// [11/29/2018 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/bcActor.h"

namespace black_cat
{
	namespace game
	{
		class bc_iscene_graph_node
		{
		public:
			virtual ~bc_iscene_graph_node() = default;

			virtual void add_actor(bc_actor p_actor) = 0;

			virtual void update_actor(bc_actor p_actor) = 0;

			virtual void remove_actor(bc_actor p_actor) = 0;

			virtual void get_bound_box(physics::bc_bound_box& p_box, physics::bc_transform& p_box_transform) = 0;
		};

		enum class bc_octal_tree_node_position
		{
			top_left_back,
			top_left_front,
			top_right_front,
			top_right_back,
			bottom_left_back,
			bottom_left_front,
			bottom_right_front,
			bottom_right_back
		};

		class BC_GAME_DLL bc_octal_tree_graph_node : bc_iscene_graph_node
		{
		public:
			explicit bc_octal_tree_graph_node(physics::bc_bound_box p_box);

			bc_octal_tree_graph_node(const bc_octal_tree_graph_node& p_parent, bc_octal_tree_node_position p_my_position);

			bc_octal_tree_graph_node(bc_octal_tree_graph_node&&) = default;

			~bc_octal_tree_graph_node() = default;

			bc_octal_tree_graph_node& operator=(bc_octal_tree_graph_node&&) = default;

			void add_actor(bc_actor p_actor) override;

			void update_actor(bc_actor p_actor) override;

			void remove_actor(bc_actor p_actor) override;

		protected:

		private:
			void _split();

			const bc_octal_tree_graph_node* m_parent;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_top_left_back;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_top_left_front;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_top_right_front;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_top_right_back;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_bottom_left_back;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_bottom_left_front;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_bottom_right_front;
			core::bc_unique_ptr<bc_octal_tree_graph_node> m_bottom_right_back;

			physics::bc_bound_box m_bound_box;
			core::bc_vector_movale<bc_actor>* m_actors;
		};
	}
}