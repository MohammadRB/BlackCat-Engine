// [05/30/2021 MRB]

#pragma once

#include "PlatformImp/Network/bcSocket.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_network_connection
		{
		public:
			bc_network_connection() noexcept;

			bc_network_connection(platform::bc_socket_address p_address, platform::bc_socket_type p_type, platform::bc_socket_protocol p_protocol);

			~bc_network_connection();
			
			bc_network_connection(bc_network_connection&&) noexcept;

			bc_network_connection& operator=(bc_network_connection&&) noexcept;

			void set_timeout(bcUINT32 p_milliseconds);
			
			void start_listening(bcUINT16 p_port);

			platform::bc_socket accept();

			void connect(platform::bc_socket_address p_address_family, const bcCHAR* p_ip, bcUINT16 p_port);

			bcUINT32 send(const void* p_buffer, bcUINT32 p_buffer_size);

			bcUINT32 receive(void* p_buffer, bcUINT32 p_buffer_size);

			void close();

			bool is_valid() const noexcept;

		private:
			platform::bc_socket m_socket;
		};
	}	
}