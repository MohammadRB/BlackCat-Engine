// [06/10/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcSpan.h"
#include "Core/File/bcMemoryStream.h"
#include "Core/Math/bcValueSampler.h"
#include "PlatformImp/Network/bcNetworkAddress.h"
#include "Game/System/Network/bcNetworkMessageBuffer.h"

namespace black_cat
{
	namespace game
	{
		class bc_network_server_manager;
		
		class bc_network_server_manager_client1
		{
		public:
			bc_network_server_manager_client1(const platform::bc_network_address& p_address);

			bc_network_server_manager_client1(bc_network_server_manager_client1&&) noexcept;

			~bc_network_server_manager_client1();

			bc_network_server_manager_client1& operator=(bc_network_server_manager_client1&&) noexcept;

			const platform::bc_network_address& get_address() const noexcept;
			
			bc_network_packet_time get_last_sync_time() const noexcept;

			void set_last_sync_time(bc_network_packet_time p_time) noexcept;

			bc_network_packet_time get_rtt_time() const noexcept;

			void add_rtt_time(bc_network_packet_time p_time) noexcept;

			void add_message(bc_network_message_ptr p_message);

			core::bc_span<bc_network_message_ptr> get_messages() noexcept;

			void clear_messages() noexcept;
			
			void add_message_with_acknowledgment(bc_network_message_ptr p_message) noexcept;
			
			core::bc_span<bc_network_message_ptr> get_messages_waiting_acknowledgment() noexcept;
			
		private:
			platform::bc_network_address m_address;
			bc_network_packet_time m_last_sync_time;
			core::bc_value_sampler<bc_network_packet_time, 64> m_rtt_sampler;

			core::bc_vector<bc_network_message_ptr> m_messages;
			core::bc_vector<bc_network_message_ptr> m_messages_waiting_acknowledgment;
		};

		inline bc_network_server_manager_client1::bc_network_server_manager_client1(const platform::bc_network_address& p_address)
			: m_address(p_address),
			m_last_sync_time(0),
			m_rtt_sampler(100)
		{
		}

		inline bc_network_server_manager_client1::bc_network_server_manager_client1(bc_network_server_manager_client1&&) noexcept = default;

		inline bc_network_server_manager_client1::~bc_network_server_manager_client1() = default;

		inline bc_network_server_manager_client1& bc_network_server_manager_client1::operator=(bc_network_server_manager_client1&&) noexcept = default;

		inline const platform::bc_network_address& bc_network_server_manager_client1::get_address() const noexcept
		{
			return m_address;
		}

		inline bc_network_packet_time bc_network_server_manager_client1::get_last_sync_time() const noexcept
		{
			return m_last_sync_time;
		}

		inline void bc_network_server_manager_client1::set_last_sync_time(bc_network_packet_time p_time) noexcept
		{
			m_last_sync_time = p_time;
		}
		
		inline bc_network_packet_time bc_network_server_manager_client1::get_rtt_time() const noexcept
		{
			return m_rtt_sampler.average_value();
		}

		inline void bc_network_server_manager_client1::add_rtt_time(bc_network_packet_time p_time) noexcept
		{
			m_rtt_sampler.add_sample(p_time);
		}

		inline void bc_network_server_manager_client1::add_message(bc_network_message_ptr p_message)
		{
			m_messages.push_back(std::move(p_message));
		}

		inline core::bc_span<bc_network_message_ptr> bc_network_server_manager_client1::get_messages() noexcept
		{
			return core::bc_make_span(m_messages);
		}

		inline void bc_network_server_manager_client1::clear_messages() noexcept
		{
			m_messages.clear();
		}

		inline void bc_network_server_manager_client1::add_message_with_acknowledgment(bc_network_message_ptr p_message) noexcept
		{
			m_messages_waiting_acknowledgment.push_back(std::move(p_message));
		}
		
		inline core::bc_span<bc_network_message_ptr> bc_network_server_manager_client1::get_messages_waiting_acknowledgment() noexcept
		{
			return core::bc_make_span(m_messages_waiting_acknowledgment);
		}
	}	
}