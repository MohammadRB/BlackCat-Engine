// [07/08/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/System/Network/Message/bcActorRemoveNetworkMessage.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_actor_remove_network_message::bc_actor_remove_network_message()
			: bci_network_message(message_name())
		{
		}

		bc_actor_remove_network_message::bc_actor_remove_network_message(bc_actor& p_actor)
			: bci_network_message(message_name()),
			m_actor(p_actor)
		{
		}

		bc_actor_remove_network_message::bc_actor_remove_network_message(bc_actor_remove_network_message&&) noexcept = default;

		bc_actor_remove_network_message::~bc_actor_remove_network_message() = default;

		bc_actor_remove_network_message& bc_actor_remove_network_message::operator=(bc_actor_remove_network_message&&) noexcept = default;

		void bc_actor_remove_network_message::execute(const bc_network_message_client_context& p_context) noexcept
		{
		}

		void bc_actor_remove_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
		}

		void bc_actor_remove_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
		{
			const auto* l_network_component = m_actor.get_component<bc_network_component>();
			p_context.m_params.add("net_id", core::bc_any(l_network_component->get_network_id()));
		}

		void bc_actor_remove_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
		{
			const auto* l_actor_network_id = p_context.m_params.find("net_id")->second.as<bc_actor_network_id>();
			if (!l_actor_network_id || *l_actor_network_id == bc_actor::invalid_id)
			{
				core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize actor network id in remove network message"));
				return;
			}

			m_actor = p_context.m_visitor.get_actor(*l_actor_network_id);
		}
	}
}