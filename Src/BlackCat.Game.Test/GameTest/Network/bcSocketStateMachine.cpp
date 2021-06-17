// [06/13/2021 MRB]

#include "GameTest/GameTestPCH.h"

#include <future>
#include <atomic>
#include "Core/Container/bcVector.h"
#include "Game/System/Network/Client/bcClientSocketStateMachine.h"
#include "Game/System/Network/Server/bcServerSocketStateMachine.h"
#include "Game/System/Network/Server/bcServerClientSocketStateMachine.h"
#include "GameTest/bcTestFixture.h"

namespace black_cat
{
	namespace game_test
	{
		class bc_server_client_state : public game::bc_server_client_socket_state_machine
		{
		public:
			bc_server_client_state(platform::bc_non_block_socket p_socket)
				: bc_server_client_socket_state_machine(m_socket),
				m_socket(std::move(p_socket))
			{
			}

			platform::bc_non_block_socket m_socket;
		
		private:
			void on_enter(game::bc_server_client_socket_connected_state& p_state) override
			{
			}
			
			void on_enter(game::bc_server_client_socket_sending_state& p_state) override
			{
			}
			
			void on_enter(game::bc_server_client_socket_error_state& p_state) override
			{
				FAIL() << "server client socket went to error state. " << p_state.get_last_exception()->get_full_message();;
			}
		};
		
		class bc_server_state : public game::bc_server_socket_state_machine
		{
		public:
			bc_server_state()
				: bc_server_socket_state_machine(m_socket),
				m_socket(platform::bc_socket_address::inter_network, platform::bc_socket_type::data_gram, platform::bc_socket_protocol::udp)
			{
			}

			platform::bc_non_block_socket m_socket;
			core::bc_vector<bc_server_client_state> m_clients;
		
		private:
			void on_enter(game::bc_server_socket_error_state& p_state) override
			{
				FAIL() << "server socket went to error state. " << p_state.get_last_exception()->get_full_message();
			}
			
			void on_enter(game::bc_server_socket_listening_state& p_state) override
			{
			}
			
			void on_enter(game::bc_server_socket_accepting_state& p_state) override
			{
				m_clients.push_back(bc_server_client_state(p_state.get_client_socket()));
			}
		};

		class bc_client_state : public game::bc_client_socket_state_machine
		{
		public:
			bc_client_state()
				: bc_client_socket_state_machine(m_socket),
				m_socket(platform::bc_socket_address::inter_network, platform::bc_socket_type::data_gram, platform::bc_socket_protocol::udp),
				m_connected(false)
			{
			}

			platform::bc_non_block_socket m_socket;
			bool m_connected;
		
		private:
			void on_enter(game::bc_client_socket_error_state& p_state) override
			{
				FAIL() << "client socket went to error state. " << p_state.get_last_exception()->get_full_message();
			}
			
			void on_enter(game::bc_client_socket_connecting_state& p_state) override
			{
			}
			
			void on_enter(game::bc_client_socket_connected_state& p_state) override
			{
				m_connected = true;
			}
			
			void on_enter(game::bc_client_socket_sending_state& p_state) override
			{
			}
		};
		
		TEST(SocketStateMachine, BindingAccepting)
		{
			bc_test_init();
			bc_test_init_socket_library();

			std::atomic<bool> l_terminate(false);
			std::atomic<bool> l_server_ran(false);
			
			auto l_server_future = std::async
			(
				std::launch::async, 
				[&]()
				{
					bc_server_state l_server;

					game::bc_server_socket_bind_event l_bind_event{ 7050 };
					l_server.process_event(l_bind_event);

					l_server_ran.store(true);
					
					while (!l_terminate.load())
					{
						game::bc_server_socket_update_event l_update_event{ core_platform::bc_clock::update_param{0,0} };
						l_server.process_event(l_update_event);
					}
				}
			);

			while (!l_server_ran.load());
			
			auto l_client_future = std::async
			(
				std::launch::async,
				[&]()
				{
					bc_client_state l_client;

					game::bc_client_socket_connect_event l_connection_event{ "127.0.0.1", 7050 };
					l_client.process_event(l_connection_event);
					
					while (!l_terminate.load() && !l_client.m_connected)
					{
						game::bc_client_socket_update_event l_update_event{ core_platform::bc_clock::update_param{0,0} };
						l_client.process_event(l_update_event);
					}

					l_terminate.store(true);
				}
			);

			l_client_future.wait();
			l_server_future.wait();
			
			bc_test_close_socket_library();
			bc_test_close();
		}
	}	
}