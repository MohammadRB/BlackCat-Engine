// [15/01/2020 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_scene;

	class BC_GAME_DLL bc_added_to_scene_actor_event : public bc_actor_event
	{
		BC_EVENT(a_a_s)
			
	public:
		explicit bc_added_to_scene_actor_event(bc_scene& p_scene);

		bc_added_to_scene_actor_event(const bc_added_to_scene_actor_event&);

		~bc_added_to_scene_actor_event() override;

		bc_added_to_scene_actor_event& operator=(const bc_added_to_scene_actor_event&);

		bc_scene& get_scene() const noexcept;

	private:
		bc_scene* m_scene;
	};

	inline bc_added_to_scene_actor_event::bc_added_to_scene_actor_event(bc_scene& p_scene)
		: bc_actor_event(message_name()),
		  m_scene(&p_scene)
	{
	}

	inline bc_added_to_scene_actor_event::bc_added_to_scene_actor_event(const bc_added_to_scene_actor_event&) = default;

	inline bc_added_to_scene_actor_event::~bc_added_to_scene_actor_event() = default;

	inline bc_added_to_scene_actor_event& bc_added_to_scene_actor_event::operator=(const bc_added_to_scene_actor_event&) = default;

	inline bc_scene& bc_added_to_scene_actor_event::get_scene() const noexcept
	{
		return *m_scene;
	}
}
