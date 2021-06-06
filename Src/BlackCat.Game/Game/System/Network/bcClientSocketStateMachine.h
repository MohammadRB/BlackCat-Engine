// [06/04/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcStateMachine.h"
#include "Core/Utility/bcNullable.h"
#include "Core/Utility/bcLogger.h"
#include "PlatformImp/Network/bcNonBlockSocket.h"

namespace black_cat
{
	namespace game
	{
		class bc_client_socket_state_machine;
		class bc_socket_error_state;
		class bc_socket_connecting_state;
		class bc_socket_connected_state;
		class bc_socket_sending_state;
		class bc_socket_receiving_state;

		struct bc_socket_update_event
		{
			core_platform::bc_clock::update_param m_clock;
		};
		
		struct bc_socket_connect_event
		{
			const bcCHAR* m_ip;
			bcUINT16 m_port;
		};

		struct bc_socket_send_event
		{
			const void* m_buffer;
			bcUINT32 m_buffer_size;
		};

		struct bc_socket_receive_event
		{
			void* m_buffer;
			bcUINT32 m_buffer_size;
		};

		class bc_socket_error_state : core::bc_state<bc_client_socket_state_machine, bc_socket_connect_event>
		{
		public:
			bc_socket_error_state(platform::bc_non_block_socket& p_socket)
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
			state_transition handle(const bc_socket_connect_event& p_event) override
			{
				m_socket->connect(platform::bc_socket_address::inter_network, p_event.m_ip, p_event.m_port);
				return state_transition::transfer_to<bc_socket_connecting_state>();
			}

			void on_enter() override
			{
				if(m_last_exception.has_value())
				{
					const auto l_str = core::bc_to_estring_frame(m_last_exception->get_full_message());
					core::bc_log(core::bc_log_type::error, l_str.c_str());
				}
			}

			void on_exit() override
			{
				m_last_exception.reset();
			}
		
			platform::bc_non_block_socket* m_socket;
			core::bc_nullable<bc_network_exception> m_last_exception;
		};

		class bc_socket_connecting_state : core::bc_state<bc_client_socket_state_machine, bc_socket_update_event>
		{
		public:
			bc_socket_connecting_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(const bc_socket_update_event& p_event) override
			{
				try
				{
					const bool l_succeeded = m_socket->is_connect_succeeded();
					if(l_succeeded)
					{
						return state_transition::transfer_to<bc_socket_connected_state>();
					}

					return state_transition::empty();
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_socket_error_state>
					(
						[p_exception](bc_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}
			}
			
			platform::bc_non_block_socket* m_socket;
		};

		class bc_socket_connected_state : core::bc_state<bc_client_socket_state_machine, bc_socket_send_event, bc_socket_receive_event>
		{
		public:
			bc_socket_connected_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(const bc_socket_send_event& p_event) override
			{
				try
				{
					m_socket->send(p_event.m_buffer, p_event.m_buffer_size);
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_socket_error_state>
					(
						[p_exception](bc_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}
				
				return state_transition::transfer_to<bc_socket_sending_state>();
			}
			
			state_transition handle(const bc_socket_receive_event& p_event) override
			{
				try
				{
					m_socket->receive(p_event.m_buffer, p_event.m_buffer_size);
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_socket_error_state>
					(
						[p_exception](bc_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}
				
				return state_transition::transfer_to<bc_socket_receiving_state>();
			}
		
			platform::bc_non_block_socket* m_socket;
		};

		class bc_socket_sending_state : core::bc_state<bc_client_socket_state_machine, bc_socket_update_event>
		{
		public:
			bc_socket_sending_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(const bc_socket_update_event& p_event) override
			{
				try
				{
					const bool l_is_available = m_socket->is_send_available();
					if (l_is_available)
					{
						return state_transition::transfer_to<bc_socket_connected_state>();
					}
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_socket_error_state>
					(
						[p_exception](bc_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}

				return state_transition::empty();
			}
			
			platform::bc_non_block_socket* m_socket;
		};

		class bc_socket_receiving_state : core::bc_state<bc_client_socket_state_machine, bc_socket_update_event>
		{
		public:
			bc_socket_receiving_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(const bc_socket_update_event& p_event) override
			{
				try
				{
					const bool l_is_available = m_socket->is_receive_available();
					if (l_is_available)
					{
						return state_transition::transfer_to<bc_socket_connected_state>();
					}
				}
				catch (const bc_network_exception & p_exception)
				{
					return state_transition::transfer_to<bc_socket_error_state>
					(
						[p_exception](bc_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}

				return state_transition::empty();
			}
		
			platform::bc_non_block_socket* m_socket;
		};
		
		class bc_client_socket_state_machine : public core::bc_state_machine
		<
			bc_socket_error_state,
			bc_socket_connecting_state,
			bc_socket_connected_state,
			bc_socket_sending_state,
			bc_socket_receiving_state
		>
		{
		public:
			bc_client_socket_state_machine(platform::bc_non_block_socket& p_socket)
				: bc_state_machine
				(
					bc_socket_error_state(p_socket),
					bc_socket_connecting_state(p_socket),
					bc_socket_connected_state(p_socket),
					bc_socket_sending_state(p_socket),
					bc_socket_receiving_state(p_socket)
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

		private:
			platform::bc_non_block_socket* m_socket;
		};
	}	
}