// [06/10/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcSpan.h"
#include "Core/File/bcMemoryStream.h"
#include "Core/Math/bcValueSampler.h"
#include "PlatformImp/Network/bcNonBlockSocket.h"
#include "Game/System/Network/bcNetworkMessageBuffer.h"
#include "Game/System/Network/Server/bcServerTcpClientSocketStateMachine.h"

namespace black_cat
{
	namespace game
	{
		class bc_network_server_manager;
		
		class bc_network_server_manager_tcp_client : private bc_server_tcp_client_socket_state_machine
		{
		public:
			bc_network_server_manager_tcp_client(bc_network_server_manager& p_manager, platform::bc_non_block_socket p_socket);

			bc_network_server_manager_tcp_client(bc_network_server_manager_tcp_client&&) noexcept;

			~bc_network_server_manager_tcp_client();

			bc_network_server_manager_tcp_client& operator=(bc_network_server_manager_tcp_client&&) noexcept;

			bool get_socket_is_connected() const noexcept;

			bool get_socket_is_ready() const noexcept;
			
			bc_network_packet_time get_last_sync_time() const noexcept;

			void set_last_sync_time(bc_network_packet_time p_time) noexcept;

			bc_network_packet_time get_rtt_time() const noexcept;

			void add_rtt_time(bc_network_packet_time p_time) noexcept;

			void add_message(bc_network_message_ptr p_message);

			core::bc_span<bc_network_message_ptr> get_messages() noexcept;

			void clear_messages() noexcept;
			
			void add_message_with_acknowledgment(bc_network_message_ptr p_message) noexcept;
			
			core::bc_span<bc_network_message_ptr> get_messages_waiting_acknowledgment() noexcept;
			
			bcSIZE send(core::bc_memory_stream& p_stream, bcSIZE p_bytes_to_send) noexcept;

			bcSIZE receive(core::bc_memory_stream& p_stream) noexcept;

			void update(const core_platform::bc_clock::update_param& p_clock);

		private:
			void on_enter(bc_server_tcp_client_socket_connected_state& p_state) override;
			
			void on_enter(bc_server_tcp_client_socket_sending_state& p_state) override;

			void on_enter(bc_server_tcp_client_socket_error_state& p_state) override;

			bc_network_server_manager* m_manager;
			bool m_socket_is_connected;
			bool m_socket_is_ready;
			core::bc_unique_ptr<platform::bc_non_block_socket> m_socket;
			bc_network_packet_time m_last_sync_time;
			core::bc_value_sampler<bc_network_packet_time, 64> m_rtt_sampler;

			core::bc_vector<bc_network_message_ptr> m_messages;
			core::bc_vector<bc_network_message_ptr> m_messages_waiting_acknowledgment;
		};

		inline bc_network_server_manager_tcp_client::bc_network_server_manager_tcp_client(bc_network_server_manager_tcp_client&&) noexcept = default;

		inline bc_network_server_manager_tcp_client::~bc_network_server_manager_tcp_client() = default;

		inline bc_network_server_manager_tcp_client& bc_network_server_manager_tcp_client::operator=(bc_network_server_manager_tcp_client&&) noexcept = default;
		
		inline bool bc_network_server_manager_tcp_client::get_socket_is_connected() const noexcept
		{
			return m_socket_is_connected;
		}

		inline bool bc_network_server_manager_tcp_client::get_socket_is_ready() const noexcept
		{
			return m_socket_is_ready;
		}

		inline bc_network_packet_time bc_network_server_manager_tcp_client::get_last_sync_time() const noexcept
		{
			return m_last_sync_time;
		}

		inline void bc_network_server_manager_tcp_client::set_last_sync_time(bc_network_packet_time p_time) noexcept
		{
			m_last_sync_time = p_time;
		}
		
		inline bc_network_packet_time bc_network_server_manager_tcp_client::get_rtt_time() const noexcept
		{
			return m_rtt_sampler.average_value();
		}

		inline void bc_network_server_manager_tcp_client::add_rtt_time(bc_network_packet_time p_time) noexcept
		{
			m_rtt_sampler.add_sample(p_time);
		}

		inline void bc_network_server_manager_tcp_client::add_message(bc_network_message_ptr p_message)
		{
			m_messages.push_back(std::move(p_message));
		}

		inline core::bc_span<bc_network_message_ptr> bc_network_server_manager_tcp_client::get_messages() noexcept
		{
			return core::bc_make_span(m_messages);
		}

		inline void bc_network_server_manager_tcp_client::clear_messages() noexcept
		{
			m_messages.clear();
		}

		inline void bc_network_server_manager_tcp_client::add_message_with_acknowledgment(bc_network_message_ptr p_message) noexcept
		{
			m_messages_waiting_acknowledgment.push_back(std::move(p_message));
		}
		
		inline core::bc_span<bc_network_message_ptr> bc_network_server_manager_tcp_client::get_messages_waiting_acknowledgment() noexcept
		{
			return core::bc_make_span(m_messages_waiting_acknowledgment);
		}

		inline bcSIZE bc_network_server_manager_tcp_client::send(core::bc_memory_stream& p_stream, bcSIZE p_bytes_to_send) noexcept
		{
			bc_server_tcp_client_socket_send_event l_send_event{ p_stream, p_bytes_to_send, 0 };
			bc_server_tcp_client_socket_state_machine::process_event(l_send_event);

			return l_send_event.m_bytes_sent;
		}

		inline bcSIZE bc_network_server_manager_tcp_client::receive(core::bc_memory_stream& p_stream) noexcept
		{
			bc_server_tcp_client_socket_receive_event l_receive_event{ p_stream, 0 };
			bc_server_tcp_client_socket_state_machine::process_event(l_receive_event);

			return l_receive_event.m_bytes_received;
		}

		inline void bc_network_server_manager_tcp_client::update(const core_platform::bc_clock::update_param& p_clock)
		{
			bc_server_tcp_client_socket_update_event l_update_event{ p_clock };
			bc_server_tcp_client_socket_state_machine::process_event(l_update_event);
		}
	}	
}