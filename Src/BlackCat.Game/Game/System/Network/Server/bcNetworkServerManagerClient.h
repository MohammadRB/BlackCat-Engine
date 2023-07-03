// [10/06/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcSpan.h"
#include "Core/File/bcMemoryStream.h"
#include "Core/Utility/bcValueSampler.h"
#include "PlatformImp/Network/bcNetworkAddress.h"
#include "Game/System/Network/Server/bcNetworkClient.h"
#include "Game/System/Network/bcNetworkMessageAcknowledgeBuffer.h"
#include "Game/System/Network/bcNetworkMessageSerializationBuffer.h"

namespace black_cat
{
	namespace game
	{
		class bc_network_server_manager;
		
		class bc_network_server_manager_client
		{
		public:
			bc_network_server_manager_client(const platform::bc_network_address& p_address, core::bc_string p_name);

			bc_network_server_manager_client(bc_network_server_manager_client&&) noexcept;

			~bc_network_server_manager_client();

			bc_network_server_manager_client& operator=(bc_network_server_manager_client&&) noexcept;

			void lock();

			void unlock();
			
			const platform::bc_network_address& get_address() const noexcept;

			bc_network_client_id get_id() const noexcept;

			core::bc_string_view get_name() const noexcept;

			bool get_ready_for_sync() const noexcept;

			void set_ready_for_sync(bool p_ready) noexcept;

			bcUINT32 get_timeout_elapsed() const noexcept;

			void add_timeout_elapsed(bcUINT32 p_elapsed) noexcept;

			bc_network_packet_time get_last_sync_time() const noexcept;

			void set_last_sync_time(bc_network_packet_time p_time) noexcept;

			bc_network_rtt get_rtt_time() const noexcept;

			bc_network_rtt get_remote_rtt_time() const noexcept;

			void add_rtt_time(bc_network_rtt p_rtt, bc_network_rtt p_remote_rtt) noexcept;

			void set_last_executed_message_id(bc_network_message_id p_id) noexcept;

			bc_network_message_id get_last_executed_message_id() const noexcept;

			core::bc_const_span<bc_network_message_ptr> get_messages() const noexcept;
			
			void add_message(bc_network_message_ptr p_message);

			void clear_messages() noexcept;

			core::bc_span<bc_retry_message> get_messages_waiting_acknowledgment() noexcept;
			
			void add_message_waiting_acknowledgment_if_not_exist(bc_retry_message p_message) noexcept;

			void erase_message_waiting_acknowledgment(bc_network_message_id p_id) noexcept;

			void add_acknowledged_message(bc_network_message_id p_id, core::bc_string p_ack_data) noexcept;

			core::bc_string* find_acknowledge_data(bc_network_message_id p_id) noexcept;
			
			core::bc_span<bc_actor> get_replicated_actors();
			
			void add_replicated_actor(bc_actor p_actor);

			void erase_replicated_actor(bc_actor p_actor);

			bc_network_client to_network_client() const noexcept;

		private:
			platform::bc_mutex m_mutex;
			platform::bc_network_address m_address;
			bc_network_client_id m_id;
			core::bc_string m_name;

			bool m_ready_for_sync;
			bcUINT32 m_timeout_elapsed;
			bc_network_packet_time m_last_sync_time;
			core::bc_value_sampler<bc_network_rtt, 16> m_rtt_sampler;
			bc_network_rtt m_remote_rtt;

			bc_network_message_id m_last_executed_message_id;
			core::bc_vector<bc_network_message_ptr> m_messages;
			core::bc_vector<bc_retry_message> m_messages_waiting_acknowledgment;
			bc_network_message_acknowledge_buffer m_executed_messages;
			
			core::bc_vector<bc_actor> m_replicated_actors;
		};

		inline bc_network_server_manager_client::bc_network_server_manager_client(const platform::bc_network_address& p_address, core::bc_string p_name)
			: m_address(p_address),
			m_name(std::move(p_name)),
			m_ready_for_sync(false),
			m_timeout_elapsed(0),
			m_last_sync_time(0),
			m_rtt_sampler(100),
			m_remote_rtt(100),
			m_last_executed_message_id(0),
			m_executed_messages(50)
		{
			m_id = std::get<bcUINT16>(m_address.get_traits());
		}

		inline bc_network_server_manager_client::bc_network_server_manager_client(bc_network_server_manager_client&& p_other) noexcept
			: m_address(p_other.m_address),
			m_id(p_other.m_id),
			m_name(p_other.m_name),
			m_ready_for_sync(p_other.m_ready_for_sync),
			m_timeout_elapsed(0),
			m_last_sync_time(p_other.m_last_sync_time),
			m_rtt_sampler(p_other.m_rtt_sampler),
			m_remote_rtt(p_other.m_remote_rtt),
			m_last_executed_message_id(p_other.m_last_executed_message_id),
			m_messages(std::move(p_other.m_messages)),
			m_messages_waiting_acknowledgment(std::move(p_other.m_messages_waiting_acknowledgment)),
			m_executed_messages(std::move(p_other.m_executed_messages)),
			m_replicated_actors(std::move(p_other.m_replicated_actors))
		{
		}

		inline bc_network_server_manager_client::~bc_network_server_manager_client() = default;

		inline bc_network_server_manager_client& bc_network_server_manager_client::operator=(bc_network_server_manager_client&& p_other) noexcept
		{
			m_address = p_other.m_address;
			m_id = p_other.m_id;
			m_name = p_other.m_name;
			m_ready_for_sync = p_other.m_ready_for_sync;
			m_timeout_elapsed = p_other.m_timeout_elapsed;
			m_last_sync_time = p_other.m_last_sync_time;
			m_rtt_sampler = p_other.m_rtt_sampler;
			m_remote_rtt = p_other.m_remote_rtt;
			m_last_executed_message_id = p_other.m_last_executed_message_id;
			m_messages = std::move(p_other.m_messages);
			m_messages_waiting_acknowledgment = std::move(p_other.m_messages_waiting_acknowledgment);
			m_executed_messages = std::move(p_other.m_executed_messages);
			m_replicated_actors = std::move(p_other.m_replicated_actors);
			
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

		inline bc_network_client_id bc_network_server_manager_client::get_id() const noexcept
		{
			return m_id;
		}

		inline core::bc_string_view bc_network_server_manager_client::get_name() const noexcept
		{
			return core::bc_string_view(m_name);
		}

		inline bool bc_network_server_manager_client::get_ready_for_sync() const noexcept
		{
			return m_ready_for_sync;
		}

		inline void bc_network_server_manager_client::set_ready_for_sync(bool p_ready) noexcept
		{
			m_ready_for_sync = p_ready;
		}

		inline bcUINT32 bc_network_server_manager_client::get_timeout_elapsed() const noexcept
		{
			return m_timeout_elapsed;
		}

		inline void bc_network_server_manager_client::add_timeout_elapsed(bcUINT32 p_elapsed) noexcept
		{
			m_timeout_elapsed += p_elapsed;
		}

		inline bc_network_packet_time bc_network_server_manager_client::get_last_sync_time() const noexcept
		{
			return m_last_sync_time;
		}

		inline void bc_network_server_manager_client::set_last_sync_time(bc_network_packet_time p_time) noexcept
		{
			m_last_sync_time = p_time;
		}
		
		inline bc_network_rtt bc_network_server_manager_client::get_rtt_time() const noexcept
		{
			return m_rtt_sampler.average_value();
		}
		
		inline bc_network_rtt bc_network_server_manager_client::get_remote_rtt_time() const noexcept
		{
			return m_remote_rtt;
		}

		inline void bc_network_server_manager_client::add_rtt_time(bc_network_rtt p_rtt, bc_network_rtt p_remote_rtt) noexcept
		{
			m_rtt_sampler.add_sample(p_rtt);
			m_remote_rtt = p_remote_rtt;
			m_timeout_elapsed = 0;
		}
		
		inline void bc_network_server_manager_client::set_last_executed_message_id(bc_network_message_id p_id) noexcept
		{
			m_last_executed_message_id = p_id;
		}

		inline bc_network_message_id bc_network_server_manager_client::get_last_executed_message_id() const noexcept
		{
			return m_last_executed_message_id;
		}

		inline core::bc_const_span<bc_network_message_ptr> bc_network_server_manager_client::get_messages() const noexcept
		{
			return core::bc_make_cspan(m_messages);
		}
		
		inline void bc_network_server_manager_client::add_message(bc_network_message_ptr p_message)
		{
			m_messages.push_back(std::move(p_message));
		}

		inline void bc_network_server_manager_client::clear_messages() noexcept
		{
			m_messages.clear();
		}

		inline core::bc_span<bc_retry_message> bc_network_server_manager_client::get_messages_waiting_acknowledgment() noexcept
		{
			return core::bc_make_span(m_messages_waiting_acknowledgment);
		}
		
		inline void bc_network_server_manager_client::add_message_waiting_acknowledgment_if_not_exist(bc_retry_message p_message) noexcept
		{
			const auto l_ite = std::find_if
			(
				std::cbegin(m_messages_waiting_acknowledgment),
				std::cend(m_messages_waiting_acknowledgment),
				[&](const bc_retry_message& p_entry)
				{
					return p_entry.m_message->get_id() == p_message.m_message->get_id();
				}
			);
			if(l_ite != std::cend(m_messages_waiting_acknowledgment))
			{
				// Message is already in the buffer to be acknowledged
				return;
			}
			
			m_messages_waiting_acknowledgment.push_back(std::move(p_message));
		}
		
		inline void bc_network_server_manager_client::erase_message_waiting_acknowledgment(bc_network_message_id p_id) noexcept
		{
			const auto l_msg_ite = std::find_if
			(
				std::begin(m_messages_waiting_acknowledgment),
				std::end(m_messages_waiting_acknowledgment),
				[&](const bc_retry_message& p_msg)
				{
					return p_msg.m_message->get_id() == p_id;
				}
			);
			m_messages_waiting_acknowledgment.erase(l_msg_ite);
		}

		inline void bc_network_server_manager_client::add_acknowledged_message(bc_network_message_id p_id, core::bc_string p_ack_data) noexcept
		{
			m_executed_messages.add_acknowledged_message(p_id, std::move(p_ack_data));
		}

		inline core::bc_string* bc_network_server_manager_client::find_acknowledge_data(bc_network_message_id p_id) noexcept
		{
			return m_executed_messages.find_acknowledge_data(p_id);
		}

		inline core::bc_span<bc_actor> bc_network_server_manager_client::get_replicated_actors()
		{
			return core::bc_make_span(m_replicated_actors);
		}

		inline void bc_network_server_manager_client::add_replicated_actor(bc_actor p_actor)
		{
			m_replicated_actors.push_back(p_actor);
		}

		inline void bc_network_server_manager_client::erase_replicated_actor(bc_actor p_actor)
		{
			const auto l_ite = std::find_if
			(
				std::begin(m_replicated_actors),
				std::end(m_replicated_actors),
				[&](const bc_actor& p_entry)
				{
					return p_entry == p_actor;
				}
			);
			m_replicated_actors.erase(l_ite);
		}

		inline bc_network_client bc_network_server_manager_client::to_network_client() const noexcept
		{
			return bc_network_client{ get_address(), get_id(), get_name() };
		}
	}	
}