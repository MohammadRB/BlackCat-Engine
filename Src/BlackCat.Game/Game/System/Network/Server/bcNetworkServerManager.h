// [05/27/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Concurrency/bcMutexTest.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/File/bcMemoryStream.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "PlatformImp/Network/bcNetworkAddress.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/System/Network/bcNetworkManager.h"
#include "Game/System/Network/bcNetworkMessageSerializationBuffer.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "Game/System/Network/Server/bcNetworkServerManagerHook.h"
#include "Game/System/Network/Server/bcServerSocketStateMachine.h"
#include "Game/System/Network/Server/bcNetworkServerManagerClient.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_event_manager;
	}
	
	namespace game
	{
		class bc_game_system;
		class bc_network_system;

		class BC_GAME_DLL bc_network_server_manager : public bci_network_manager,
				private bci_network_message_serialization_visitor,
				private bci_network_message_deserialization_visitor,
				private bci_network_message_server_visitor,
				private bc_server_socket_state_machine
		{
		public:
			using bci_network_manager::send_message;
			
		public:
			bc_network_server_manager(core::bc_event_manager& p_event_manager, 
				bc_game_system& p_game_system, 
				bc_network_system& p_network_system, 
				bci_network_server_manager_hook& p_hook, 
				bcUINT16 p_port);

			bc_network_server_manager(bc_network_server_manager&&) noexcept;

			~bc_network_server_manager() override;

			bc_network_server_manager& operator=(bc_network_server_manager&&) noexcept;

			bc_network_type get_network_type() const noexcept override;
			
			void add_actor_to_sync(bc_actor& p_actor) override;
			
			void remove_actor_from_sync(bc_actor& p_actor) override;

			void send_message(bc_network_message_ptr p_message) override;
			
			void update(const bc_network_manager_update_context& p_context) override;
			
		private:
			// State machine methods
			
			void on_enter(bc_server_socket_error_state& p_state) override;
			
			void on_enter(bc_server_socket_listening_state& p_state) override;

			// Server visitor methods
			
			void client_connected(const platform::bc_network_address& p_address) override;

			void client_disconnected(const platform::bc_network_address& p_address) override;

			void acknowledge_message(const platform::bc_network_address& p_address, bc_network_message_id p_ack_id, core::bc_string p_ack_data) override;

			void replicate_scene(const platform::bc_network_address& p_address) override;

			void replicate_actor(const platform::bc_network_address& p_address, bc_actor& p_actor) override;

			void remove_actor(const platform::bc_network_address& p_address, bc_actor& p_actor) override;
			
			// Deserialization visitor methods
			
			bc_actor create_actor(const bcCHAR* p_entity_name) override;

			bc_replicated_actor get_actor(bc_actor_network_id p_actor_network_id) override;

			// Private methods
			
			void _add_message_to_clients(bc_network_message_ptr p_message, const platform::bc_network_address* p_exclude_client = nullptr);
			
			void _retry_messages_waiting_acknowledgment(bc_network_packet_time p_current_time, const core_platform::bc_clock::update_param& p_clock, bc_network_server_manager_client& p_client);
			
			void _send_to_client(const core_platform::bc_clock::update_param& p_clock, bc_network_server_manager_client& p_client);

			bcSIZE _receive_from_clients();

			bc_network_server_manager_client* _check_if_contains_client_connect_message(platform::bc_network_address& p_address, core::bc_span<bc_network_message_ptr> p_messages);
			
			bc_network_server_manager_client* _find_client(const platform::bc_network_address& p_address);

			bool _event_handler(core::bci_event& p_event);

			core::bc_event_manager* m_event_manager;
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
			bc_network_message_serialization_buffer m_messages_buffer;

			core::bc_event_listener_handle m_scene_change_event;
			core::bc_string m_scene_name;
		};
	}
}