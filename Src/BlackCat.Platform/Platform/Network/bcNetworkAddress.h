// [05/24/2021 MRB]

#pragma once

#include <tuple>
#include "Core/Container/bcString.h"
#include "CorePlatform/bcPlatform.h"
#include "Platform/Network/bcNetworkDefinitions.h"
#include "Platform/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_logger_output_stream;
	}
}

namespace black_cat
{
	namespace platform
	{
		template<bc_platform>
		struct bc_platform_network_address_pack
		{
		};

		template<bc_platform TPlatform>
		class bc_platform_network_address
		{
		public:
			using platform_pack = bc_platform_network_address_pack<TPlatform>;
			
		public:
			bc_platform_network_address();

			explicit bc_platform_network_address(platform_pack& p_pack);

			bc_platform_network_address(const bc_platform_network_address<TPlatform>&) noexcept;

			~bc_platform_network_address();

			bc_platform_network_address& operator=(const bc_platform_network_address<TPlatform>&) noexcept;

			std::tuple<bc_socket_address, core::bc_string_frame, bcUINT16> get_traits() const noexcept;

			bool is_valid() const noexcept;

			bool operator==(const bc_platform_network_address<TPlatform>& p_other) const noexcept;
			
			bool operator!=(const bc_platform_network_address<TPlatform>& p_other) const noexcept;
			
			platform_pack& get_platform_pack() noexcept
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept
			{
				return m_pack;
			}

			static bc_platform_network_address from_ip_port(bc_socket_address p_address_family, const bcCHAR* p_ip, bcUINT16 p_port) noexcept;
		
		private:
			platform_pack m_pack;
		};

		using bc_network_address = bc_platform_network_address<g_current_platform>;

		BC_PLATFORMSHARED_DLL core::bc_logger_output_stream& operator<<(core::bc_logger_output_stream& p_stream, const bc_network_address& p_address);
	}	
}