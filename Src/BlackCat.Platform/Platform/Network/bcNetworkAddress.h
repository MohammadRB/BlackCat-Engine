// [05/24/2021 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "Platform/Network/bcNetworkDefinitions.h"

namespace black_cat
{
	namespace platform
	{
		template<core_platform::bc_platform>
		struct bc_platform_network_address_pack
		{
		};

		template<core_platform::bc_platform TPlatform>
		class bc_platform_network_address
		{
		public:
			using platform_pack = bc_platform_network_address_pack<TPlatform>;
			
		public:
			bc_platform_network_address();

			explicit bc_platform_network_address(platform_pack& p_pack);

			bc_platform_network_address(const bc_platform_network_address&) noexcept;

			~bc_platform_network_address();

			bc_platform_network_address& operator=(const bc_platform_network_address&) noexcept;

			static bc_platform_network_address from_ip_port(bc_socket_address p_address_family, const bcCHAR* p_ip, bcUINT32 p_port);
			
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

		using bc_network_address = bc_platform_network_address<core_platform::g_current_platform>;
	}	
}