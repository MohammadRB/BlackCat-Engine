// [06/05/2021 MRB]

#pragma once

#include <utility>
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcSpan.h"
#include "Game/System/Network/Command/bcNetworkCommand.h"

namespace black_cat
{
	namespace game
	{
		class bc_network_system;
		using bc_network_packet_time = core_platform::bc_clock::big_delta_time;
		
		class bc_network_command_packet
		{
		public:
			bc_network_command_packet(bc_network_system& p_network_system);
			
			bc_network_command_packet(bc_network_command_packet&&) noexcept = default;

			~bc_network_command_packet() = default;

			bc_network_command_packet& operator=(bc_network_command_packet&&) noexcept = default;
			
			std::pair<const void*, bcUINT32> serialize(bc_network_packet_time p_time, core::bc_span<bc_network_command_ptr> p_commands);

			std::pair<bc_network_packet_time, core::bc_span<bc_network_command_ptr>> deserialize(const void* p_buffer, bcUINT32 p_buffer_size);

		private:
			bc_network_system* m_network_system;
			core::bc_string m_serialize_buffer;
			core::bc_vector<bc_network_command_ptr> m_deserialize_buffer;
		};
	}	
}