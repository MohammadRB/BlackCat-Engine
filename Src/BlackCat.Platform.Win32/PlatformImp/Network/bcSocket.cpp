// [25/05/2021 MRB]

#include "PlatformImp/PlatformImpPCH.h"

#include "Core/Container/bcString.h"
#include "PlatformImp/Network/bcSocket.h"
#include "PlatformImp/bcUtility.h"
#include "PlatformImp/bcExport.h"
#include <WS2tcpip.h>

namespace black_cat
{
	namespace platform
	{
		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_socket<platform::g_api_win32>::bc_platform_socket() noexcept
		{
			m_pack.m_socket = INVALID_SOCKET;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_socket<platform::g_api_win32>::bc_platform_socket(bc_socket_address p_address,
			bc_socket_type p_type,
			bc_socket_protocol p_protocol)
		{
			m_pack.m_socket = socket
			(
				bc_cast_from_address_family(p_address),
				bc_cast_from_socket_type(p_type),
				bc_cast_from_socket_protocol(p_protocol)
			);

			if(!is_valid())
			{
				bc_throw_network_exception();
			}
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_socket<platform::g_api_win32>::bc_platform_socket(bc_platform_socket&& p_other) noexcept
		{
			m_pack.m_socket = p_other.m_pack.m_socket;
			p_other.m_pack.m_socket = INVALID_SOCKET;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_socket<platform::g_api_win32>::~bc_platform_socket()
		{
			if(!is_valid())
			{
				return;
			}

			close();
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_socket<platform::g_api_win32>& bc_platform_socket<platform::g_api_win32>::operator=(bc_platform_socket&& p_other) noexcept
		{
			m_pack.m_socket = p_other.m_pack.m_socket;
			p_other.m_pack.m_socket = INVALID_SOCKET;

			return *this;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_socket_address bc_platform_socket<platform::g_api_win32>::get_address_family() const
		{
			return std::get<bc_socket_address>(get_traits());
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_socket_type bc_platform_socket<platform::g_api_win32>::get_type() const
		{
			return std::get<bc_socket_type>(get_traits());
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_socket_protocol bc_platform_socket<platform::g_api_win32>::get_protocol() const
		{
			return std::get<bc_socket_protocol>(get_traits());
		}

		template<>
		BC_PLATFORMIMP_DLL
		std::tuple<bc_socket_address, bc_socket_type, bc_socket_protocol> bc_platform_socket<platform::g_api_win32>::get_traits() const
		{
			WSAPROTOCOL_INFO l_socket_info;
			const auto l_result = WSADuplicateSocket(m_pack.m_socket, GetCurrentProcessId(), &l_socket_info);
			if (l_result != 0)
			{
				bc_throw_network_exception();
			}

			auto l_address = bc_cast_to_address_family(l_socket_info.iAddressFamily);
			auto l_type = bc_cast_to_socket_type(l_socket_info.iSocketType);
			auto l_protocol = bc_cast_to_socket_protocol(l_socket_info.iProtocol);

			return std::make_tuple(l_address, l_type, l_protocol);
		}
		
		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_socket<platform::g_api_win32>::set_timeout(bcUINT32 p_milliseconds)
		{
			auto l_timeout = static_cast<DWORD>(p_milliseconds);
			auto l_result = setsockopt(m_pack.m_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&l_timeout), sizeof(DWORD));
			if(l_result == SOCKET_ERROR)
			{
				bc_throw_network_exception();
			}

			l_result = setsockopt(m_pack.m_socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&l_timeout), sizeof(DWORD));
			if (l_result == SOCKET_ERROR)
			{
				bc_throw_network_exception();
			}
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_socket<platform::g_api_win32>::bind(bcUINT16 p_port)
		{
			sockaddr_in l_address;
			ZeroMemory(&l_address, sizeof(l_address));

			l_address.sin_family = bc_cast_from_address_family(get_address_family());
			l_address.sin_port = htons(p_port);

			const auto l_bind_result = ::bind(m_pack.m_socket, reinterpret_cast<sockaddr*>(&l_address), sizeof(sockaddr_in));
			if (l_bind_result == SOCKET_ERROR)
			{
				bc_throw_network_exception();
			}
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_socket<platform::g_api_win32>::listen()
		{
			const auto l_listen_result = ::listen(m_pack.m_socket, SOMAXCONN);
			if(l_listen_result == SOCKET_ERROR)
			{
				bc_throw_network_exception();
			}
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_socket<platform::g_api_win32> bc_platform_socket<platform::g_api_win32>::accept()
		{
			const auto l_client_socket = ::accept(m_pack.m_socket, nullptr, nullptr);
			if(l_client_socket == INVALID_SOCKET)
			{
				bc_throw_network_exception();
			}

			bc_socket l_socket;
			l_socket.get_platform_pack().m_socket = l_client_socket;
			
			return std::move(l_socket);
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_socket<platform::g_api_win32>::connect(const bc_network_address& p_address)
		{
			if (!p_address.is_valid())
			{
				bc_throw_network_exception("Invalid network address");
			}

			const sockaddr_in& l_address = p_address.get_platform_pack().m_address;
			const auto l_connect_result = ::connect(m_pack.m_socket, reinterpret_cast<const sockaddr*>(&l_address), sizeof(sockaddr_in));
			if (l_connect_result == SOCKET_ERROR)
			{
				const auto l_error = WSAGetLastError();
				if (l_error == WSAEWOULDBLOCK)
				{
					return;
				}

				bc_throw_network_exception();
			}
		}

		template<>
		BC_PLATFORMIMP_DLL
		bcUINT32 bc_platform_socket<platform::g_api_win32>::send(const void* p_buffer, bcUINT32 p_buffer_size)
		{
			const auto l_send_result = ::send(m_pack.m_socket, static_cast<const bcCHAR*>(p_buffer), p_buffer_size, 0);
			if(l_send_result == SOCKET_ERROR)
			{
				bc_throw_network_exception();
			}

			return l_send_result;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bcUINT32 bc_platform_socket<platform::g_api_win32>::send_to(const bc_network_address& p_address, const void* p_buffer, bcUINT32 p_buffer_size)
		{
			const sockaddr_in& l_address = p_address.get_platform_pack().m_address;
			const auto l_send_result = ::sendto
			(
				m_pack.m_socket, 
				static_cast<const bcCHAR*>(p_buffer), 
				p_buffer_size, 
				0, 
				reinterpret_cast<const sockaddr*>(&l_address), 
				sizeof(sockaddr_in)
			);
			if (l_send_result == SOCKET_ERROR)
			{
				bc_throw_network_exception();
			}

			return l_send_result;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bcUINT32 bc_platform_socket<platform::g_api_win32>::receive(void* p_buffer, bcUINT32 p_buffer_size)
		{
			const auto l_receive_result = ::recv(m_pack.m_socket, static_cast<bcCHAR*>(p_buffer), p_buffer_size, 0);
			if(l_receive_result == SOCKET_ERROR)
			{
				const auto l_error = WSAGetLastError();
				if (l_error == WSAEWOULDBLOCK)
				{
					return 0;
				}
				
				bc_throw_network_exception();
			}

			return l_receive_result;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bcUINT32 bc_platform_socket<platform::g_api_win32>::receive_from(bc_network_address& p_address, void* p_buffer, bcUINT32 p_buffer_size)
		{
			sockaddr_in& l_address = p_address.get_platform_pack().m_address;
			bcINT l_address_size = sizeof(sockaddr_in);
			
			const auto l_receive_result = ::recvfrom
			(
				m_pack.m_socket, 
				static_cast<bcCHAR*>(p_buffer), 
				p_buffer_size, 
				0,
				reinterpret_cast<sockaddr*>(&l_address),
				&l_address_size
			);
			if (l_receive_result == SOCKET_ERROR)
			{
				const auto l_error = WSAGetLastError();
				if (l_error == WSAEWOULDBLOCK)
				{
					return 0;
				}
				
				bc_throw_network_exception();
			}

			return l_receive_result;
		}
		
		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_socket<platform::g_api_win32>::close()
		{
			closesocket(m_pack.m_socket);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_socket<platform::g_api_win32>::is_valid() const noexcept
		{
			return m_pack.m_socket != INVALID_SOCKET;
		}
	}
}