// [06/10/2021 MRB]

#pragma once

#include "Platform/bcException.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		class bci_network_server_manager_hook
		{
		public:
			virtual void started_listening(bcUINT16 p_port) = 0;

			virtual void client_connected() = 0;

			virtual void client_disconnected() = 0;

			virtual void message_sent(bci_network_message& p_message) = 0;

			virtual void message_received(bci_network_message& p_message) = 0;
			
			virtual void error_occurred(const bc_network_exception* p_exception) = 0;
		};
	}
}