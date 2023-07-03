// [24/09/2021 MRB]

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
			replicate,							// Replicate actor from server
			replicate_sync,						// Replicate actor from server
			replicate_sync_from_client,			// Replicate actor from client
			replicate_sync_from_client_server	// Convert to 'replicate_sync' or 'replicate_sync_from_client'
		};

		enum class bc_actor_replication_side : bcUBYTE
		{
			origin,
			replicated
		};

		using bc_actor_network_id = bcINT32;
		using bc_network_message_id = bcUINT32;
		using bc_network_message_hash = core::bc_string_cmp_hash;
		using bc_network_packet_time = platform::bc_clock::big_clock;
		using bc_network_rtt = platform::bc_clock::small_time;

		constexpr bc_actor_network_id g_invalid_actor_network_id = static_cast<bc_actor_network_id>(-1);
	}
}