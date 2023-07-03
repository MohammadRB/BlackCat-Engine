// [25/02/2021 MRB]

#pragma once

#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_remove_decal_actor_event : public bc_actor_event
		{
			BC_EVENT(a_r_d)

		public:
			explicit bc_remove_decal_actor_event(bc_decal_instance& p_instance);

			bc_remove_decal_actor_event(const bc_remove_decal_actor_event&);

			~bc_remove_decal_actor_event();

			bc_remove_decal_actor_event& operator=(const bc_remove_decal_actor_event&);

			bc_decal_instance& get_decal_instance() const noexcept;

		private:
			bc_decal_instance* m_instance;
		};

		inline bc_remove_decal_actor_event::bc_remove_decal_actor_event(bc_decal_instance& p_instance)
			: bc_actor_event(message_name()),
			m_instance(&p_instance)
		{
		}

		inline bc_remove_decal_actor_event::bc_remove_decal_actor_event(const bc_remove_decal_actor_event&) = default;

		inline bc_remove_decal_actor_event::~bc_remove_decal_actor_event() = default;

		inline bc_remove_decal_actor_event& bc_remove_decal_actor_event::operator=(const bc_remove_decal_actor_event&) = default;

		inline bc_decal_instance& bc_remove_decal_actor_event::get_decal_instance() const noexcept
		{
			return *m_instance;
		}
	}
}