// [06/05/2021 MRB]

#pragma once

#include <utility>
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcSpan.h"
#include "Core/File/bcMemoryStream.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_network_system;
		using bc_network_packet_time = core_platform::bc_clock::big_clock;

		BC_GAME_DLL bc_network_packet_time bc_current_packet_time() noexcept;

		BC_GAME_DLL bc_network_packet_time bc_elapsed_packet_time(const bc_network_packet_time& p_packet_time) noexcept;

		struct bc_message_with_time
		{
			bc_network_packet_time m_time;
			bc_network_message_ptr m_message;
		};
		
		class bc_network_message_buffer
		{
		public:
			explicit bc_network_message_buffer(bc_network_system& p_network_system);
			
			bc_network_message_buffer(bc_network_message_buffer&&) noexcept = default;

			~bc_network_message_buffer() = default;

			bc_network_message_buffer& operator=(bc_network_message_buffer&&) noexcept = default;
			
			std::pair<core::bc_memory_stream*, bcUINT32> serialize(bc_network_packet_time p_time, const core::bc_span<bc_network_message_ptr>& p_messages);

			std::pair<bc_network_packet_time, core::bc_span<bc_network_message_ptr>> deserialize(core::bc_memory_stream& p_buffer, bcUINT32 p_buffer_size);

		private:
			bc_network_system* m_network_system;
			core::bc_memory_stream m_serialize_buffer;
			core::bc_vector<bc_network_message_ptr> m_deserialize_buffer;
		};
	}	
}