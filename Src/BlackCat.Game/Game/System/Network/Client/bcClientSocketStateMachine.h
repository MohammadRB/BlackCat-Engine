// [06/04/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcStateMachine.h"
#include "Core/File/bcMemoryStream.h"
#include "Core/Utility/bcNullable.h"
#include "PlatformImp/Network/bcNonBlockSocket.h"

namespace black_cat
{
	namespace game
	{
		class bc_client_socket_state_machine;
		class bc_client_socket_error_state;
		class bc_client_socket_connecting_state;
		class bc_client_socket_connected_state;
		class bc_client_socket_sending_state;
		class bc_client_socket_receiving_state;

		struct bc_client_socket_update_event
		{
			platform::bc_clock::update_param m_clock;
		};
		
		struct bc_client_socket_connect_event
		{
			const platform::bc_network_address& m_address;
		};

		struct bc_client_socket_send_event
		{
			core::bc_memory_stream& m_stream;
			bcSIZE m_bytes_to_send;
			bcSIZE m_bytes_sent;
		};

		struct bc_client_socket_receive_event
		{
			core::bc_memory_stream& m_stream;
			bcSIZE m_bytes_received;
		};

		class bc_client_socket_error_state : public core::bc_state<bc_client_socket_state_machine, bc_client_socket_connect_event>
		{
		public:
			explicit bc_client_socket_error_state(platform::bc_non_block_socket& p_socket)
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
			state_transition handle(bc_client_socket_connect_event& p_event) override
			{
				try
				{
					m_socket->connect(p_event.m_address);
					return state_transition::transfer_to<bc_client_socket_connecting_state>();
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_client_socket_error_state>
					(
						[p_exception](bc_client_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}
			}

			platform::bc_non_block_socket* m_socket;
			core::bc_nullable<bc_network_exception> m_last_exception;
		};

		class bc_client_socket_connecting_state : public core::bc_state<bc_client_socket_state_machine, bc_client_socket_update_event>
		{
		public:
			explicit bc_client_socket_connecting_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(bc_client_socket_update_event& p_event) override
			{
				try
				{
					const bool l_succeeded = m_socket->is_connect_succeeded();
					if(l_succeeded)
					{
						return state_transition::transfer_to<bc_client_socket_connected_state>();
					}

					return state_transition::empty();
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_client_socket_error_state>
					(
						[p_exception](bc_client_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}
			}
			
			platform::bc_non_block_socket* m_socket;
		};

		class bc_client_socket_connected_state : public core::bc_state<bc_client_socket_state_machine, bc_client_socket_send_event, bc_client_socket_receive_event>
		{
		public:
			explicit bc_client_socket_connected_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket),
				m_buffer(core::bc_alloc_type::unknown_movable, s_buffer_size)
			{
			}

		private:
			state_transition handle(bc_client_socket_send_event& p_event) override
			{
				try
				{
					if (!m_socket->is_send_available())
					{
						p_event.m_bytes_sent = 0;
						return state_transition::empty();
					}
					
					p_event.m_bytes_sent = m_socket->send(p_event.m_stream.get_position_data(), p_event.m_bytes_to_send);
					return state_transition::transfer_to<bc_client_socket_sending_state>();
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_client_socket_error_state>
					(
						[p_exception](bc_client_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}
			}
			
			state_transition handle(bc_client_socket_receive_event& p_event) override
			{				
				try
				{
					if (!m_socket->is_receive_available())
					{
						return state_transition::empty();
					}
					
					p_event.m_bytes_received = m_socket->receive(m_buffer.get_data(), s_buffer_size);
					if(p_event.m_bytes_received)
					{
						p_event.m_stream.write(static_cast<bcBYTE*>(m_buffer.get_data()), p_event.m_bytes_received);
					}

					return state_transition::transfer_to<bc_client_socket_receiving_state>();
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_client_socket_error_state>
					(
						[p_exception](bc_client_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}
			}

			static constexpr bcUINT32 s_buffer_size = 64000;
			
			platform::bc_non_block_socket* m_socket;
			core::bc_memory_stream m_buffer;
		};

		class bc_client_socket_sending_state : public core::bc_state<bc_client_socket_state_machine, bc_client_socket_update_event>
		{
		public:
			explicit bc_client_socket_sending_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(bc_client_socket_update_event& p_event) override
			{
				try
				{
					const bool l_is_available = m_socket->is_send_available();
					if (l_is_available)
					{
						return state_transition::transfer_to<bc_client_socket_connected_state>();
					}
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_client_socket_error_state>
					(
						[p_exception](bc_client_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}

				return state_transition::empty();
			}
			
			platform::bc_non_block_socket* m_socket;
		};

		class bc_client_socket_receiving_state : public core::bc_state<bc_client_socket_state_machine, bc_client_socket_update_event>
		{
		public:
			explicit bc_client_socket_receiving_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(bc_client_socket_update_event& p_event) override
			{
				try
				{
					//const bool l_is_available = m_socket->is_receive_available();
					//if (l_is_available)
					{
						return state_transition::transfer_to<bc_client_socket_connected_state>();
					}
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_client_socket_error_state>
					(
						[p_exception](bc_client_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}

				//return state_transition::empty();
			}
		
			platform::bc_non_block_socket* m_socket;
		};
		
		class bc_client_socket_state_machine : public core::bc_state_machine
		<
			bc_client_socket_error_state,
			bc_client_socket_connecting_state,
			bc_client_socket_connected_state,
			bc_client_socket_sending_state,
			bc_client_socket_receiving_state
		>
		{
		public:
			explicit bc_client_socket_state_machine(platform::bc_non_block_socket& p_socket)
				: bc_state_machine
				(
					bc_client_socket_error_state(p_socket),
					bc_client_socket_connecting_state(p_socket),
					bc_client_socket_connected_state(p_socket),
					bc_client_socket_sending_state(p_socket),
					bc_client_socket_receiving_state(p_socket)
				),
				m_socket(&p_socket)
			{
			}

			bc_client_socket_state_machine(bc_client_socket_state_machine&&) noexcept = default;

			~bc_client_socket_state_machine() = default;

			bc_client_socket_state_machine& operator=(bc_client_socket_state_machine&&) noexcept = default;

			platform::bc_non_block_socket& get_socket() noexcept
			{
				return *m_socket;
			}
			
			const platform::bc_non_block_socket& get_socket() const noexcept
			{
				return *m_socket;
			}

			void update(const platform::bc_clock::update_param& p_clock)
			{
				bc_client_socket_update_event l_update{ p_clock };
				process_event(l_update);
			}
		
		private:
			platform::bc_non_block_socket* m_socket;
		};
	}
}