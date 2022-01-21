// [06/02/2021 MRB]

#pragma once

#include "Core/Container/bcSpan.h"
#include "Core/File/bcMemoryStream.h"
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
			virtual void connecting_to_server(const platform::bc_network_address& p_address) noexcept = 0;
			
			virtual void connection_to_server_approved(const platform::bc_network_address& p_address, core::bc_string p_error_message) noexcept = 0;

			virtual void message_packet_sent(const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<bc_network_message_ptr> p_messages) noexcept = 0;
			
			virtual void message_packet_received(const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<bc_network_message_ptr> p_messages) noexcept = 0;

			virtual void error_occurred(const bc_network_exception* p_exception) noexcept = 0;
		};
	}	
}