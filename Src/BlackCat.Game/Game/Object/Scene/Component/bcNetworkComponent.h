// [05/27/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcValueChangeRateSampler.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/System/Network/Server/bcNetworkClient.h"
#include "Game/System/Network/bcNetworkDefinitions.h"
#include "Game/System/Network/bcNetworkManager.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		template<typename T>
		using bc_network_extrapolation = core::bc_value_change_rate_sampler<T, 80, 15, 5>;
		
		using bc_network_v3_extrapolation = bc_network_extrapolation<core::bc_vector3f>;

		class BC_GAME_DLL bc_network_component : public bci_actor_component
		{
			BC_COMPONENT(ntwr, true, false)
			friend class bci_network_manager;
			
		public:
			bc_network_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept;

			bc_network_component(bc_network_component&&) noexcept;

			~bc_network_component() override;

			bc_network_component& operator=(bc_network_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			bc_network_client_id get_network_client_id() const noexcept;

			bc_actor_network_id get_network_id() const noexcept;
			
			void set_network_ids(bc_network_client_id p_network_client_id, bc_actor_network_id p_network_id) noexcept;

			void set_as_invalid_network_state() noexcept;

			bc_network_type get_network_type() const noexcept;
			
			bc_actor_network_data_dir get_network_data_dir() const noexcept;

			bc_actor_replication_side get_network_replication_side() const noexcept;

			const bcCHAR* get_network_entity_name() const noexcept;

			bc_network_rtt get_out_ping() const noexcept;

			bc_network_rtt get_in_ping() const noexcept;

			void set_ping(bc_network_rtt p_out_ping, bc_network_rtt p_in_ping) noexcept;

			bool get_sync_enabled() const noexcept;

			void set_sync_enabled(bool p_value) noexcept;
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;

			void load_network_instance(const bc_actor_component_network_load_context& p_context) override;
			
			void write_network_instance(const bc_actor_component_network_write_context& p_context) override;

			void update(const bc_actor_component_update_content& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;
			
			void add_extrapolating_value(const bcCHAR* p_name, const core::bc_vector3f& p_value);

			/**
			 * \brief Get property rate of change based on ping time
			 * \param p_name 
			 * \return 
			 */
			std::pair<bool, core::bc_vector3f> get_extrapolated_value(const bcCHAR* p_name) const noexcept;
			
			/**
			 * \brief Get property rate of change based on ping time scaled with elapsed clock.
			 * \n Also add sample to extrapolator if no new sample was provided
			 * \param p_name 
			 * \param p_clock 
			 * \return 
			 */
			std::pair<bool, core::bc_vector3f> get_extrapolated_value(const bcCHAR* p_name, const platform::bc_clock::update_param& p_clock) const noexcept;

		private:
			bc_network_client_id m_network_client_id;
			bc_actor_network_id m_network_id;
			bc_network_type m_network_type;
			bc_actor_network_data_dir m_data_dir;
			bc_actor_replication_side m_replication_side{ bc_actor_replication_side::origin };
			bool m_sync_enabled;
			const bcCHAR* m_network_entity_name;
			bc_network_rtt m_out_ping;
			bc_network_rtt m_in_ping;
			core::bc_vector<std::tuple<const bcCHAR*, bc_network_v3_extrapolation, platform::bc_clock::small_time>> m_extrapolators;
		};

		inline bc_network_client_id bc_network_component::get_network_client_id() const noexcept
		{
			return m_network_client_id;
		}
		
		inline bc_actor_network_id bc_network_component::get_network_id() const noexcept
		{
			return m_network_id;
		}
		
		inline void bc_network_component::set_network_ids(bc_network_client_id p_network_client_id, bc_actor_network_id p_network_id) noexcept
		{
			m_network_client_id = p_network_client_id;
			m_network_id = p_network_id;
		}

		inline void bc_network_component::set_as_invalid_network_state() noexcept
		{
			m_network_id = g_invalid_actor_network_id;
		}

		inline bc_network_type bc_network_component::get_network_type() const noexcept
		{
			return m_network_type;
		}
		
		inline bc_actor_network_data_dir bc_network_component::get_network_data_dir() const noexcept
		{
			return m_data_dir;
		}

		inline bc_actor_replication_side bc_network_component::get_network_replication_side() const noexcept
		{
			return m_replication_side;
		}

		inline const bcCHAR* bc_network_component::get_network_entity_name() const noexcept
		{
			return m_network_entity_name;
		}
		
		inline bc_network_rtt bc_network_component::get_out_ping() const noexcept
		{
			return m_out_ping;
		}

		inline bc_network_rtt bc_network_component::get_in_ping() const noexcept
		{
			return m_in_ping;
		}

		inline void bc_network_component::set_ping(bc_network_rtt p_out_ping, bc_network_rtt p_in_ping) noexcept
		{
			m_out_ping = p_out_ping;
			m_in_ping = p_in_ping;
		}

		inline bool bc_network_component::get_sync_enabled() const noexcept
		{
			return m_sync_enabled;
		}

		inline void bc_network_component::set_sync_enabled(bool p_value) noexcept
		{
			m_sync_enabled = p_value;
		}
	}
}