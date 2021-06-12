// [06/10/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Network/Server/bcNetworkServerManager.h"
#include "Game/System/Network/Server/bcNetworkServerManagerClient.h"

namespace black_cat
{
	namespace game
	{
		void bc_network_server_manager_client::on_enter(bc_server_client_socket_connected_state& p_state)
		{
			m_socket_is_connected = true;
			m_socket_is_ready = true;
		}

		void bc_network_server_manager_client::on_enter(bc_server_client_socket_sending_state& p_state)
		{
			m_socket_is_ready = false;
		}

		void bc_network_server_manager_client::on_enter(bc_server_client_socket_error_state& p_state)
		{
			m_socket_is_connected = false;
			m_socket_is_ready = false;
			m_manager->client_disconnected(*this);
		}
	}
}