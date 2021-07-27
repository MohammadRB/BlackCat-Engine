// [06/05/2021 MRB]

#include "Game/GamePCH.h"

#include <chrono>
#include "Core/File/bcJsonDocument.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/Network/bcNetworkMessageBuffer.h"

namespace black_cat
{
	namespace game
	{
		BC_JSON_STRUCTURE(bc_network_packet_command)
		{
			BC_JSON_VALUE(bc_network_message_hash, hash);
			BC_JSON_VALUE(core::bc_json_key_value, values);
		};
		
		BC_JSON_STRUCTURE(bc_network_packet)
		{
			BC_JSON_VALUE(bc_network_packet_time, time);
			BC_JSON_ARRAY(bc_network_packet_command, commands);
		};

		bc_network_packet_time bc_current_packet_time() noexcept
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}

		bc_network_packet_time bc_elapsed_packet_time(const bc_network_packet_time& p_packet_time) noexcept
		{
			return bc_current_packet_time() - p_packet_time;
		}

		bc_network_message_buffer::bc_network_message_buffer(bc_network_system& p_network_system)
			: m_network_system(&p_network_system)
		{
		}

		std::pair<core::bc_memory_stream*, bcUINT32> bc_network_message_buffer::serialize(const bc_network_packet_time p_time, const core::bc_span<bc_network_message_ptr>& p_messages)
		{
			core::bc_json_document<bc_network_packet> l_json_packet;
			l_json_packet->m_time.set(p_time);

			for (const auto& l_command : p_messages)
			{
				auto& l_json_command = l_json_packet->m_commands.new_entry();
				*l_json_command->m_hash = l_command->get_message_hash();
				l_command->serialize(bc_network_message_serialization_context{ *l_json_command->m_values });
			}

			const auto l_packet_str = l_json_packet.write();
			m_serialize_buffer.set_position(core::bc_stream_seek::start, 0);
			m_serialize_buffer.write(l_packet_str.c_str(), l_packet_str.size());
			m_serialize_buffer.set_position(core::bc_stream_seek::start, 0);

			return std::make_pair<core::bc_memory_stream*, bcUINT32>(&m_serialize_buffer, l_packet_str.size());
		}

		std::pair<bc_network_packet_time, core::bc_span<bc_network_message_ptr>> bc_network_message_buffer::deserialize(bci_network_message_deserialization_visitor& p_bridge, 
			core::bc_memory_stream& p_buffer,
			bcUINT32 p_buffer_size)
		{
			const core::bc_string_frame l_str(static_cast<const bcCHAR*>(p_buffer.get_position_data()), p_buffer_size);
			core::bc_json_document<bc_network_packet> l_json_packet;
			l_json_packet.load(l_str.c_str());
			m_deserialize_buffer.clear();
			
			for (auto& l_json_command : l_json_packet->m_commands)
			{
				auto l_command = m_network_system->create_message_instance(*l_json_command->m_hash);
				if(!l_command)
				{
					continue;
				}
				
				l_command->deserialize(bc_network_message_deserialization_context{ p_bridge, *l_json_command->m_values });
				m_deserialize_buffer.push_back(std::move(l_command));
			}

			return std::make_pair(*l_json_packet->m_time, core::bc_make_span(m_deserialize_buffer));
		}
	}
}