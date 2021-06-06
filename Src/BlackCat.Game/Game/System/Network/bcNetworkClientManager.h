// [05/27/2021 MRB]

#pragma once

#include "PlatformImp/Network/bcNonBlockSocket.h"
#include "Core/Container/bcVector.h"
#include "Core/Math/bcValueSampler.h"
#include "Game/System/Network/bcNetworkManager.h"
#include "Game/System/Network/bcClientSocketStateMachine.h"
#include "Game/System/Network/bcNetworkCommandPacket.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_network_system;
		class bci_network_client_manager_hook;
		
		class BC_GAME_DLL bc_network_client_manager : public bci_network_manager, private bc_client_socket_state_machine
		{
		public:
			bc_network_client_manager(bc_network_system& p_network_system, bci_network_client_manager_hook& p_hook, const bcCHAR* p_ip, bcUINT16 p_port);

			bc_network_client_manager(bc_network_client_manager&&) noexcept;

			~bc_network_client_manager() override;

			bc_network_client_manager& operator=(bc_network_client_manager&&) noexcept;

			void add_actor(bc_actor& p_actor) override;
			
			void remove_actor(bc_actor& p_actor) override;

			void send_command(bc_network_command_ptr p_command) override;
			
			void update(const core_platform::bc_clock::update_param& p_clock) override;

		private:
			void on_enter(const bc_socket_error_state& p_state) override;
			
			void on_enter(const bc_socket_connecting_state& p_state) override;
			
			void on_enter(const bc_socket_connected_state& p_state) override;
			
			void on_enter(const bc_socket_sending_state& p_state) override;
			
			void on_enter(const bc_socket_receiving_state& p_state) override;
			
			void on_exit(const bc_socket_error_state& p_state) override;
			
			void on_exit(const bc_socket_connecting_state& p_state) override;
			
			void on_exit(const bc_socket_connected_state& p_state) override;
			
			void on_exit(const bc_socket_sending_state& p_state) override;
			
			void on_exit(const bc_socket_receiving_state& p_state) override;

			const bcCHAR* m_ip;
			bcUINT16 m_port;
			platform::bc_non_block_socket m_socket;
			bc_network_packet_time m_last_rtt_time;
			core::bc_value_sampler<bc_network_packet_time, 64> m_rtt_time;
			bci_network_client_manager_hook* m_hook;
			
			core::bc_vector<bc_actor> m_sync_actors;
			core::bc_vector<bc_network_command_ptr> m_commands;
			bc_network_command_packet m_commands_packet;
		};
	}	
}