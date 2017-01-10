// [07/14/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderThread.h"
#include "Game/Object/Scene/bcScenceGraph.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_scene_graph::bc_scene_graph()
		{
		}

		bc_scene_graph::~bc_scene_graph()
		{
		}

		void bc_scene_graph::add_object(bc_actor p_actor)
		{
			m_objects.push_back(p_actor);
		}

		bc_actor bc_scene_graph::remove_object(bc_actor p_actor)
		{
			auto l_object = std::find(std::cbegin(m_objects), std::cend(m_objects), p_actor);

			m_objects.erase(l_object);

			return *l_object;
		}

		core::bc_vector_frame<bc_actor> bc_scene_graph::get_heightmaps() const
		{
			core::bc_vector_frame<bc_actor> l_result;

			for (auto& l_actor : m_objects)
			{
				if(l_actor.has_component<bc_height_map_component>())
				{
					l_result.push_back(l_actor);
				}
			}

			return l_result;
		}

		void bc_scene_graph::update(const bc_input_system& p_input_system, core_platform::bc_clock::update_param p_clock_update_param)
		{
		}

		void bc_scene_graph::render_heightmaps(bc_render_system& p_render_system, bc_render_thread& p_render_thread)
		{
			for (auto& l_actor : m_objects)
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
			for(auto& l_actor : m_objects)
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

				l_mesh_component->render(*l_render_component);
			}

			p_render_system.render_all_instances(p_render_thread);

			if(!p_preserve_render_instances)
			{
				p_render_system.clear_render_instances();
			}
		}

		void bc_scene_graph::clear()
		{
			for (auto& l_actor : m_objects)
			{
				l_actor.destroy();
			}

			m_objects.clear();
			m_objects.shrink_to_fit();
		}
	}
}
