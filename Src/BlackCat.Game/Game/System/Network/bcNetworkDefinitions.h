// [09/24/2021 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcString.h"
#include "Core/Utility/bcValueChangeRateSampler.h"

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
		template<typename T>
		using bc_network_extrapolation = core::bc_value_change_rate_sampler<T, 80, 15, 5>;
	}
}