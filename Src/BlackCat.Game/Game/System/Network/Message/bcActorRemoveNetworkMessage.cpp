// [08/07/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/System/Network/Message/bcActorRemoveNetworkMessage.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/bcJsonParse.h"

namespace black_cat
{
	namespace game
	{
		bc_actor_remove_network_message::bc_actor_remove_network_message()
			: bci_network_message(message_name()),
			m_net_id(g_invalid_actor_network_id)
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
			if(!m_actor.is_valid())
			{
				return;
			}
			
			p_context.m_visitor.remove_actor(m_actor);
		}

		void bc_actor_remove_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			if(!m_actor.is_valid())
			{
				return;
			}
			
			p_context.m_visitor.remove_actor(p_context.m_client.m_address, m_actor);
		}

		void bc_actor_remove_network_message::acknowledge(const bc_network_message_client_acknowledge_context& p_context) noexcept
		{
		}

		void bc_actor_remove_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
		{
			json_parse::bc_write(p_context.m_params, "nid", m_net_id);
		}

		void bc_actor_remove_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
		{
			const auto l_has_net_id = json_parse::bc_load(p_context.m_params, "nid", m_net_id);
			if (!l_has_net_id || m_net_id == g_invalid_actor_network_id)
			{
				core::bc_log(core::bc_log_type::error) << bcL("Failed to deserialize actor network id in remove network message") << core::bc_lend;
				return;
			}

			m_actor = p_context.m_visitor.get_actor(m_net_id).m_actor;
			if (!m_actor.is_valid())
			{
				core::bc_log(core::bc_log_type::debug) << bcL("Failed to find network actor with network id ") << m_net_id << bcL(" in remove network message") << core::bc_lend;
				return;
			}
		}
	}
}