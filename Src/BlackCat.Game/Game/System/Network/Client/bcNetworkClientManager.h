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
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "Game/System/Network/Client/bcNetworkClientManagerHook.h"
#include "Game/System/Network/Client/bcClientSocketStateMachine.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_game_system;
		class bc_network_system;

		class bc_network_client_manager_visitor : private bci_network_message_deserialization_visitor, private bci_network_message_client_visitor
		{
		private:
			bc_actor create_actor(const bcCHAR* p_entity_name) override final
			{
				return visitor_create_actor(p_entity_name);
			}

			bc_actor get_actor(bc_actor_network_id p_actor_network_id) override final
			{
				return visitor_get_actor(p_actor_network_id);
			}
			
			void connection_approved() override final
			{
				visitor_connection_approved();
			}

			void acknowledge_message(bc_network_message_id p_message_id) override final
			{
				visitor_acknowledge_message(p_message_id);
			}

			void replicate_actor(bc_actor& p_actor) override final
			{
				visitor_replicate_actor(p_actor);
			}

			void remove_actor(bc_actor& p_actor) override final
			{
				visitor_remove_actor(p_actor);
			}

			virtual bc_actor visitor_create_actor(const bcCHAR* p_entity_name) = 0;

			virtual bc_actor visitor_get_actor(bc_actor_network_id p_actor_network_id) = 0;

			virtual void visitor_connection_approved() = 0;

			virtual void visitor_acknowledge_message(bc_network_message_id p_message_id) = 0;

			virtual void visitor_replicate_actor(bc_actor& p_actor) = 0;

			virtual void visitor_remove_actor(bc_actor& p_actor) = 0;
		};
		
		class BC_GAME_DLL bc_network_client_manager : public bci_network_manager,
				private bc_network_client_manager_visitor,
				private bc_client_socket_state_machine
		{
		public:
			using bci_network_manager::send_message;
			
		public:
			bc_network_client_manager(bc_game_system& p_game_system,
				bc_network_system& p_network_system, 
				bci_network_client_manager_hook& p_hook, 
				const platform::bc_network_address& p_address);

			bc_network_client_manager(bc_network_client_manager&&) noexcept;

			~bc_network_client_manager() override;

			bc_network_client_manager& operator=(bc_network_client_manager&&) noexcept;

			void add_actor(bc_actor& p_actor) override;
			
			void remove_actor(bc_actor& p_actor) override;
			
			void send_message(bc_network_message_ptr p_message) override;
			
			void update(const bc_network_manager_update_context& p_context) override;
			
		private:
			void on_enter(bc_client_socket_error_state& p_state) override;
			
			void on_enter(bc_client_socket_connecting_state& p_state) override;
			
			void on_enter(bc_client_socket_connected_state& p_state) override;

			void on_enter(bc_client_socket_sending_state& p_state) override;

			void visitor_connection_approved() override;
			
			void visitor_acknowledge_message(bc_network_message_id p_message_id) override;

			void visitor_replicate_actor(bc_actor& p_actor) override;
			
			bc_actor visitor_create_actor(const bcCHAR* p_entity_name) override;

			bc_actor visitor_get_actor(bc_actor_network_id p_actor_network_id) override;
			
			void _retry_messages_with_acknowledgment(bc_network_packet_time p_current_time);
			
			void _send_to_server();
			
			void _receive_from_server();

			bc_game_system* m_game_system;
			
			bool m_socket_is_connected;
			bool m_socket_is_ready;
			platform::bc_network_address m_address;
			core::bc_unique_ptr<platform::bc_non_block_socket> m_socket;
			bci_network_client_manager_hook* m_hook;
			bc_network_packet_time m_last_sync_time;
			core::bc_value_sampler<bc_network_packet_time, 64> m_rtt_sampler;

			core::bc_mutex_test m_actors_lock;
			core::bc_vector<bc_actor> m_sync_actors;
			core::bc_unordered_map<bc_actor_network_id, bc_actor> m_network_actors;

			core_platform::bc_mutex m_messages_lock;
			bc_network_message_id m_last_executed_message_id;
			core::bc_vector<bc_network_message_ptr> m_messages;
			core::bc_vector<bc_message_with_time> m_messages_waiting_acknowledgment;
			core::bc_memory_stream m_memory_buffer;
			bc_network_message_buffer m_messages_buffer;
		};
	}
}