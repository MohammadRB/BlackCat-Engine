// [06/10/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
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
		
		class bc_network_server_manager_client
		{
		public:
			explicit bc_network_server_manager_client(const platform::bc_network_address& p_address);

			bc_network_server_manager_client(bc_network_server_manager_client&&) noexcept;

			~bc_network_server_manager_client();

			bc_network_server_manager_client& operator=(bc_network_server_manager_client&&) noexcept;

			void lock();

			void unlock();
			
			const platform::bc_network_address& get_address() const noexcept;
			
			bc_network_packet_time get_last_sync_time() const noexcept;

			void set_last_sync_time(bc_network_packet_time p_time) noexcept;

			bc_network_packet_time get_rtt_time() const noexcept;

			void add_rtt_time(bc_network_packet_time p_time) noexcept;

			void set_last_executed_message_id(bc_network_message_id p_id) noexcept;

			bc_network_message_id get_last_executed_message_id() const noexcept;
			
			void add_message(bc_network_message_ptr p_message);

			core::bc_span<bc_network_message_ptr> get_messages() noexcept;

			void clear_messages() noexcept;
			
			void add_message_with_acknowledgment(bc_message_with_time p_message) noexcept;
			
			core::bc_span<bc_message_with_time> get_messages_waiting_acknowledgment() noexcept;

			void erase_message_with_acknowledgment(bc_network_message_id p_id) noexcept;
			
		private:
			core_platform::bc_mutex m_mutex;
			platform::bc_network_address m_address;
			bc_network_packet_time m_last_sync_time;
			core::bc_value_sampler<bc_network_packet_time, 64> m_rtt_sampler;

			bc_network_message_id m_last_executed_message_id;
			core::bc_vector<bc_network_message_ptr> m_messages;
			core::bc_vector<bc_message_with_time> m_messages_waiting_acknowledgment;
		};

		inline bc_network_server_manager_client::bc_network_server_manager_client(const platform::bc_network_address& p_address)
			: m_address(p_address),
			m_last_sync_time(0),
			m_rtt_sampler(100),
			m_last_executed_message_id(0)
		{
		}

		inline bc_network_server_manager_client::bc_network_server_manager_client(bc_network_server_manager_client&& p_other) noexcept
			: m_address(std::move(p_other.m_address)),
			m_last_sync_time(p_other.m_last_sync_time),
			m_rtt_sampler(std::move(p_other.m_rtt_sampler)),
			m_last_executed_message_id(p_other.m_last_executed_message_id),
			m_messages(std::move(p_other.m_messages)),
			m_messages_waiting_acknowledgment(std::move(p_other.m_messages_waiting_acknowledgment))
		{
		}

		inline bc_network_server_manager_client::~bc_network_server_manager_client() = default;

		inline bc_network_server_manager_client& bc_network_server_manager_client::operator=(bc_network_server_manager_client&& p_other) noexcept
		{
			m_address = std::move(p_other.m_address);
			m_last_sync_time = p_other.m_last_sync_time;
			m_rtt_sampler = std::move(p_other.m_rtt_sampler);
			m_last_executed_message_id = p_other.m_last_executed_message_id;
			m_messages = std::move(p_other.m_messages);
			m_messages_waiting_acknowledgment = std::move(p_other.m_messages_waiting_acknowledgment);
			
			return *this;
		}

		inline void bc_network_server_manager_client::lock()
		{
			m_mutex.lock();
		}

		inline void bc_network_server_manager_client::unlock()
		{
			m_mutex.unlock();
		}

		inline const platform::bc_network_address& bc_network_server_manager_client::get_address() const noexcept
		{
			return m_address;
		}

		inline bc_network_packet_time bc_network_server_manager_client::get_last_sync_time() const noexcept
		{
			return m_last_sync_time;
		}

		inline void bc_network_server_manager_client::set_last_sync_time(bc_network_packet_time p_time) noexcept
		{
			m_last_sync_time = p_time;
		}
		
		inline bc_network_packet_time bc_network_server_manager_client::get_rtt_time() const noexcept
		{
			return m_rtt_sampler.average_value();
		}

		inline void bc_network_server_manager_client::add_rtt_time(bc_network_packet_time p_time) noexcept
		{
			m_rtt_sampler.add_sample(p_time);
		}

		inline void bc_network_server_manager_client::set_last_executed_message_id(bc_network_message_id p_id) noexcept
		{
			m_last_executed_message_id = p_id;
		}

		inline bc_network_message_id bc_network_server_manager_client::get_last_executed_message_id() const noexcept
		{
			return m_last_executed_message_id;
		}

		inline void bc_network_server_manager_client::add_message(bc_network_message_ptr p_message)
		{
			m_messages.push_back(std::move(p_message));
		}

		inline core::bc_span<bc_network_message_ptr> bc_network_server_manager_client::get_messages() noexcept
		{
			return core::bc_make_span(m_messages);
		}

		inline void bc_network_server_manager_client::clear_messages() noexcept
		{
			m_messages.clear();
		}

		inline void bc_network_server_manager_client::add_message_with_acknowledgment(bc_message_with_time p_message) noexcept
		{
			m_messages_waiting_acknowledgment.push_back(std::move(p_message));
		}
		
		inline core::bc_span<bc_message_with_time> bc_network_server_manager_client::get_messages_waiting_acknowledgment() noexcept
		{
			return core::bc_make_span(m_messages_waiting_acknowledgment);
		}

		inline void bc_network_server_manager_client::erase_message_with_acknowledgment(bc_network_message_id p_id) noexcept
		{
			const auto l_msg_ite = std::find_if
			(
				std::begin(m_messages_waiting_acknowledgment),
				std::end(m_messages_waiting_acknowledgment),
				[&](const bc_message_with_time& p_msg)
				{
					return p_msg.m_message->get_id() == p_id;
				}
			);
			m_messages_waiting_acknowledgment.erase(l_msg_ite);
		}
	}	
}