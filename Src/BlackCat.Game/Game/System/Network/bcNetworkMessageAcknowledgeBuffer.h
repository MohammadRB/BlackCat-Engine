// [24/09/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/System/Network/bcNetworkDefinitions.h"

namespace black_cat
{
	namespace game
	{
		class bc_network_message_acknowledge_buffer
		{
		public:
			explicit bc_network_message_acknowledge_buffer(bcSIZE p_buffer_size);

			bc_network_message_acknowledge_buffer(bc_network_message_acknowledge_buffer&&) noexcept;

			~bc_network_message_acknowledge_buffer();

			bc_network_message_acknowledge_buffer& operator=(bc_network_message_acknowledge_buffer&&) noexcept;

			void add_acknowledged_message(bc_network_message_id p_id, core::bc_string p_acknowledge_data) noexcept;

			core::bc_string* find_acknowledge_data(bc_network_message_id p_id) noexcept;

		private:
			core::bc_vector<std::pair<bc_network_message_id, core::bc_string>> m_ids;
			bcSIZE m_last_id;
		};

		inline bc_network_message_acknowledge_buffer::bc_network_message_acknowledge_buffer(bcSIZE p_buffer_size)
			: m_ids(p_buffer_size, std::make_pair(static_cast<bc_network_message_id>(-1), core::bc_string())),
			m_last_id(0)
		{
		}

		inline bc_network_message_acknowledge_buffer::bc_network_message_acknowledge_buffer(bc_network_message_acknowledge_buffer&&) noexcept = default;

		inline bc_network_message_acknowledge_buffer::~bc_network_message_acknowledge_buffer() = default;

		inline bc_network_message_acknowledge_buffer& bc_network_message_acknowledge_buffer::operator=(bc_network_message_acknowledge_buffer&&) noexcept = default;

		inline void bc_network_message_acknowledge_buffer::add_acknowledged_message(bc_network_message_id p_id, core::bc_string p_acknowledge_data) noexcept
		{
			m_ids[m_last_id] = std::make_pair(p_id, std::move(p_acknowledge_data));
			m_last_id = (m_last_id + 1) % m_ids.size();
		}

		inline core::bc_string* bc_network_message_acknowledge_buffer::find_acknowledge_data(bc_network_message_id p_id) noexcept
		{
			auto l_ite = std::find_if
			(
				std::begin(m_ids),
				std::end(m_ids),
				[=](const auto& p_entry)
				{
					return p_entry.first == p_id;
				}
			);
			if(l_ite != std::end(m_ids))
			{
				return &l_ite->second;
			}

			return nullptr;
		}
	}
}