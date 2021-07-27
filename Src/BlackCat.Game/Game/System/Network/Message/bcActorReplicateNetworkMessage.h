// [06/06/2021 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_actor_replicate_network_message : public bci_network_message
		{
			BC_NETWORK_MESSAGE(act_rpl)

		public:
			bc_actor_replicate_network_message();

			explicit bc_actor_replicate_network_message(const bc_actor& p_actor);

			bc_actor_replicate_network_message(bc_actor_replicate_network_message&&) noexcept;

			~bc_actor_replicate_network_message() override;

			bc_actor_replicate_network_message& operator=(bc_actor_replicate_network_message&&) noexcept;

			bool need_acknowledgment() const noexcept override;

			void execute(const bc_network_message_client_context& p_context) noexcept override;
			
			void execute(const bc_network_message_server_context& p_context) noexcept override;
			
		private:
			void serialize_message(const bc_network_message_serialization_context& p_context) override;

			void deserialize_message(const bc_network_message_deserialization_context& p_context) override;

			bc_actor m_actor;
		};

		inline bool bc_actor_replicate_network_message::need_acknowledgment() const noexcept
		{
			return true;
		}
	}
}