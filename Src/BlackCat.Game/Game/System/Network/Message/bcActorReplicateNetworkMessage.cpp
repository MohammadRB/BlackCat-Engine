// [07/08/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		bc_actor_replicate_network_message::bc_actor_replicate_network_message()
			: bci_network_message(message_name())
		{
		}

		bc_actor_replicate_network_message::bc_actor_replicate_network_message(const bc_actor& p_actor)
			: bci_network_message(message_name()),
			m_actor(p_actor)
		{
		}

		bc_actor_replicate_network_message::bc_actor_replicate_network_message(bc_actor_replicate_network_message&&) noexcept = default;

		bc_actor_replicate_network_message::~bc_actor_replicate_network_message() = default;

		bc_actor_replicate_network_message& bc_actor_replicate_network_message::operator=(bc_actor_replicate_network_message&&) noexcept = default;

		void bc_actor_replicate_network_message::execute(const bc_network_message_client_context& p_context) noexcept
		{
			p_context.m_visitor.replicate_actor(m_actor);
		}

		void bc_actor_replicate_network_message::execute(const bc_network_message_server_context& p_context) noexcept
		{
		}

		void bc_actor_replicate_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
		{
			p_context.m_params.reserve(10);
			
			const auto* l_mediate_component = m_actor.get_component<bc_mediate_component>();
			p_context.m_params.add("ent", core::bc_any(core::bc_string_frame(l_mediate_component->get_entity_name())));

			core::bc_vector_frame<bci_actor_component*> l_components(10);
			const bc_actor_component_network_write_context l_context(p_context.m_params, m_actor);
			
			bc_actor_write_network_instance(l_components, l_context);
		}

		void bc_actor_replicate_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
		{
			const auto* l_entity_name = p_context.m_params.find("ent")->second.as<core::bc_string>();
			if(!l_entity_name)
			{
				core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize actor entity name in replicate network message"));
				return;
			}

			m_actor = p_context.m_visitor.create_actor(l_entity_name->c_str());

			core::bc_vector_frame<bci_actor_component*> l_components(10);
			bc_actor_load_network_instance(l_components, bc_actor_component_network_load_context(p_context.m_params, m_actor));
		}
	}
}