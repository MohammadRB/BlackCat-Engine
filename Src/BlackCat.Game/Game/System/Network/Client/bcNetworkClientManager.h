// [05/27/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "PlatformImp/Network/bcNonBlockSocket.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Math/bcValueSampler.h"
#include "Core/Concurrency/bcMutexTest.h"
#include "Core/File/bcMemoryStream.h"
#include "PlatformImp/Network/bcNetworkAddress.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/System/Network/bcNetworkManager.h"
#include "Game/System/Network/bcNetworkMessageBuffer.h"
#include "Game/System/Network/Client/bcNetworkClientManagerHook.h"
#include "Game/System/Network/Client/bcClientSocketStateMachine.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_network_system;
		
		class BC_GAME_DLL bc_network_client_manager : public bci_network_manager, private bc_client_socket_state_machine
		{
		public:
			using bci_network_manager::send_message;
			
		public:
			bc_network_client_manager(bc_network_system& p_network_system, bci_network_client_manager_hook& p_hook, const platform::bc_network_address& p_address);

			bc_network_client_manager(bc_network_client_manager&&) noexcept;

			~bc_network_client_manager() override;

			bc_network_client_manager& operator=(bc_network_client_manager&&) noexcept;

			void add_actor(bc_actor& p_actor) override;
			
			void remove_actor(bc_actor& p_actor) override;
						
			void send_message(bc_network_message_ptr p_message) override;
			
			void acknowledge_message(bc_network_message_id p_message_id);
			
			void update(const core_platform::bc_clock::update_param& p_clock) override;

			void connection_approved();
			
			void actor_replicated(bc_actor& p_actor);
			
		private:
			void on_enter(bc_client_socket_error_state& p_state) override;
			
			void on_enter(bc_client_socket_connecting_state& p_state) override;
			
			void on_enter(bc_client_socket_connected_state& p_state) override;

			void on_enter(bc_client_socket_sending_state& p_state) override;

			void _retry_messages_with_acknowledgment(bc_network_packet_time p_current_time);
			
			void _send_to_server();
			
			void _receive_from_server();

			platform::bc_network_address m_address;
			bool m_socket_is_connected;
			bool m_socket_is_ready;
			core::bc_unique_ptr<platform::bc_non_block_socket> m_socket;
			bci_network_client_manager_hook* m_hook;
			bc_network_packet_time m_last_sync_time;
			core::bc_value_sampler<bc_network_packet_time, 64> m_rtt_sampler;

			core::bc_mutex_test m_actors_lock;
			core::bc_vector<bc_actor> m_sync_actors;
			core::bc_unordered_map<bc_actor_network_id, bc_actor> m_network_actors;

			core_platform::bc_mutex m_messages_lock;
			core::bc_vector<bc_network_message_ptr> m_messages;
			core::bc_vector<bc_message_with_time> m_messages_waiting_acknowledgment;
			core::bc_memory_stream m_memory_buffer;
			bc_network_message_buffer m_messages_buffer;
		};
	}	
}