// [10/01/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/System/Network/bcNetworkDefinitions.h"
#include "Game/System/bcGameSystem.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_network_controller : public bci_actor_controller
		{
		protected:
			bc_network_component& get_network_component() const noexcept;

			bc_network_type get_network_type() const noexcept;

			bc_actor_network_data_dir get_network_data_dir() const noexcept;

			bc_actor_replication_side get_replication_side() const noexcept;

			bc_actor_network_id get_actor_network_id() const noexcept;

			bc_network_client_id get_network_client_id() const noexcept;

			bc_network_rtt get_out_ping() const noexcept;

			bc_network_rtt get_in_ping() const noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void load_network_instance(const bc_actor_component_network_load_context& p_context) final override;

			void write_network_instance(const bc_actor_component_network_write_context& p_context) final override;

			void update(const bc_actor_component_update_content& p_context) final override;

		private:
			virtual void load_origin_network_instance(const bc_actor_component_network_load_context& p_context) = 0;

			virtual void load_replicated_network_instance(const bc_actor_component_network_load_context& p_context) = 0;

			virtual void write_origin_network_instance(const bc_actor_component_network_write_context& p_context) = 0;

			virtual void write_replicated_network_instance(const bc_actor_component_network_write_context& p_context) = 0;

			virtual void update_origin_instance(const bc_actor_component_update_content& p_context) = 0;

			virtual void update_replicated_instance(const bc_actor_component_update_content& p_context) = 0;

			bc_network_component* m_network_component {nullptr};
			bc_actor_replication_side m_replication_side {bc_actor_replication_side::origin};
		};

		inline void bc_actor_network_controller::initialize(const bc_actor_component_initialize_context& p_context)
		{
			m_network_component = p_context.m_actor.get_component<bc_network_component>();
			if (!m_network_component)
			{
				throw bc_invalid_operation_exception("network controller must have network component");
			}

			const auto l_network_type = p_context.m_game_system.get_network_system().get_network_type();
			const auto l_network_data_dir = m_network_component->get_network_data_dir();
						
			if(l_network_type == bc_network_type::not_started)
			{
				return;
			}
			
			if
			(
				(l_network_type == bc_network_type::server && l_network_data_dir == bc_actor_network_data_dir::replicate_sync) ||
				(l_network_type == bc_network_type::client && l_network_data_dir == bc_actor_network_data_dir::replicate_sync_from_client)
			)
			{
				m_replication_side = bc_actor_replication_side::origin;
			}
			else
			{
				m_replication_side = bc_actor_replication_side::replicated;
			}
		}

		inline void bc_actor_network_controller::load_network_instance(const bc_actor_component_network_load_context& p_context)
		{
			if(m_replication_side == bc_actor_replication_side::origin)
			{
				load_origin_network_instance(p_context);
			}
			else
			{
				load_replicated_network_instance(p_context);
			}
		}

		inline void bc_actor_network_controller::write_network_instance(const bc_actor_component_network_write_context& p_context)
		{
			if (m_replication_side == bc_actor_replication_side::origin)
			{
				write_origin_network_instance(p_context);
			}
			else
			{
				write_replicated_network_instance(p_context);
			}
		}

		inline void bc_actor_network_controller::update(const bc_actor_component_update_content& p_context)
		{
			if (m_replication_side == bc_actor_replication_side::origin)
			{
				update_origin_instance(p_context);
			}
			else
			{
				update_replicated_instance(p_context);
			}
		}

		inline bc_network_component& bc_actor_network_controller::get_network_component() const noexcept
		{
			return *m_network_component;
		}

		inline bc_network_type bc_actor_network_controller::get_network_type() const noexcept
		{
			return m_network_component->get_network_type();
		}

		inline bc_actor_network_data_dir bc_actor_network_controller::get_network_data_dir() const noexcept
		{
			return m_network_component->get_network_data_dir();
		}

		inline bc_actor_replication_side bc_actor_network_controller::get_replication_side() const noexcept
		{
			return m_replication_side;
		}

		inline bc_actor_network_id bc_actor_network_controller::get_actor_network_id() const noexcept
		{
			return m_network_component->get_network_id();
		}

		inline bc_network_client_id bc_actor_network_controller::get_network_client_id() const noexcept
		{
			return m_network_component->get_network_client_id();
		}

		inline bc_network_rtt bc_actor_network_controller::get_out_ping() const noexcept
		{
			return m_network_component->get_out_ping();
		}

		inline bc_network_rtt bc_actor_network_controller::get_in_ping() const noexcept
		{
			return m_network_component->get_in_ping();
		}
	}	
}