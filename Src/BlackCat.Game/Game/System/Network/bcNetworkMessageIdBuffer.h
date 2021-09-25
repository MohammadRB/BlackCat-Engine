// [09/24/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/System/Network/bcNetworkDefinitions.h"

namespace black_cat
{
	namespace game
	{
		class bc_network_message_id_buffer
		{
		public:
			explicit bc_network_message_id_buffer(bcSIZE p_buffer_size);

			bc_network_message_id_buffer(bc_network_message_id_buffer&&) noexcept;

			~bc_network_message_id_buffer();

			bc_network_message_id_buffer& operator=(bc_network_message_id_buffer&&) noexcept;

			void add_id(bc_network_message_id p_id) noexcept;

			bool id_exist(bc_network_message_id p_id) noexcept;

		private:
			core::bc_vector_movable<bc_network_message_id> m_ids;
			bcSIZE m_last_id;
		};

		inline bc_network_message_id_buffer::bc_network_message_id_buffer(bcSIZE p_buffer_size)
			: m_ids(p_buffer_size, -1),
			m_last_id(0)
		{
		}

		inline bc_network_message_id_buffer::bc_network_message_id_buffer(bc_network_message_id_buffer&&) noexcept = default;

		inline bc_network_message_id_buffer::~bc_network_message_id_buffer() = default;

		inline bc_network_message_id_buffer& bc_network_message_id_buffer::operator=(bc_network_message_id_buffer&&) noexcept = default;

		inline void bc_network_message_id_buffer::add_id(bc_network_message_id p_id) noexcept
		{
			m_ids[m_last_id] = p_id;
			m_last_id = (m_last_id + 1) % m_ids.size();
		}

		inline bool bc_network_message_id_buffer::id_exist(bc_network_message_id p_id) noexcept
		{
			return std::find(std::begin(m_ids), std::end(m_ids), p_id) != std::end(m_ids);
		}
	}
}