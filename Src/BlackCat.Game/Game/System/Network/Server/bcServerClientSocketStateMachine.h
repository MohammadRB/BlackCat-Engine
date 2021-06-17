// [06/10/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcStateMachine.h"
#include "Core/Container/bcVector.h"
#include "Core/File/bcMemoryStream.h"
#include "Core/Utility/bcNullable.h"
#include "Core/Utility/bcLogger.h"
#include "PlatformImp/Network/bcNonBlockSocket.h"

namespace black_cat
{
	namespace game
	{
		class bc_server_client_socket_state_machine;
		class bc_server_client_socket_error_state;
		class bc_server_client_socket_connected_state;
		class bc_server_client_socket_sending_state;

		struct bc_server_client_socket_update_event
		{
			core_platform::bc_clock::update_param m_clock;
		};

		struct bc_server_client_socket_send_event
		{
			core::bc_memory_stream& m_stream;
			bcSIZE m_bytes_to_send;
			bcSIZE m_bytes_sent;
		};

		struct bc_server_client_socket_receive_event
		{
			core::bc_memory_stream& m_stream;
			bcSIZE m_bytes_received;
		};

		struct bc_server_client_socket_disconnect_event
		{
		};

		class bc_server_client_socket_error_state : public core::bc_state<bc_server_client_socket_state_machine>
		{
		public:
			bc_server_client_socket_error_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

			const bc_network_exception* get_last_exception() const noexcept
			{
				return m_last_exception.get();
			}

			void set_last_exception(const bc_network_exception& p_exception)
			{
				m_last_exception.reset(p_exception);
			}

		private:
			void on_enter() override
			{
				if (m_last_exception.has_value())
				{
					const auto l_str = core::bc_to_estring_frame(m_last_exception->get_full_message());
					core::bc_log(core::bc_log_type::error, l_str.c_str());
				}
			}

			void on_exit() override
			{
			}

			platform::bc_non_block_socket* m_socket;
			core::bc_nullable<bc_network_exception> m_last_exception;
		};

		class bc_server_client_socket_connected_state : public core::bc_state<bc_server_client_socket_state_machine, bc_server_client_socket_send_event, bc_server_client_socket_receive_event>
		{
		public:
			bc_server_client_socket_connected_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(bc_server_client_socket_send_event& p_event) override
			{
				if (!m_socket->is_send_available())
				{
					p_event.m_bytes_sent = 0;
					return state_transition::empty();
				}

				try
				{
					p_event.m_bytes_sent = m_socket->send(p_event.m_stream.get_position_data(), p_event.m_bytes_to_send);
					return state_transition::transfer_to<bc_server_client_socket_sending_state>();
				}
				catch (const bc_network_exception & p_exception)
				{
					return state_transition::transfer_to<bc_server_client_socket_error_state>
					(
						[p_exception](bc_server_client_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}

				return state_transition::empty();
			}

			state_transition handle(bc_server_client_socket_receive_event& p_event) override
			{
				if (!m_socket->is_receive_available())
				{
					return state_transition::empty();
				}

				try
				{
					constexpr auto l_local_buffer_size = 100;
					bcBYTE l_buffer[l_local_buffer_size];

					while (true)
					{
						const auto l_bytes_received = m_socket->receive(l_buffer, l_local_buffer_size);
						if (!l_bytes_received)
						{
							break;
						}

						p_event.m_stream.write(l_buffer, l_local_buffer_size);
						p_event.m_bytes_received += l_bytes_received;
					}
				}
				catch (const bc_network_exception & p_exception)
				{
					return state_transition::transfer_to<bc_server_client_socket_error_state>
					(
						[p_exception](bc_server_client_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}

				return state_transition::empty();
			}
			
			platform::bc_non_block_socket* m_socket;
		};

		class bc_server_client_socket_sending_state : public core::bc_state<bc_server_client_socket_state_machine, bc_server_client_socket_update_event>
		{
		public:
			bc_server_client_socket_sending_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(bc_server_client_socket_update_event& p_event) override
			{
				try
				{
					const bool l_is_available = m_socket->is_send_available();
					if (l_is_available)
					{
						return state_transition::transfer_to<bc_server_client_socket_connected_state>();
					}
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_server_client_socket_error_state>
					(
						[p_exception](bc_server_client_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}

				return state_transition::empty();
			}
			
			platform::bc_non_block_socket* m_socket;
		};

		class bc_server_client_socket_state_machine : public core::bc_state_machine
		<
			bc_server_client_socket_connected_state,
			bc_server_client_socket_sending_state,
			bc_server_client_socket_error_state
		>
		{
		public:
			bc_server_client_socket_state_machine(platform::bc_non_block_socket& p_socket)
				: bc_state_machine
				(
					bc_server_client_socket_connected_state(p_socket),
					bc_server_client_socket_sending_state(p_socket),
					bc_server_client_socket_error_state(p_socket)
				),
				m_socket(&p_socket)
			{
			}

			bc_server_client_socket_state_machine(bc_server_client_socket_state_machine&&) noexcept = default;

			~bc_server_client_socket_state_machine() = default;

			bc_server_client_socket_state_machine& operator=(bc_server_client_socket_state_machine&&) noexcept = default;
		
		private:
			platform::bc_non_block_socket* m_socket;
		};
	}
}