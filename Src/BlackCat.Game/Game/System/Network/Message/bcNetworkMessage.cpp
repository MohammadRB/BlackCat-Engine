// [31/05/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"

namespace black_cat::game
{
	platform::bc_atomic<bc_network_message_id> bci_network_message::s_id_counter(1);

	core::bc_string bci_network_message::get_acknowledgment_data() const noexcept
	{
		return {};
	}

	void bci_network_message::serialize(const bc_network_message_serialization_context& p_context)
	{
		if(m_id == 0)
		{
			m_id = s_id_counter.fetch_add(1, platform::bc_memory_order::relaxed);
		}

		p_context.m_params.add_or_update("msg_id", core::bc_any(m_id));
			
		if(m_is_retry)
		{
			p_context.m_params.add_or_update("is_rty", core::bc_any(m_is_retry));
		}

		serialize_message(p_context);
	}

	void bci_network_message::deserialize(const bc_network_message_deserialization_context& p_context)
	{
		// we should use cast function because message id is defined to be uint32 but in json serialization it will be written as int32
		auto [l_has_id, l_id] = p_context.m_params.find("msg_id")->second.cast_to_int();
		if(!l_has_id)
		{
			core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize network message id"));
			return;
		}

		m_id = static_cast<bc_network_message_id>(l_id);

		const auto l_is_retry_param = p_context.m_params.find("is_rty");
		if(l_is_retry_param != std::end(p_context.m_params))
		{
			const auto* l_is_retry = l_is_retry_param->second.as<bool>();
			m_is_retry = l_is_retry ? *l_is_retry : false;
		}
			
		deserialize_message(p_context);
	}

	void bci_network_message::execute(const bc_network_message_client_context& p_context) noexcept
	{
	}

	void bci_network_message::execute(const bc_network_message_server_context& p_context) noexcept
	{
	}

	void bci_network_message::acknowledge(const bc_network_message_client_acknowledge_context& p_context) noexcept
	{
	}

	void bci_network_message::acknowledge(const bc_network_message_server_acknowledge_context& p_context) noexcept
	{
	}

	bci_network_message::bci_network_message(const bcCHAR* p_name) noexcept
		: bci_message(p_name),
		  m_id(0),
		  m_is_retry(false)
	{
	}
}
