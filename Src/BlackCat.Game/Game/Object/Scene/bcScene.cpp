// [01/13/2017 MRB]

#include "Game/GamePCH.h"

#include "PhysicsImp/Body/bcRigidBody.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_scene::bc_scene(bc_scene_graph&& p_scene_graph, physics::bc_scene_ref&& p_px_scene)
			: m_scene_graph(std::move(p_scene_graph)),
			m_px_scene(std::move(p_px_scene))
		{
		}

		bc_scene::bc_scene(bc_scene&& p_other) noexcept
			: bc_scene(std::move(p_other.m_scene_graph), std::move(p_other.m_px_scene))
		{	
		}

		bc_scene::~bc_scene()
		{
			m_scene_graph.clear();
		}

		bc_scene& bc_scene::operator=(bc_scene&& p_other) noexcept
		{
			m_scene_graph = std::move(p_other.m_scene_graph);
			m_px_scene = std::move(p_other.m_px_scene);

			return *this;
		}

		core::bc_vector_frame<bc_actor> bc_scene::get_height_maps() const
		{
			return m_scene_graph.get_height_maps();
		}

		void bc_scene::add_object(bc_actor p_actor)
		{
			bc_rigid_body_component* l_rigid_component = p_actor.get_component<bc_rigid_body_component>();

			if(l_rigid_component)
			{
				physics::bc_rigid_body l_rigid_body = l_rigid_component->get_body();
				m_px_scene->add_actor(l_rigid_body);
			}

			m_scene_graph.add_actor(p_actor);
		}

		void bc_scene::remove_object(bc_actor p_actor)
		{
			bc_rigid_body_component* l_rigid_component = p_actor.get_component<bc_rigid_body_component>();

			if (l_rigid_component)
			{
				physics::bc_rigid_body l_rigid_body = l_rigid_component->get_body();
				m_px_scene->remove_actor(l_rigid_body);
			}

			m_scene_graph.remove_actor(p_actor);
		}

		void bc_scene::render_height_maps(bc_render_system& p_render_system, bc_render_thread& p_render_thread)
		{
			m_scene_graph.render_heightmaps(p_render_system, p_render_thread);
		}

		void bc_scene::render_meshes(bc_render_system& p_render_system, bc_render_thread& p_render_thread, bool p_preserve_render_instances)
		{
			m_scene_graph.render_meshes(p_render_system, p_render_thread, p_preserve_render_instances);
		}

		void bc_scene::update(bc_physics_system& p_physics, core_platform::bc_clock::update_param p_time)
		{
			m_px_scene->update(p_time);
			m_px_scene->wait();

			auto l_px_actors = m_px_scene->get_active_actors();
			for(physics::bc_updated_actor& l_px_actor : l_px_actors)
			{
				auto l_rigid_body = l_px_actor.m_actor.is_rigid_body();
				if(l_rigid_body.is_valid())
				{
					bc_actor l_actor = p_physics.get_game_actor(l_rigid_body);
					auto* l_mesh_component = l_actor.get_component<bc_mesh_component>();
					if(l_mesh_component)
					{
						auto& l_actor_bound_box = l_actor.get_component<bc_mediate_component>()->get_bound_box();
						l_mesh_component->set_world_pos(l_px_actor.m_global_pose.get_matrix4());

						m_scene_graph.update_actor(l_actor, l_actor_bound_box);
					}
				}
			}

			m_scene_graph.update(p_time);
		}
	}
}