// [01/15/2020 MRB]

#pragma once

#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_actor_event_added_to_scene : public bc_actor_event
		{
			BC_EVENT(a_a_s)
			
		public:
			explicit bc_actor_event_added_to_scene(bc_scene& p_scene);

			bc_actor_event_added_to_scene(const bc_actor_event_added_to_scene&);

			~bc_actor_event_added_to_scene();

			bc_actor_event_added_to_scene& operator=(const bc_actor_event_added_to_scene&);

			bc_scene& get_scene() const noexcept;

		private:
			bc_scene* m_scene;
		};

		inline bc_actor_event_added_to_scene::bc_actor_event_added_to_scene(bc_scene& p_scene)
			: bc_actor_event(event_name()),
			m_scene(&p_scene)
		{
		}

		inline bc_actor_event_added_to_scene::bc_actor_event_added_to_scene(const bc_actor_event_added_to_scene&) = default;

		inline bc_actor_event_added_to_scene::~bc_actor_event_added_to_scene() = default;

		inline bc_actor_event_added_to_scene& bc_actor_event_added_to_scene::operator=(const bc_actor_event_added_to_scene&) = default;

		inline bc_scene& bc_actor_event_added_to_scene::get_scene() const noexcept
		{
			return *m_scene;
		}
	}
}