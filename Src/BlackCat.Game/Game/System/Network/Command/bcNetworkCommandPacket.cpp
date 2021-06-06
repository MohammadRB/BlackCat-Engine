// [06/05/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/File/bcJsonDocument.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/Network/bcNetworkCommandPacket.h"

namespace black_cat
{
	namespace game
	{
		BC_JSON_STRUCTURE(bc_network_packet_command)
		{
			BC_JSON_VALUE(bc_network_command_hash, hash);
			BC_JSON_VALUE(core::bc_json_key_value, values);
		};
		
		BC_JSON_STRUCTURE(bc_network_packet)
		{
			BC_JSON_VALUE(core_platform::bc_clock::big_clock, time);
			BC_JSON_ARRAY(bc_network_packet_command, commands);
		};
		
		bc_network_command_packet::bc_network_command_packet(bc_network_system& p_network_system)
			: m_network_system(&p_network_system)
		{
			
		}

		std::pair<const void*, bcUINT32> bc_network_command_packet::serialize(bc_network_packet_time p_time, core::bc_span<bc_network_command_ptr> p_commands)
		{
			core::bc_json_document<bc_network_packet> l_json_packet;
			l_json_packet->m_time.set(p_time);

			for (auto& l_command : p_commands)
			{
				auto& l_json_command = l_json_packet->m_commands.new_entry();
				*l_json_command->m_hash = l_command->get_message_hash();
				l_command->serialize(*l_json_command->m_values);
			}

			m_serialize_buffer.clear();
			m_serialize_buffer = l_json_packet.write().c_str();

			return std::make_pair(m_serialize_buffer.c_str(), m_serialize_buffer.size());
		}

		std::pair<bc_network_packet_time, core::bc_span<bc_network_command_ptr>> bc_network_command_packet::deserialize(const void* p_buffer, bcUINT32 p_buffer_size)
		{
			core::bc_json_document<bc_network_packet> l_json_packet;
			l_json_packet.load(static_cast<const bcCHAR*>(p_buffer));

			for (auto& l_json_command : l_json_packet->m_commands)
			{
				auto l_command = m_network_system->create_command_instance(*l_json_command->m_hash);
				l_command->deserialize(*l_json_command->m_values);

				m_deserialize_buffer.push_back(std::move(l_command));
			}

			return std::make_pair(*l_json_packet->m_time, core::bc_make_span(m_deserialize_buffer));
		}
	}	
}