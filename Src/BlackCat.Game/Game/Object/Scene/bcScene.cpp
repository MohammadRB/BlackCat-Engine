// [01/13/2017 MRB]

#include "Game/GamePCH.h"

#include "PhysicsImp/Body/bcRigidBody.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_scene::bc_scene(core::bc_estring p_path,
			core::bc_string p_name,
			core::bc_vector<core::bc_string> p_loaded_streams,
			bc_scene_graph p_scene_graph, 
			physics::bc_scene_ref p_px_scene)
			: m_path(std::move(p_path)),
			m_name(std::move(p_name)),
			m_loaded_streams(std::move(p_loaded_streams)),
			m_scene_graph(std::move(p_scene_graph)),
			m_px_scene(std::move(p_px_scene))
		{
		}

		bc_scene::bc_scene(bc_scene&& p_other) noexcept
			: bc_scene
			(
				std::move(p_other.m_path),
				std::move(p_other.m_name),
				std::move(p_other.m_loaded_streams),
				std::move(p_other.m_scene_graph),
				std::move(p_other.m_px_scene)
			)
		{
		}

		bc_scene::~bc_scene()
		{
			m_scene_graph.clear();
			m_px_scene.reset();

			auto* l_content_stream_manager = core::bc_get_service<core::bc_content_stream_manager>();
			for (auto& l_stream : m_loaded_streams)
			{
				l_content_stream_manager->unload_content_stream(l_stream.c_str());
			}
		}

		bc_scene& bc_scene::operator=(bc_scene&& p_other) noexcept
		{
			m_path = std::move(p_other.m_path);
			m_name = std::move(p_other.m_name);
			m_loaded_streams = std::move(p_other.m_loaded_streams);
			m_scene_graph = std::move(p_other.m_scene_graph);
			m_px_scene = std::move(p_other.m_px_scene);

			return *this;
		}

		bc_scene_graph_buffer bc_scene::get_actors(const bc_camera_frustum& p_camera_frustum) const
		{
			return m_scene_graph.get_actors(p_camera_frustum);
		}

		void bc_scene::add_actor(bc_actor& p_actor)
		{
			const bool l_added = m_scene_graph.add_actor(p_actor);
			if(!l_added)
			{
				p_actor.destroy();
				return;
			}

			auto* l_rigid_component = p_actor.get_component<bc_rigid_body_component>();
			if (l_rigid_component)
			{
				physics::bc_rigid_body l_rigid_body = l_rigid_component->get_body();
				m_px_scene->add_actor(l_rigid_body);
			}
		}

		void bc_scene::remove_actor(bc_actor& p_actor)
		{
			auto* l_rigid_component = p_actor.get_component<bc_rigid_body_component>();
			if (l_rigid_component)
			{
				physics::bc_rigid_body l_rigid_body = l_rigid_component->get_body();
				m_px_scene->remove_actor(l_rigid_body);
			}

			m_scene_graph.remove_actor(p_actor);
		}

		void bc_scene::add_debug_shapes(bc_shape_drawer& p_shape_drawer) const
		{
			m_scene_graph.add_debug_shapes(p_shape_drawer);
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
					auto* l_mediate_component = l_actor.get_component<bc_mediate_component>();

					physics::bc_bound_box l_actor_prev_bound_box = l_mediate_component->get_bound_box();
					l_mediate_component->set_world_transform(l_px_actor.m_global_pose.get_matrix4());

					const bool l_updated = m_scene_graph.update_actor(l_actor, l_actor_prev_bound_box);
					if(!l_updated)
					{
						l_mediate_component->set_bound_box(l_actor_prev_bound_box);
						remove_actor(l_actor);
						l_actor.destroy();
					}
				}
			}

			m_scene_graph.update(p_time);
		}
	}
}