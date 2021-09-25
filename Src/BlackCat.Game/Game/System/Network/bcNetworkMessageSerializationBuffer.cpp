// [06/05/2021 MRB]

#include "Game/GamePCH.h"

#include <chrono>
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/File/bcJsonDocument.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/Network/bcNetworkMessageSerializationBuffer.h"

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

		core::bc_vector_frame<core::bc_string_frame> _split_multiple_packets(const core::bc_string_frame& p_packet)
		{
			constexpr bcCHAR l_delimiter[] = "{\"time\":";
			
			core::bc_string_frame::size_type l_next_packet_pos = 0;
			core::bc_vector_frame<core::bc_string_frame> l_packets;
			l_packets.reserve(5);

			do
			{
				const auto l_last_packet_pos = l_next_packet_pos;
				l_next_packet_pos = p_packet.find(l_delimiter, l_last_packet_pos + sizeof(l_delimiter));
				auto l_sub_packet = p_packet.substr(l_last_packet_pos, l_next_packet_pos - l_last_packet_pos);

				l_packets.push_back(std::move(l_sub_packet));
			}
			while (l_next_packet_pos != core::bc_string_frame::npos);

			return l_packets;
		}
		
		bc_network_packet_time bc_current_packet_time() noexcept
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}

		bc_network_packet_time bc_elapsed_packet_time(const bc_network_packet_time& p_packet_time) noexcept
		{
			return bc_current_packet_time() - p_packet_time;
		}

		bc_network_message_serialization_buffer::bc_network_message_serialization_buffer(bc_network_system& p_network_system)
			: m_network_system(&p_network_system)
		{
		}

		std::pair<bcUINT32, core::bc_memory_stream*> bc_network_message_serialization_buffer::serialize(bci_network_message_serialization_visitor& p_visitor,
			const bc_network_packet_time p_time,
			const core::bc_const_span<bc_network_message_ptr>& p_messages)
		{
			core::bc_json_document<bc_network_packet> l_json_packet;
			l_json_packet->m_time.set(p_time);

			for (const auto& l_command : p_messages)
			{
				auto& l_json_command = l_json_packet->m_commands.new_entry();
				*l_json_command->m_hash = l_command->get_message_hash();
				l_command->serialize(bc_network_message_serialization_context{ p_visitor, *l_json_command->m_values });
			}

			const auto l_packet_str = l_json_packet.write();
			m_serialize_buffer.set_position(core::bc_stream_seek::start, 0);
			m_serialize_buffer.write(l_packet_str.c_str(), l_packet_str.size());
			m_serialize_buffer.set_position(core::bc_stream_seek::start, 0);

			return std::make_pair<bcUINT32, core::bc_memory_stream*>(l_packet_str.size() , &m_serialize_buffer);
		}

		std::pair<bc_network_packet_time, core::bc_span<bc_network_message_ptr>> bc_network_message_serialization_buffer::deserialize(bci_network_message_deserialization_visitor& p_visitor, 
			core::bc_memory_stream& p_buffer,
			bcUINT32 p_buffer_size)
		{
			m_deserialize_buffer.clear();
			
			const core::bc_string_frame l_packet(static_cast<const bcCHAR*>(p_buffer.get_position_data()), p_buffer_size);
			const auto l_sub_packets = _split_multiple_packets(l_packet);
			bc_network_packet_time l_min_packet_time = std::numeric_limits<bc_network_packet_time>::max();

			for (auto& l_sub_packet : l_sub_packets)
			{
				core::bc_json_document<bc_network_packet> l_json_packet;
				l_json_packet.load(l_sub_packet.c_str());

				for (auto& l_json_command : l_json_packet->m_commands)
				{
					auto l_command = m_network_system->create_message_instance(*l_json_command->m_hash);
					if (!l_command)
					{
						continue;
					}

					l_command->deserialize(bc_network_message_deserialization_context{ p_visitor, *l_json_command->m_values });
					m_deserialize_buffer.push_back(std::move(l_command));
				}

				l_min_packet_time = std::min(l_min_packet_time, *l_json_packet->m_time);
			}

			return std::make_pair(l_min_packet_time, core::bc_make_span(m_deserialize_buffer));
		}
	}
}