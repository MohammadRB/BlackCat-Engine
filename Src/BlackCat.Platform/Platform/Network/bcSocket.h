// [23/05/2021 MRB]

#pragma once

#include <tuple>
#include "CorePlatform/bcPlatform.h"
#include "Platform/Network/bcNetworkDefinitions.h"
#include "Platform/Network/bcNetworkAddress.h"
#include "Platform/bcException.h"

namespace black_cat::platform
{
	template<bc_platform>
	struct bc_platform_socket_pack
	{
	};

	template<bc_platform TPlatform>
	class bc_platform_socket
	{
	public:
		using platform_pack = bc_platform_socket_pack<TPlatform>;
			
	public:
		bc_platform_socket() noexcept;
			
		bc_platform_socket(bc_socket_address p_address, bc_socket_type p_type, bc_socket_protocol p_protocol);

		bc_platform_socket(bc_platform_socket&&) noexcept;

		~bc_platform_socket();

		bc_platform_socket& operator=(bc_platform_socket&&) noexcept;

		bc_socket_address get_address_family() const;

		bc_socket_type get_type() const;

		bc_socket_protocol get_protocol() const;

		std::tuple<bc_socket_address, bc_socket_type, bc_socket_protocol> get_traits() const;
			
		/**
			 * \brief Set timeout in milliseconds
			 * \param p_milliseconds 
			 */
		void set_timeout(bcUINT32 p_milliseconds);
			
		/**
			 * \brief Bind to a specified network address to accept client connections. In case of any error throws network exception
			 * \param p_port 
			 */
		void bind(bcUINT16 p_port);

		/**
			 * \brief After the socket is bound, start listening on bound address.
			 * \n This function is operable with connection oriented sockets.
			 */
		void listen();

		/**
			 * \brief Accept incoming connections after socket was set to listen state.
			 * \n This function is operable with connection oriented sockets.
			 * \return 
			 */
		bc_platform_socket accept();
			
		/**
			 * \brief Try to connect to specified network address. In case of any error throws network exception
			 * \param p_address
			 */
		void connect(const bc_network_address& p_address);

		/**
			 * \brief Send data over a connected socket. In case of any error throws network exception.
			 * \n This function is operable with connection oriented sockets.
			 * \param p_buffer 
			 * \param p_buffer_size 
			 * \return The total number of bytes sent
			 */
		bcUINT32 send(const void* p_buffer, bcUINT32 p_buffer_size);

		/**
			 * \brief Send data over a connectionless socket. In case of any error throws network exception.
			 * \param p_address The address which data must be sent.
			 * \param p_buffer 
			 * \param p_buffer_size 
			 * \return The total number of bytes sent
			 */
		bcUINT32 send_to(const bc_network_address& p_address, const void* p_buffer, bcUINT32 p_buffer_size);

		/**
			 * \brief Receives data from a connected socket. In case of any error throws network exception.
			 * \n This function is operable with connection oriented sockets.
			 * \param p_buffer 
			 * \param p_buffer_size 
			 * \return The total number of bytes received
			 */
		bcUINT32 receive(void* p_buffer, bcUINT32 p_buffer_size);

		/**
			 * \brief Receives data from a connectionless socket. In case of any error throws network exception.
			 * \param p_address The address which data are received from.
			 * \param p_buffer 
			 * \param p_buffer_size 
			 * \return The total number of bytes received
			 */
		bcUINT32 receive_from(bc_network_address& p_address, void* p_buffer, bcUINT32 p_buffer_size);

		/**
			 * \brief Early socket shutdown. Socket will be closed in destructor if is in valid state
			 */
		void close();
			
		/**
			 * \brief Check if socket is in valid and connected state
			 * \return 
			 */
		bool is_valid() const noexcept;
			
		platform_pack& get_platform_pack() noexcept
		{
			return m_pack;
		}

		const platform_pack& get_platform_pack() const noexcept
		{
			return m_pack;
		}
		
	private:
		platform_pack m_pack;
	};

	using bc_socket = bc_platform_socket<g_current_platform>;
}
