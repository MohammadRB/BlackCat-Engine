// [09/24/2021 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace game
	{
		enum class bc_network_type : bcUBYTE
		{
			not_started, server, client
		};

		enum class bc_network_state : bcUBYTE
		{
			error, connected
		};

		enum class bc_actor_network_data_dir : bcUBYTE
		{
			replicate,						// Replicate actor from server
			replicate_sync,					// Replicate actor from server
			replicate_sync_from_client,		// Replicate actor from client
			replicate_sync_to_server_client	// Convert to 'replicate_sync' or 'replicate_sync_from_client'
		};

		enum class bc_actor_replication_side : bcUBYTE
		{
			origin,
			replicated
		};

		using bc_network_message_id = bcINT32; // it must be int because in json deserialization it is interpreted as int
		using bc_network_message_hash = core::bc_string_cmp_hash;
		using bc_network_packet_time = platform::bc_clock::big_clock;
		using bc_network_rtt = platform::bc_clock::small_time;
	}
}