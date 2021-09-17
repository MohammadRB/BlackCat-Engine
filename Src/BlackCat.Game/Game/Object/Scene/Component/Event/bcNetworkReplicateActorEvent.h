// [09/17/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		/**
		 * \brief Is risen when actor is replicated through network.
		 */
		class BC_GAME_DLL bc_network_replicate_actor_event : public bc_actor_event
		{
			BC_EVENT(net_rep)
			
		public:
			explicit bc_network_replicate_actor_event(bc_actor_network_data_dir p_data_dir);

			bc_network_replicate_actor_event(const bc_network_replicate_actor_event&) noexcept;

			~bc_network_replicate_actor_event() override;

			bc_network_replicate_actor_event& operator=(const bc_network_replicate_actor_event&) noexcept;

			bc_actor_network_data_dir get_data_dir() const noexcept;
			
		private:
			bc_actor_network_data_dir m_data_dir;
		};

		inline bc_network_replicate_actor_event::bc_network_replicate_actor_event(bc_actor_network_data_dir p_data_dir)
			: bc_actor_event(message_name()),
			m_data_dir(p_data_dir)
		{
		}

		inline bc_network_replicate_actor_event::bc_network_replicate_actor_event(const bc_network_replicate_actor_event&) noexcept = default;

		inline bc_network_replicate_actor_event::~bc_network_replicate_actor_event() = default;

		inline bc_network_replicate_actor_event& bc_network_replicate_actor_event::operator=(const bc_network_replicate_actor_event&) noexcept = default;

		inline bc_actor_network_data_dir bc_network_replicate_actor_event::get_data_dir() const noexcept
		{
			return m_data_dir;
		}
	}	
}