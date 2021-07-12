// [06/10/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Network/Server/bcNetworkServerManager.h"
#include "Game/System/Network/Server/bcNetworkServerManagerTcpClient.h"

namespace black_cat
{
	namespace game
	{
		bc_network_server_manager_tcp_client::bc_network_server_manager_tcp_client(bc_network_server_manager& p_manager, platform::bc_non_block_socket p_socket)
			: bc_server_tcp_client_socket_state_machine(*BC_NEW(platform::bc_non_block_socket, core::bc_alloc_type::unknown)(std::move(p_socket))),
			m_manager(&p_manager),
			m_socket_is_connected(false),
			m_socket_is_ready(false),
			m_last_sync_time(0),
			m_rtt_sampler(100)
		{
			m_socket.reset(&bc_server_tcp_client_socket_state_machine::get_socket());
			bc_server_tcp_client_socket_state_machine::transfer_state<bc_server_tcp_client_socket_connected_state>();
		}
		
		void bc_network_server_manager_tcp_client::on_enter(bc_server_tcp_client_socket_connected_state& p_state)
		{
			m_socket_is_connected = true;
			m_socket_is_ready = true;
		}

		void bc_network_server_manager_tcp_client::on_enter(bc_server_tcp_client_socket_sending_state& p_state)
		{
			m_socket_is_ready = false;
		}

		void bc_network_server_manager_tcp_client::on_enter(bc_server_tcp_client_socket_error_state& p_state)
		{
			m_socket_is_connected = false;
			m_socket_is_ready = false;
			//m_manager->client_disconnected(*this);
		}
	}
}