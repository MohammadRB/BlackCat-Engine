// [08/19/2021 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/Math/bcMatrix4f.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Network/bcNetworkDefinitions.h"

namespace black_cat
{
	namespace game
	{
		class bci_network_message_serialization_visitor
		{
		public:
			virtual bc_network_type get_network_type() const noexcept = 0;

		protected:
			bci_network_message_serialization_visitor() = default;

			virtual ~bci_network_message_serialization_visitor() = default;
		};
		
		class bci_network_message_deserialization_visitor
		{
		public:
			struct bc_replicated_actor
			{
				bc_actor m_actor;
				bool m_is_self_replicated;

				operator std::tuple<bc_actor&, bool&>() noexcept
				{
					return std::tie(m_actor, m_is_self_replicated);
				}
			};
		
		public:
			virtual bc_network_type get_network_type() const noexcept = 0;
			
			virtual bc_actor create_actor(const bcCHAR* p_entity_name, const core::bc_matrix4f& p_transform) = 0;

			virtual bc_replicated_actor get_actor(bc_actor_network_id p_actor_network_id) = 0;

		protected:
			bci_network_message_deserialization_visitor() = default;

			virtual ~bci_network_message_deserialization_visitor() = default;
		};

		class bci_network_message_server_visitor
		{
		public:
			virtual void get_rtt_time(const platform::bc_network_address& p_address, bc_network_rtt* p_rtt, bc_network_rtt* p_remote_rtt) noexcept = 0;
			
			virtual void add_rtt_sample(const platform::bc_network_address& p_address, bc_network_rtt p_rtt, bc_network_rtt p_remote_rtt) noexcept = 0;
			
			virtual core::bc_string client_connected(const platform::bc_network_address& p_address, core::bc_string p_name) = 0;

			virtual void client_disconnected(const platform::bc_network_address& p_address) = 0;

			virtual void acknowledge_message(const platform::bc_network_address& p_address, bc_network_message_id p_ack_id, core::bc_string p_ack_data) = 0;

			virtual void replicate_scene(const platform::bc_network_address& p_address) = 0;

			virtual void replicate_actor(const platform::bc_network_address& p_address, bc_actor& p_actor) = 0;

			virtual void remove_actor(const platform::bc_network_address& p_address, bc_actor& p_actor) = 0;
		
		protected:
			bci_network_message_server_visitor() = default;

			virtual ~bci_network_message_server_visitor() = default;
		};

		class bci_network_message_client_visitor
		{
		public:
			virtual void get_rtt_time(bc_network_rtt* p_rtt, bc_network_rtt* p_remote_rtt) noexcept = 0;
			
			virtual void add_rtt_sample(bc_network_rtt p_rtt, bc_network_rtt p_remote_rtt) noexcept = 0;
			
			virtual void connection_approved(core::bc_string p_error_message) = 0;

			virtual void acknowledge_message(bc_network_message_id p_ack_id, core::bc_string p_ack_data) = 0;

			virtual void load_scene(const bcECHAR* p_scene_name) = 0;

			virtual void replicate_actor(bc_actor& p_actor) = 0;

			virtual void remove_actor(bc_actor& p_actor) = 0;

		protected:
			bci_network_message_client_visitor() = default;

			virtual ~bci_network_message_client_visitor() = default;
		};
	}
}
