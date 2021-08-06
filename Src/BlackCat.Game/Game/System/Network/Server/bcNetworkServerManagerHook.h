// [06/10/2021 MRB]

#pragma once

#include "Core/Container/bcSpan.h"
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

			virtual void message_packet_sent(bcSIZE p_packet_size, core::bc_const_span<bc_network_message_ptr> p_messages) = 0;

			virtual void message_packet_received(bcSIZE p_packet_size, core::bc_const_span<bc_network_message_ptr> p_messages) = 0;
			
			virtual void error_occurred(const bc_network_exception* p_exception) = 0;
		};
	}
}