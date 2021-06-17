// [06/09/2021 MRB]

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
		class bc_server_socket_state_machine;
		class bc_server_socket_error_state;
		class bc_server_socket_accepting_state;
		class bc_server_socket_listening_state;

		struct bc_server_socket_update_event
		{
			core_platform::bc_clock::update_param m_clock;
		};

		struct bc_server_socket_bind_event
		{
			bcUINT16 m_port;
		};

		struct bc_server_socket_accept_client_event
		{
			platform::bc_non_block_socket* m_client_socket;
		};

		struct bc_server_socket_disconnect_client_event
		{
			platform::bc_non_block_socket* m_client_socket;
		};

		struct bc_server_socket_send_event
		{
			core::bc_memory_stream& m_stream;
			bcSIZE m_bytes_to_send;
			bcSIZE m_bytes_sent;
		};

		struct bc_server_socket_receive_event
		{
			core::bc_memory_stream& m_stream;
			bcSIZE m_bytes_received;
		};

		class bc_server_socket_error_state : public core::bc_state<bc_server_socket_state_machine, bc_server_socket_bind_event>
		{
		public:
			bc_server_socket_error_state(platform::bc_non_block_socket& p_socket)
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
			state_transition handle(bc_server_socket_bind_event& p_event) override
			{
				try
				{
					m_socket->bind(p_event.m_port);

					if(m_socket->get_protocol() != platform::bc_socket_protocol::udp)
					{
						m_socket->listen();
					}
					
					return state_transition::transfer_to<bc_server_socket_listening_state>();
				}
				catch (const bc_network_exception& p_exception)
				{
					return state_transition::transfer_to<bc_server_socket_error_state>
					(
						[p_exception](bc_server_socket_error_state& p_error_state)
						{
							p_error_state.set_last_exception(p_exception);
						}
					);
				}
			}

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

		class bc_server_socket_accepting_state : public core::bc_state<bc_server_socket_state_machine, bc_server_socket_update_event>
		{
		public:
			bc_server_socket_accepting_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

			platform::bc_non_block_socket get_client_socket() noexcept
			{
				return std::move(m_client_socket);
			}

			void set_client_socket(platform::bc_non_block_socket p_client_socket) noexcept
			{
				m_client_socket = std::move(p_client_socket);
			}

		private:
			state_transition handle(bc_server_socket_update_event& p_event) override
			{
				return state_transition::transfer_to<bc_server_socket_listening_state>();
			}

			platform::bc_non_block_socket* m_socket;
			platform::bc_non_block_socket m_client_socket;
		};
		
		class bc_server_socket_listening_state : public core::bc_state<bc_server_socket_state_machine, bc_server_socket_update_event>
		{
		public:
			bc_server_socket_listening_state(platform::bc_non_block_socket& p_socket)
				: m_socket(&p_socket)
			{
			}

		private:
			state_transition handle(bc_server_socket_update_event& p_event) override
			{
				if(m_socket->is_accept_available())
				{
					return state_transition::transfer_to<bc_server_socket_accepting_state>
					(
						[this](bc_server_socket_accepting_state& p_state)
						{
							auto l_client_socket = m_socket->accept();
							p_state.set_client_socket(std::move(l_client_socket));
						}
					);
				}

				return state_transition::empty();
			}
			
			platform::bc_non_block_socket* m_socket;
		};
		
		class bc_server_socket_state_machine : public core::bc_state_machine
		<
			bc_server_socket_error_state,
			bc_server_socket_listening_state,
			bc_server_socket_accepting_state
		>
		{
		public:
			explicit bc_server_socket_state_machine(platform::bc_non_block_socket& p_socket)
				: bc_state_machine
				(
					bc_server_socket_error_state(p_socket),
					bc_server_socket_listening_state(p_socket),
					bc_server_socket_accepting_state(p_socket)
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
		
		private:
			platform::bc_non_block_socket* m_socket;
		};
	}	
}