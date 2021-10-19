// [07/08/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		bc_actor_replicate_network_message::bc_actor_replicate_network_message()
			: bci_network_message(message_name()),
			m_deserialization_visitor(nullptr)
		{
		}

		bc_actor_replicate_network_message::bc_actor_replicate_network_message(const bc_actor& p_actor)
			: bci_network_message(message_name()),
			m_actor(p_actor),
			m_deserialization_visitor(nullptr)
		{
		}

		bc_actor_replicate_network_message::bc_actor_replicate_network_message(bc_actor_replicate_network_message&&) noexcept = default;

		bc_actor_replicate_network_message::~bc_actor_replicate_network_message() = default;

		bc_actor_replicate_network_message& bc_actor_replicate_network_message::operator=(bc_actor_replicate_network_message&&) noexcept = default;

		core::bc_string bc_actor_replicate_network_message::get_acknowledgment_data() const noexcept
		{
			if(!m_actor.is_valid())
			{
				return {};
			}
			
			const auto* l_network_component = m_actor.get_component<bc_network_component>();
			const auto l_network_id = l_network_component->get_network_id();
			return core::bc_to_string(l_network_id);
		}

		void bc_actor_replicate_network_message::execute(const bc_network_message_client_context& p_context) noexcept
		{
			if(!_create_actor())
			{
				return;
			}
			p_context.m_visitor.replicate_actor(m_actor);
		}

		void bc_actor_replicate_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
			if (!_create_actor())
			{
				return;
			}
			p_context.m_visitor.replicate_actor(p_context.m_address, m_actor);
		}

		void bc_actor_replicate_network_message::acknowledge(const bc_network_message_client_acknowledge_context& p_context) noexcept
		{
			const auto l_network_id = core::bc_stoi(p_context.m_ack_data);
			auto* l_network_component = m_actor.get_component<bc_network_component>();

			l_network_component->set_network_id(l_network_id);
			p_context.m_visitor.replicate_actor(m_actor);
		}

		void bc_actor_replicate_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
		{
			p_context.m_params.reserve(10);

			/*const bcCHAR* l_entity_name;

			if(p_context.m_visitor.get_network_type() == bc_network_type::server)
			{
				const auto* l_network_component = m_actor.get_component<bc_network_component>();
				l_entity_name = l_network_component->get_network_entity_name();
			}
			else
			{
				const auto* l_mediate_component = m_actor.get_component<bc_mediate_component>();
				l_entity_name = l_mediate_component->get_entity_name();
			}*/

			const auto* l_network_component = m_actor.get_component<bc_network_component>();
			const bcCHAR* l_entity_name = l_network_component->get_network_entity_name();
			p_context.m_params.add("ent", core::bc_any(core::bc_string(l_entity_name)));

			core::bc_vector_frame<bci_actor_component*> l_components(10);
			const bc_actor_component_network_write_context l_context(p_context.m_params, m_actor);
			
			bc_actor_write_network_instance(l_components, l_context);
		}

		void bc_actor_replicate_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
		{
			m_deserialization_visitor = &p_context.m_visitor;
			m_deserialization_values = p_context.m_params;
		}

		bool bc_actor_replicate_network_message::_create_actor()
		{
			const auto* l_entity_name = m_deserialization_values.find("ent")->second.as<core::bc_string>();
			if (!l_entity_name)
			{
				core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize actor entity name in replicate network message"));
				return false;
			}

			m_actor = m_deserialization_visitor->create_actor(l_entity_name->c_str());

			core::bc_vector_frame<bci_actor_component*> l_components(10);
			bc_actor_load_network_instance(l_components, bc_actor_component_network_load_context(m_deserialization_values, m_actor));

			return true;
		}
	}
}