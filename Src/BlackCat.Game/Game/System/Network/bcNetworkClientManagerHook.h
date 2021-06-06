// [06/02/2021 MRB]

#pragma once

#include "PlatformImp/Network/bcNonBlockSocket.h"
#include "Game/System/Network/Command/bcNetworkCommand.h"

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

			void command_sent(bci_network_command& p_command);
			
			void command_received(bci_network_command& p_command);
		};
	}	
}