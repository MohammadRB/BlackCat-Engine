// [05/27/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_network_component::bc_network_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept
			: bci_actor_component(p_actor_index, p_index),
			m_id(bc_actor::invalid_id),
			m_data_dir(bc_actor_network_data_dir::download)
		{
		}

		bc_network_component::bc_network_component(bc_network_component&&) noexcept = default;

		bc_network_component::~bc_network_component() = default;

		bc_network_component& bc_network_component::operator=(bc_network_component&&) noexcept = default;

		bc_actor bc_network_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_network_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const auto& l_data_dir_param = p_context.m_parameters.get_value_throw<core::bc_string>(constant::g_param_network_data_dir);
			
			if(l_data_dir_param == "upload")
			{
				m_data_dir = bc_actor_network_data_dir::upload;
			}
			else if(l_data_dir_param == "upload_stream")
			{
				m_data_dir = bc_actor_network_data_dir::upload_stream;
			}
			else if(l_data_dir_param == "download")
			{
				m_data_dir = bc_actor_network_data_dir::download;
			}
			else
			{
				throw bc_invalid_argument_exception("Invalid value for network data direction parameter");
			}
		}

		void bc_network_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
		}

		void bc_network_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_scene_add_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if (l_scene_add_event)
			{
				if(m_data_dir == bc_actor_network_data_dir::upload_stream)
				{
					p_context.m_game_system.get_network_system().add_actor(p_context.m_actor);
				}
				else if(m_data_dir == bc_actor_network_data_dir::upload)
				{
					p_context.m_game_system.get_network_system().send_message(bc_actor_replicate_network_message());
				}

				return;
			}

			const auto* l_scene_remove_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event);
			if (l_scene_remove_event)
			{
				if (m_data_dir == bc_actor_network_data_dir::upload_stream || m_data_dir == bc_actor_network_data_dir::upload)
				{
					p_context.m_game_system.get_network_system().remove_actor(p_context.m_actor);
				}
					
				return;
			}
		}
	}	
}