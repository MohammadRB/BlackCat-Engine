// [27/05/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "PlatformImp/Network/bcNonBlockSocket.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Concurrency/bcMutexTest.h"
#include "Core/File/bcMemoryStream.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "Core/Utility/bcValueSampler.h"
#include "PlatformImp/Network/bcNetworkAddress.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/System/Network/bcNetworkManager.h"
#include "Game/System/Network/bcNetworkMessageSerializationBuffer.h"
#include "Game/System/Network/bcNetworkMessageAcknowledgeBuffer.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "Game/System/Network/Client/bcNetworkClientManagerHook.h"
#include "Game/System/Network/Client/bcClientSocketStateMachine.h"
#include "Game/System/Network/Server/bcNetworkClient.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_game_system;
	class bc_network_system;

	class BC_GAME_DLL bc_network_client_manager : public bci_network_manager,
	                                              private bci_network_message_serialization_visitor,
	                                              private bci_network_message_deserialization_visitor,
	                                              private bci_network_message_client_visitor,
	                                              private bc_client_socket_state_machine
	{
	public:
		using bci_network_manager::send_message;
			
	public:
		bc_network_client_manager(bc_game_system& p_game_system,
		                          bc_network_system& p_network_system, 
		                          bci_network_client_manager_hook& p_hook,
		                          bci_network_message_visitor& p_message_visitor,
		                          const platform::bc_network_address& p_server_address,
		                          bcUINT32 p_timeout_ms);

		bc_network_client_manager(bc_network_client_manager&&) noexcept;

		~bc_network_client_manager() override;

		bc_network_client_manager& operator=(bc_network_client_manager&&) noexcept;

		bc_network_type get_network_type() const noexcept override;

		bc_network_state get_network_state() const noexcept override;

		const platform::bc_network_address& get_server_address() const noexcept;
			
		void add_actor_to_sync(bc_actor& p_actor) override;
			
		void remove_actor_from_sync(bc_actor& p_actor) override;

		void actor_removed(bc_actor& p_actor) override;
			
		void send_message(bc_network_message_ptr p_message) override;
			
		void update(const bc_network_manager_update_context& p_context) override;
			
	private:
		// State machine methods
			
		void on_enter(bc_client_socket_error_state& p_state) override;
			
		void on_enter(bc_client_socket_connecting_state& p_state) override;
			
		void on_enter(bc_client_socket_connected_state& p_state) override;
			
		// Client visitor methods

		void get_rtt_time(bc_network_rtt* p_rtt, bc_network_rtt* p_remote_rtt) noexcept override;
			
		void add_rtt_sample(bc_network_rtt p_rtt, bc_network_rtt p_remote_rtt) noexcept override;
			
		void connection_approved(bc_client_connect_result p_result) override;
			
		void acknowledge_message(bc_network_message_id p_ack_id, core::bc_string p_ack_data) override;

		void load_scene(const bcECHAR* p_scene_name) override;
			
		void replicate_actor(bc_actor& p_actor) override;

		void remove_actor(bc_actor& p_actor) override;

		// Deserialization visitor methods
			
		bc_actor create_actor(const bcCHAR* p_entity_name, const core::bc_matrix4f& p_transform) override;

		bc_replicated_actor get_actor(bc_actor_network_id p_actor_network_id) override;

		// Private methods

		void _test_connectivity(const platform::bc_clock::update_param& p_clock);

		void _retry_messages_waiting_acknowledgment(const platform::bc_clock::update_param& p_clock);
			
		void _send_to_server(const platform::bc_clock::update_param& p_clock);
			
		void _receive_from_server();

		void _event_handler(core::bci_event& p_event);

		bc_game_system* m_game_system;
		bci_network_client_manager_hook* m_hook;
		bci_network_message_visitor* m_message_visitor;
		platform::bc_network_address m_server_address;
		bcUINT32 m_timeout_ms;

		core::bc_unique_ptr<platform::bc_non_block_socket> m_socket;
		bool m_socket_is_connected;
		bool m_socket_is_ready;
		bcUINT32 m_timeout_elapsed;
		bc_network_client_id m_my_client_id;
		bc_network_packet_time m_last_sync_time;
		core::bc_value_sampler<bc_network_rtt, 16> m_rtt_sampler;
		bc_network_rtt m_remote_rtt;
		core::bc_string_view m_client_name;

		core::bc_mutex_test m_actors_lock;
		core::bc_vector<bc_actor> m_sync_actors;
		core::bc_unordered_map<bc_actor_network_id, bc_actor> m_network_actors;

		platform::bc_mutex m_messages_lock;
		bc_network_message_id m_last_executed_message_id;
		core::bc_vector<bc_network_message_ptr> m_messages;
		core::bc_vector<bc_retry_message> m_messages_waiting_acknowledgment;
		bc_network_message_acknowledge_buffer m_executed_messages;

		core::bc_memory_stream m_memory_buffer;
		bc_network_message_serialization_buffer m_messages_buffer;

		core::bc_event_listener_handle m_scene_change_handle;
		core::bc_event_listener_handle m_config_change_handle;
	};
}
