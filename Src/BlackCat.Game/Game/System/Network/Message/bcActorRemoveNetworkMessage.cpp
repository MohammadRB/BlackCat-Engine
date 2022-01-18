// [07/08/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/System/Network/Message/bcActorRemoveNetworkMessage.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_actor_remove_network_message::bc_actor_remove_network_message()
			: bci_network_message(message_name()),
			m_net_id(bc_actor::invalid_id)
		{
		}

		bc_actor_remove_network_message::bc_actor_remove_network_message(bc_actor_network_id p_net_id)
			: bci_network_message(message_name()),
			m_net_id(p_net_id)
		{
		}

		bc_actor_remove_network_message::bc_actor_remove_network_message(bc_actor_remove_network_message&&) noexcept = default;

		bc_actor_remove_network_message::~bc_actor_remove_network_message() = default;

		bc_actor_remove_network_message& bc_actor_remove_network_message::operator=(bc_actor_remove_network_message&&) noexcept = default;

		void bc_actor_remove_network_message::execute(const bc_network_message_client_context& p_context) noexcept
		{
			if(m_actor == nullptr)
			{
				return;
			}
			
			p_context.m_visitor.remove_actor(m_actor);
		}

		void bc_actor_remove_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			if(m_actor == nullptr)
			{
				return;
			}
			
			p_context.m_visitor.remove_actor(p_context.m_address, m_actor);
		}

		void bc_actor_remove_network_message::acknowledge(const bc_network_message_client_acknowledge_context& p_context) noexcept
		{
		}

		void bc_actor_remove_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
		{
			p_context.m_params.add("nid", core::bc_any(m_net_id));
		}

		void bc_actor_remove_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
		{
			const auto* l_actor_network_id = p_context.m_params.find("nid")->second.as<bc_actor_network_id>();
			if (!l_actor_network_id || *l_actor_network_id == bc_actor::invalid_id)
			{
				core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize actor network id in remove network message"));
				return;
			}

			m_actor = p_context.m_visitor.get_actor(*l_actor_network_id).m_actor;
			if (!m_actor.is_valid())
			{
				core::bc_log(core::bc_log_type::warning, bcL("Failed to find network actor in remove network message"));
				return;
			}
		}
	}
}