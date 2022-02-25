// [02/24/2022 MRB]

#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcCheckPointComponent.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "BoX.Game/Application/bxSceneCheckPoint.h"

namespace box
{
	bx_scene_checkpoint::bx_scene_checkpoint(game::bc_scene& p_scene)
		: bc_scene_checkpoint(p_scene)
	{
	}

	core::bc_vector_frame<game::bc_actor> bx_scene_checkpoint::export_dynamic_actors() const
	{
		core::bc_vector_frame<game::bc_actor> l_result;
		l_result.reserve(256);

		for (const auto& l_actor : m_scene->get_scene_graph())
		{
			if (!l_actor.has_component<game::bc_checkpoint_component>())
			{
				continue;
			}

			if(const auto* l_network_component = l_actor.get_component<game::bc_network_component>())
			{
				const auto l_is_replicated = l_network_component->get_network_replication_side() == game::bc_actor_replication_side::replicated;

				// Replicated actors will be added and removed by remote party
				if (l_is_replicated)
				{
					continue;
				}
			}

			l_result.push_back(l_actor);
		}

		return l_result;
	}

	void bx_scene_checkpoint::remove_dynamic_actors() const
	{
		for (auto& l_actor : m_scene->get_scene_graph())
		{
			if (!l_actor.has_component<game::bc_checkpoint_component>())
			{
				continue;
			}

			if (const auto* l_network_component = l_actor.get_component<game::bc_network_component>())
			{
				const auto l_is_replicated = l_network_component->get_network_replication_side() == game::bc_actor_replication_side::replicated;

				// Replicated actors will be added and removed by remote party
				if (l_is_replicated)
				{
					continue;
				}
			}

			m_scene->remove_actor(l_actor);
		}
	}
}
