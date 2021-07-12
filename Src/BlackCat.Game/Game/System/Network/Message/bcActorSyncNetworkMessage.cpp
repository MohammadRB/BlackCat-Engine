// [07/08/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Network/Message/bcActorSyncNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		bc_actor_sync_network_message::bc_actor_sync_network_message()
			: bci_network_message(message_name())
		{
		}

		bc_actor_sync_network_message::bc_actor_sync_network_message(bc_actor_sync_network_message&&) noexcept = default;

		bc_actor_sync_network_message::~bc_actor_sync_network_message() = default;

		bc_actor_sync_network_message& bc_actor_sync_network_message::operator=(bc_actor_sync_network_message&&) noexcept = default;

		void bc_actor_sync_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
		{
		}

		void bc_actor_sync_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
		{
		}
	}
}