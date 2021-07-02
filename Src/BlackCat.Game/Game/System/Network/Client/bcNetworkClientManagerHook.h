// [06/02/2021 MRB]

#pragma once

#include "Platform/bcException.h"
#include "PlatformImp/Network/bcNetworkAddress.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		class bci_network_client_manager_hook
		{
		public:
			virtual void connecting_to_server(const platform::bc_network_address& p_address) = 0;
			
			virtual void connected_to_server(const platform::bc_network_address& p_address) = 0;

			virtual void message_sent(bci_network_message& p_message) = 0;
			
			virtual void message_received(bci_network_message& p_message) = 0;

			virtual void error_occurred(const bc_network_exception* p_exception) = 0;
		};
	}	
}