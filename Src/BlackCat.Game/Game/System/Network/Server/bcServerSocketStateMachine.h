// [06/09/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcStateMachine.h"
#include "Core/Container/bcVector.h"
#include "Core/File/bcMemoryStream.h"
#include "Core/Utility/bcNullable.h"
#include "PlatformImp/Network/bcNonBlockSocket.h"

namespace black_cat
{
	namespace game
	{
		class bc_server_socket_state_machine;
		class bc_server_socket_error_state;
		class bc_server_socket_listening_state;
		class bc_server_socket_sending_state;
		class bc_server_socket_receiving_state;

		struct bc_server_socket_update_event
		{
			core_platform::bc_clock::update_param m_clock;
		};

		struct bc_server_socket_bind_event
		{
			bcUINT16 m_port;
		};

		struct bc_server_socket_send_event
		{
			const platform::bc_network_address& m_address;
			core::bc_memory_stream& m_stream;
			bcSIZE m_bytes_to_send;
			bcSIZE m_bytes_sent;
		};

		struct bc_server_socket_receive_event
		{
			platform::bc_network_address& m_address;
			core::bc_memory_stream& m_stream;
			bcSIZE m_bytes_received;
		};

		class bc_server_socket_error_state : public core::bc_state<bc_server_socket_state_machine, bc_server_socket_bind_event>
		{
		public:
			explicit bc_server_socket_error_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

			const bc_network_exception* get_last_exception() const noexcept
			{
				return m_last_exception.get();
			}

			const platform::bc_network_address* get_last_client_address() const noexcept
			{
				return m_last_client_address.get();
			}
			
			void set_last_exception(const bc_network_exception& p_exception, const platform::bc_network_address* p_client_address) noexcept
			{
				m_last_exception.reset(p_exception);

				if(p_client_address)
				{
					m_last_client_address.reset(*p_client_address);
				}
				else
				{
					m_last_client_address.reset();
				}
			}
		
		private:
			state_transition handle(bc_server_socket_bind_event& p_event) override
			{
				try
				{
					m_socket->bind(p_event.m_port);
					return state_transition::transfer_to<bc_server_socket_listening_state>();
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_server_socket_error_state>
					(
						[p_exception](bc_server_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception, nullptr);
						}
					);
				}
			}
			
			platform::bc_non_block_socket* m_socket;
			core::bc_nullable<bc_network_exception> m_last_exception;
			core::bc_nullable<platform::bc_network_address> m_last_client_address;
		};
		
		class bc_server_socket_listening_state : public core::bc_state<bc_server_socket_state_machine, bc_server_socket_send_event, bc_server_socket_receive_event>
		{
		public:
			explicit bc_server_socket_listening_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket),
				m_buffer(core::bc_alloc_type::unknown_movable, s_buffer_size)
			{
			}

		private:
			state_transition handle(bc_server_socket_send_event& p_event) override
			{
				try
				{
					if (!m_socket->is_send_available())
					{
						p_event.m_bytes_sent = 0;
						return state_transition::empty();
					}

					p_event.m_bytes_sent = m_socket->send_to(p_event.m_address, p_event.m_stream.get_position_data(), p_event.m_bytes_to_send);
					return state_transition::transfer_to<bc_server_socket_sending_state>();
				}
				catch (const bc_network_exception& p_exception)
				{
					const auto& l_address = p_event.m_address;
					return state_transition::transfer_to<bc_server_socket_error_state>
					(
						[p_exception, l_address](bc_server_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception, &l_address);
						}
					);
				}
			}

			state_transition handle(bc_server_socket_receive_event& p_event) override
			{
				try
				{
					if (!m_socket->is_receive_available())
					{
						return state_transition::empty();
					}

					p_event.m_bytes_received = m_socket->receive_from(p_event.m_address, m_buffer.get_data(), s_buffer_size);
					if (p_event.m_bytes_received)
					{
						p_event.m_stream.write(m_buffer.get_data(), p_event.m_bytes_received);
					}

					return state_transition::transfer_to<bc_server_socket_receiving_state>();
				}
				catch (const bc_network_exception & p_exception)
				{
					const auto& l_address = p_event.m_address;
					return state_transition::transfer_to<bc_server_socket_error_state>
					(
						[p_exception, l_address](bc_server_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception, &l_address);
						}
					);
				}
			}

			static constexpr bcUINT32 s_buffer_size = 64000;
			
			platform::bc_non_block_socket* m_socket;
			core::bc_memory_stream m_buffer;
		};

		class bc_server_socket_sending_state : public core::bc_state<bc_server_socket_state_machine, bc_server_socket_update_event>
		{
		public:
			explicit bc_server_socket_sending_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(bc_server_socket_update_event& p_event) override
			{
				try
				{
					const bool l_is_available = m_socket->is_send_available();
					if (l_is_available)
					{
						return state_transition::transfer_to<bc_server_socket_listening_state>();
					}
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_server_socket_error_state>
					(
						[p_exception](bc_server_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception, nullptr);
						}
					);
				}

				return state_transition::empty();
			}

			platform::bc_non_block_socket* m_socket;
		};

		class bc_server_socket_receiving_state : public core::bc_state<bc_server_socket_state_machine, bc_server_socket_update_event>
		{
		public:
			explicit bc_server_socket_receiving_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(bc_server_socket_update_event& p_event) override
			{
				try
				{
					return state_transition::transfer_to<bc_server_socket_listening_state>();
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_server_socket_error_state>
					(
						[p_exception](bc_server_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception, nullptr);
						}
					);
				}
			}

			platform::bc_non_block_socket* m_socket;
		};
		
		/**
		 * \brief State machine for connection-less sockets
		 */
		class bc_server_socket_state_machine : public core::bc_state_machine
		<
			bc_server_socket_error_state,
			bc_server_socket_listening_state,
			bc_server_socket_sending_state,
			bc_server_socket_receiving_state
		>
		{
		public:
			explicit bc_server_socket_state_machine(platform::bc_non_block_socket& p_socket)
				: bc_state_machine
				(
					bc_server_socket_error_state(p_socket),
					bc_server_socket_listening_state(p_socket),
					bc_server_socket_sending_state(p_socket),
					bc_server_socket_receiving_state(p_socket)
				),
				m_socket(&p_socket)
			{
			}

			bc_server_socket_state_machine(bc_server_socket_state_machine&&) noexcept = default;

			~bc_server_socket_state_machine() = default;

			bc_server_socket_state_machine& operator=(bc_server_socket_state_machine&&) noexcept = default;
			
			platform::bc_non_block_socket& get_socket() noexcept
			{
				return *m_socket;
			}

			const platform::bc_non_block_socket& get_socket() const noexcept
			{
				return *m_socket;
			}

			void update(const core_platform::bc_clock::update_param& p_clock)
			{
				bc_server_socket_update_event l_update{ p_clock };
				process_event(l_update);
			}
		
		private:
			platform::bc_non_block_socket* m_socket;
		};
	}
}