// [05/27/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Math/bcVector3f.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/System/Network/bcNetworkDefinitions.h"
#include "Game/System/Network/bcNetworkManager.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		using bc_network_v3_extrapolation = bc_network_extrapolation<core::bc_vector3f>;

		class BC_GAME_DLL bc_network_component : public bci_actor_component
		{
			BC_COMPONENT(ntwr, true, false)
			friend class bci_network_manager;
			
		public:
			bc_network_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept;

			bc_network_component(bc_network_component&&) noexcept;

			~bc_network_component() override;

			bc_network_component& operator=(bc_network_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			bc_network_packet_time get_ping() const noexcept;

			void set_ping(bc_network_packet_time p_ping) noexcept;
			
			bc_actor_network_id get_network_id() const noexcept;

			void set_network_id(bc_actor_network_id p_id) noexcept;

			bc_actor_network_data_dir get_network_data_dir() const noexcept;

			const bcCHAR* get_network_entity_name() const noexcept;
			
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
			 * \brief Get property rate of change based on ping time scaled with elapsed clock
			 * \param p_name 
			 * \param p_clock 
			 * \return 
			 */
			std::pair<bool, core::bc_vector3f> get_extrapolated_value(const bcCHAR* p_name, const core_platform::bc_clock::update_param& p_clock) const noexcept;

		private:			
			bc_network_packet_time m_ping;
			bc_actor_network_id m_id;
			bc_actor_network_data_dir m_data_dir;
			const bcCHAR* m_network_entity_name;
			core::bc_vector<std::pair<const bcCHAR*, bc_network_v3_extrapolation>> m_extrapolators;
		};

		inline bc_network_packet_time bc_network_component::get_ping() const noexcept
		{
			return m_ping;
		}

		inline void bc_network_component::set_ping(bc_network_packet_time p_ping) noexcept
		{
			m_ping = p_ping;
		}
		
		inline bc_actor_network_id bc_network_component::get_network_id() const noexcept
		{
			return m_id;
		}

		inline void bc_network_component::set_network_id(bc_actor_network_id p_id) noexcept
		{
			m_id = p_id;
		}

		inline bc_actor_network_data_dir bc_network_component::get_network_data_dir() const noexcept
		{
			return m_data_dir;
		}

		inline const bcCHAR* bc_network_component::get_network_entity_name() const noexcept
		{
			return m_network_entity_name;
		}
	}
}