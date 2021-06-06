// [06/01/2021 MRB]

#include "Game/GamePCH.h"

#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Network/bcNetworkClientManager.h"
#include "Game/System/Network/bcNetworkClientManagerHook.h"

namespace black_cat
{
	namespace game
	{
		bc_network_client_manager::bc_network_client_manager(bc_network_system& p_network_system, bci_network_client_manager_hook& p_hook, const bcCHAR* p_ip, bcUINT16 p_port)
			: bc_client_socket_state_machine(m_socket),
			m_ip(p_ip),
			m_port(p_port),
			m_last_rtt_time(0),
			m_rtt_time(0),
			m_commands_packet(p_network_system)
		{
			m_socket = platform::bc_non_block_socket
			(
				platform::bc_socket_address::inter_network,
				platform::bc_socket_type::data_gram,
				platform::bc_socket_protocol::udp
			);
			m_hook = &p_hook;

			bc_client_socket_state_machine::process_event
			(
				bc_socket_connect_event{ m_ip, m_port }
			);
		}

		bc_network_client_manager::bc_network_client_manager(bc_network_client_manager&&) noexcept = default;

		bc_network_client_manager::~bc_network_client_manager() = default;

		bc_network_client_manager& bc_network_client_manager::operator=(bc_network_client_manager&&) noexcept = default;

		void bc_network_client_manager::add_actor(bc_actor& p_actor)
		{
			m_sync_actors.push_back(p_actor);
		}

		void bc_network_client_manager::remove_actor(bc_actor& p_actor)
		{
			const auto l_ite = std::find_if(std::cbegin(m_sync_actors), std::cend(m_sync_actors), [&](bc_actor& p_entry)
			{
				return p_actor == p_entry;
			});
			if(l_ite != std::end(m_sync_actors))
			{
				m_sync_actors.erase(l_ite);
			}
		}

		void bc_network_client_manager::send_command(bc_network_command_ptr p_command)
		{
			m_commands.push_back(p_command);
		}

		void bc_network_client_manager::update(const core_platform::bc_clock::update_param& p_clock)
		{
			bc_client_socket_state_machine::process_event
			(
				bc_socket_update_event{ p_clock }
			);

			if(p_clock.m_total_elapsed - m_last_rtt_time < m_rtt_time.average_value())
			{
				return;
			}
		}

		void bc_network_client_manager::on_enter(const bc_socket_error_state& p_state)
		{
			m_commands.clear();
			m_hook->error_occurred(p_state.get_last_exception());
		}

		void bc_network_client_manager::on_enter(const bc_socket_connecting_state& p_state)
		{
			m_hook->connecting_to_server(m_ip, m_port);
		}

		void bc_network_client_manager::on_enter(const bc_socket_connected_state& p_state)
		{
			m_commands.clear();
			m_hook->connected_to_server(m_ip, m_port);
		}

		void bc_network_client_manager::on_enter(const bc_socket_sending_state& p_state)
		{
		}

		void bc_network_client_manager::on_enter(const bc_socket_receiving_state& p_state)
		{
		}

		void bc_network_client_manager::on_exit(const bc_socket_error_state& p_state)
		{
		}

		void bc_network_client_manager::on_exit(const bc_socket_connecting_state& p_state)
		{
		}

		void bc_network_client_manager::on_exit(const bc_socket_connected_state& p_state)
		{
		}

		void bc_network_client_manager::on_exit(const bc_socket_sending_state& p_state)
		{
		}

		void bc_network_client_manager::on_exit(const bc_socket_receiving_state& p_state)
		{
		}
	}	
}