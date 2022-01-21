// [01/20/2022 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "PlatformImp/Network/bcNetworkAddress.h"

#pragma once

namespace black_cat
{
	namespace game
	{
		using bc_network_client_id = bcUINT32;

		struct bc_network_client
		{
			constexpr static bc_network_client_id invalid_id = static_cast<bc_network_client_id>(-1);

			platform::bc_network_address m_address;
			bc_network_client_id m_id;
			core::bc_string_view m_name;
		};
	}	
}