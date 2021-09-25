// [09/24/2021 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace game
	{
		enum class bc_network_type : bcUBYTE
		{
			not_started, server, client
		};

		using bc_network_message_id = bcINT32; // it must be int because in json deserialization it is interpreted as int
		using bc_network_message_hash = core::bc_string_cmp_hash;
	}
}