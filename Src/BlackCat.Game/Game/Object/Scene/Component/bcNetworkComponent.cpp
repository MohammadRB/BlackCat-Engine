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
			m_data_dir(bc_actor_network_data_dir::replicate)
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
			
			if(l_data_dir_param == "replicate")
			{
				m_data_dir = bc_actor_network_data_dir::replicate;
			}
			else if(l_data_dir_param == "replicate_sync")
			{
				m_data_dir = bc_actor_network_data_dir::replicate_sync;
			}
			else if(l_data_dir_param == "replicate_sync_from_client")
			{
				m_data_dir = bc_actor_network_data_dir::replicate_sync_from_client;
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
			const auto l_network_type = p_context.m_game_system.get_network_system().get_network_type();
			
			const auto* l_scene_add_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if (l_scene_add_event)
			{
				if(l_network_type == bc_network_type::server)
				{
					if (m_data_dir == bc_actor_network_data_dir::replicate_sync)
					{
						p_context.m_game_system.get_network_system().add_actor(p_context.m_actor);
					}
					else if(m_data_dir == bc_actor_network_data_dir::replicate)
					{
						p_context.m_game_system.get_network_system().send_message(bc_actor_replicate_network_message(p_context.m_actor));
					}
				}
				else if(l_network_type == bc_network_type::client)
				{
					if(m_data_dir == bc_actor_network_data_dir::replicate_sync_from_client)
					{
						p_context.m_game_system.get_network_system().add_actor(p_context.m_actor);
					}
					// we should remove network actors if they are not loaded through network, because they will be replicated by the server
					else if(m_id == invalid_id && (m_data_dir == bc_actor_network_data_dir::replicate_sync || m_data_dir == bc_actor_network_data_dir::replicate))
					{
						l_scene_add_event->get_scene().remove_actor(p_context.m_actor);
					}
				}
				
				return;
			}

			const auto* l_scene_remove_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event);
			if (l_scene_remove_event)
			{
				if (l_network_type == bc_network_type::server)
				{
					if (m_data_dir == bc_actor_network_data_dir::replicate_sync)
					{
						p_context.m_game_system.get_network_system().remove_actor(p_context.m_actor);
					}
				}
				else if (l_network_type == bc_network_type::client)
				{
					if (m_data_dir == bc_actor_network_data_dir::replicate_sync_from_client)
					{
						p_context.m_game_system.get_network_system().remove_actor(p_context.m_actor);
					}
				}
				
				return;
			}
		}

		void bc_network_component::load_network_instance(const bc_actor_component_network_load_context& p_context)
		{
			auto* l_net_id_param = p_context.m_parameters.find("net_id")->second.as<bc_actor_network_id>();
			if(!l_net_id_param)
			{
				m_id = bc_actor::invalid_id;
				return;
			}

			m_id = *l_net_id_param;
		}

		void bc_network_component::write_network_instance(const bc_actor_component_network_write_context& p_context)
		{
			p_context.m_parameters.add("net_id", core::bc_any(m_id));
		}
	}
}