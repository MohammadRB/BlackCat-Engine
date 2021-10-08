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

		enum class bc_actor_network_data_dir
		{
			replicate,
			replicate_sync,
			replicate_sync_from_client
		};

		enum class bc_actor_replication_side
		{
			origin,
			replicated
		};

		using bc_network_message_id = bcINT32; // it must be int because in json deserialization it is interpreted as int
		using bc_network_message_hash = core::bc_string_cmp_hash;
		using bc_network_packet_time = core_platform::bc_clock::big_clock;
		using bc_network_rtt = core_platform::bc_clock::small_delta_time;
	}
}