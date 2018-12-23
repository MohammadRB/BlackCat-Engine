// [07/14/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderThread.h"
#include "Game/Object/Scene/SceneGraph/bcScenceGraph.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_scene_graph::bc_scene_graph(core::bc_unique_ptr<bc_iscene_graph_node> p_scene_graph)
			: m_graph_node(std::move(p_scene_graph))
		{
		}

		bc_scene_graph::~bc_scene_graph()
		{
			if(m_graph_node)
			{
				clear();
			}
		}

		void bc_scene_graph::add_actor(bc_actor& p_actor)
		{
			m_graph_node->add_actor(p_actor);
		}

		void bc_scene_graph::update_actor(bc_actor& p_actor, const physics::bc_bound_box& p_previous_box)
		{
			m_graph_node->update_actor(p_actor, p_previous_box);
		}

		void bc_scene_graph::remove_actor(bc_actor& p_actor)
		{
			m_graph_node->remove_actor(p_actor);
		}

		core::bc_vector_frame<bc_actor> bc_scene_graph::get_height_maps() const
		{
			core::bc_vector_frame<bc_actor> l_result;

			for (bc_actor l_actor : *m_graph_node)
			{
				if(l_actor.has_component<bc_height_map_component>())
				{
					l_result.push_back(l_actor);
				}
			}

			return l_result;
		}

		void bc_scene_graph::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
		}

		void bc_scene_graph::render_heightmaps(bc_render_system& p_render_system, bc_render_thread& p_render_thread)
		{
			for (bc_actor l_actor : *m_graph_node)
			{
				auto* l_height_map_component = l_actor.get_component< bc_height_map_component >();

				if (!l_height_map_component)
				{
					continue;
				}

				auto* l_render_component = l_actor.get_component< bc_render_component >();

				if (!l_render_component)
				{
					continue;
				}

				l_height_map_component->render(*l_render_component);
			}

			p_render_system.render_all_instances(p_render_thread);
			p_render_system.clear_render_instances();
		}

		void bc_scene_graph::render_meshes(bc_render_system& p_render_system, bc_render_thread& p_render_thread, bool p_preserve_render_instances)
		{
			for(bc_actor l_actor : *m_graph_node)
			{
				auto* l_mesh_component = l_actor.get_component< bc_mesh_component >();

				if (!l_mesh_component)
				{
					continue;
				}

				auto* l_render_component = l_actor.get_component< bc_render_component >();

				if(!l_render_component)
				{
					continue;
				}

				l_mesh_component->render(l_actor, *l_render_component);
			}

			p_render_system.render_all_instances(p_render_thread);
			if(!p_preserve_render_instances)
			{
				p_render_system.clear_render_instances();
			}
		}

		void bc_scene_graph::clear()
		{
			bcAssert(m_graph_node);

			for (auto& l_actor : *m_graph_node)
			{
				l_actor.destroy();
			}

			m_graph_node->clear();
		}
	}
}