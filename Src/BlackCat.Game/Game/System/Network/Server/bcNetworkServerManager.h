// [05/27/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Concurrency/bcMutexTest.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Math/bcValueSampler.h"
#include "Core/File/bcMemoryStream.h"
#include "PlatformImp/Network/bcNetworkAddress.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/System/Network/bcNetworkManager.h"
#include "Game/System/Network/bcNetworkMessageBuffer.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "Game/System/Network/Server/bcNetworkServerManagerHook.h"
#include "Game/System/Network/Server/bcServerSocketStateMachine.h"
#include "Game/System/Network/Server/bcNetworkServerManagerClient.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_game_system;
		class bc_network_system;

		class BC_GAME_DLL bc_network_server_manager : public bci_network_manager,
				public bci_network_message_deserialization_bridge,
				public bci_network_message_server_bridge,
				private bc_server_socket_state_machine
		{
		public:
			using bci_network_manager::send_message;
			
		public:
			bc_network_server_manager(bc_game_system& p_game_system, bc_network_system& p_network_system, bci_network_server_manager_hook& p_hook, bcUINT16 p_port);

			bc_network_server_manager(bc_network_server_manager&&) noexcept;

			~bc_network_server_manager() override;

			bc_network_server_manager& operator=(bc_network_server_manager&&) noexcept;

			void add_actor(bc_actor& p_actor) override;
			
			void remove_actor(bc_actor& p_actor) override;

			void send_message(bc_network_message_ptr p_message) override;
			
			void update(const bc_network_manager_update_context& p_context) override;
			
		private:
			void on_enter(bc_server_socket_error_state& p_state) override;
			
			void on_enter(bc_server_socket_listening_state& p_state) override;

			void client_connected(const platform::bc_network_address& p_address) override;

			void client_disconnected(const platform::bc_network_address& p_address) override;

			void acknowledge_message(const platform::bc_network_address& p_address, bc_network_message_id p_message_id) override;
			
			bc_actor create_actor(const bcCHAR* p_entity_name, const core::bc_json_key_value& p_params) override;
			
			void _retry_messages_with_acknowledgment(bc_network_packet_time p_current_time, bc_network_server_manager_client& p_client);
			
			void _send_to_client(bc_network_server_manager_client& p_client);

			void _receive_from_clients();

			bc_network_server_manager_client* _find_client(const platform::bc_network_address& p_address);

			bc_game_system* m_game_system;
			
			bcUINT16 m_port;
			bool m_socket_is_listening;
			core::bc_unique_ptr<platform::bc_non_block_socket> m_socket;
			core::bc_mutex_test m_clients_lock;
			core::bc_vector<bc_network_server_manager_client> m_clients;
			bci_network_server_manager_hook* m_hook;

			core_platform::bc_mutex m_actors_lock;
			core_platform::bc_atomic<bc_actor_network_id> m_actor_network_id_counter;
			core::bc_unordered_map<bc_actor_network_id, bc_actor> m_network_actors;
			
			core::bc_memory_stream m_memory_buffer;
			bc_network_message_buffer m_messages_buffer;
		};
	}
}