// [06/02/2021 MRB]

#pragma once

#include "PlatformImp/Network/bcNonBlockSocket.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		class bci_network_client_manager_hook
		{
		public:
			void connecting_to_server(const bcCHAR* p_ip, bcUINT16 p_port);
			
			void connected_to_server(const bcCHAR* p_ip, bcUINT16 p_port);
			
			void error_occurred(const bc_network_exception* p_exception);

			void message_sent(bci_network_message& p_message);
			
			void message_received(bci_network_message& p_message);
		};
	}	
}