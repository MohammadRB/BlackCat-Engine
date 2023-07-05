// [10/06/2021 MRB]

#pragma once

#include "Core/Container/bcSpan.h"
#include "Core/File/bcMemoryStream.h"
#include "Platform/bcException.h"
#include "Game/System/Network/Server/bcNetworkClient.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"

namespace black_cat::game
{
	class bci_network_server_manager_hook
	{
	public:
		virtual void started_listening(bcUINT16 p_port) noexcept = 0;

		virtual void scene_changed(bc_scene* p_scene) noexcept = 0;

		/**
			 * \brief 
			 * \param p_client 
			 * \return empty string if connection was approved otherwise error message
			 */
		virtual core::bc_string client_connected(const bc_network_client& p_client) noexcept = 0;

		virtual void client_disconnected(const bc_network_client& p_client) noexcept = 0;

		virtual void message_packet_sent(const bc_network_client& p_client, const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<bc_network_message_ptr> p_messages) noexcept {}

		virtual void message_packet_received(const bc_network_client& p_client, const core::bc_memory_stream& p_packet, bcSIZE p_packet_size, core::bc_const_span<bc_network_message_ptr> p_messages) noexcept {}
			
		virtual void error_occurred(const bc_network_client* p_client, const bc_network_exception* p_exception) noexcept = 0;
	};
}
