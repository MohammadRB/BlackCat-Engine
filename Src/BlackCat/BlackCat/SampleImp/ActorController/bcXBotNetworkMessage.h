// [10/20/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_xbot_weapon_attach_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(xbw_ach)
		
	public:
		bc_xbot_weapon_attach_network_message() noexcept;

		bc_xbot_weapon_attach_network_message(const game::bc_actor& p_actor, const game::bc_actor& p_weapon_actor) noexcept;

		bc_xbot_weapon_attach_network_message(bc_xbot_weapon_attach_network_message&&) noexcept;

		~bc_xbot_weapon_attach_network_message() override;

		bc_xbot_weapon_attach_network_message& operator=(bc_xbot_weapon_attach_network_message&&) noexcept;

		bool need_acknowledgment() const noexcept override;

		void execute(const game::bc_network_message_server_context& p_context) noexcept override;

		void execute(const game::bc_network_message_client_context& p_context) noexcept override;
		
	private:
		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;
		
		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;
		
		game::bc_actor_network_id m_actor_net_id;
		core::bc_string m_weapon_entity_name;
		game::bc_actor m_actor;
		bool m_is_self_replicate;
	};

	class BC_DLL bc_xbot_weapon_detach_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(xbw_dch)

	public:
		bc_xbot_weapon_detach_network_message() noexcept;

		explicit bc_xbot_weapon_detach_network_message(const game::bc_actor& p_actor) noexcept;

		bc_xbot_weapon_detach_network_message(bc_xbot_weapon_detach_network_message&&) noexcept;

		~bc_xbot_weapon_detach_network_message() override;

		bc_xbot_weapon_detach_network_message& operator=(bc_xbot_weapon_detach_network_message&&) noexcept;

		bool need_acknowledgment() const noexcept override;

		void execute(const game::bc_network_message_server_context& p_context) noexcept override;

		void execute(const game::bc_network_message_client_context& p_context) noexcept override;
		
	private:
		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;

		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;

		game::bc_actor_network_id m_actor_net_id;
		game::bc_actor m_actor;
		bool m_is_self_replicate;
	};

	class BC_DLL bc_xbot_weapon_shoot_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(xbw_sht)

	public:
		bc_xbot_weapon_shoot_network_message() noexcept;

		explicit bc_xbot_weapon_shoot_network_message(const game::bc_actor& p_actor) noexcept;

		bc_xbot_weapon_shoot_network_message(bc_xbot_weapon_shoot_network_message&&) noexcept;

		~bc_xbot_weapon_shoot_network_message() override;

		bc_xbot_weapon_shoot_network_message& operator=(bc_xbot_weapon_shoot_network_message&&) noexcept;

		bool need_acknowledgment() const noexcept override;

		void execute(const game::bc_network_message_server_context& p_context) noexcept override;

		void execute(const game::bc_network_message_client_context& p_context) noexcept override;
		
	private:
		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;

		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;

		game::bc_actor_network_id m_actor_net_id;
		game::bc_actor m_actor;
		bool m_is_self_replicate;
	};
}