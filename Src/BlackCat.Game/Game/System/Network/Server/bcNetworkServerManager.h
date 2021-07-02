// [05/27/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Math/bcValueSampler.h"
#include "Core/File/bcMemoryStream.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/System/Network/bcNetworkManager.h"
#include "Game/System/Network/bcNetworkMessageBuffer.h"
#include "Game/System/Network/Server/bcNetworkServerManagerHook.h"
#include "Game/System/Network/Server/bcServerSocket1StateMachine.h"
#include "Game/System/Network/Server/bcNetworkServerManagerClient1.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_network_system;
		
		class BC_GAME_DLL bc_network_server_manager : public bci_network_manager, private bc_server_socket1_state_machine
		{
		public:
			bc_network_server_manager(bc_network_system& p_network_system, bci_network_server_manager_hook& p_hook, bcUINT16 p_port);

			bc_network_server_manager(bc_network_server_manager&&) noexcept;

			~bc_network_server_manager() override;

			bc_network_server_manager& operator=(bc_network_server_manager&&) noexcept;

			void add_actor(bc_actor& p_actor) override;
			
			void remove_actor(bc_actor& p_actor) override;

			void send_message(bc_network_message_ptr p_message) override;
			
			void update(const core_platform::bc_clock::update_param& p_clock) override;

			void client_disconnected(bc_network_server_manager_client1& p_client);
			
		private:
			void on_enter(bc_server_socket1_error_state& p_state) override;
			
			void on_enter(bc_server_socket1_listening_state& p_state) override;
			
			void _send_to_client(bc_network_server_manager_client1& p_client);

			void _receive_from_clients();
			
			bcUINT16 m_port;
			bool m_socket_is_listening;
			core::bc_unique_ptr<platform::bc_non_block_socket> m_socket;
			core_platform::bc_mutex m_clients_lock;
			core::bc_vector<bc_network_server_manager_client1> m_clients;
			bci_network_server_manager_hook* m_hook;

			core_platform::bc_mutex m_actors_lock;
			core_platform::bc_atomic<bc_actor_network_id> m_actor_network_id_counter;
			core::bc_vector<bc_actor> m_new_actors;
			core::bc_unordered_map<bc_actor_network_id, bc_actor> m_network_actors;
			
			core::bc_memory_stream m_memory_buffer;
			bc_network_message_buffer m_messages_buffer;
		};
	}
}