// [22/12/2018 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Scene/ActorComponent/bcBaseActorComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentEventAdapter.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/bcConstant.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_scene;

	class BC_GAME_DLL bc_mediate_component
		: public bc_base_actor_component<bc_mediate_component>,
		  public bc_component_event_adapter
		  <
			  bc_world_transform_actor_event,
			  bc_bound_box_changed_actor_event,
			  bc_added_to_scene_actor_event,
			  bc_removed_from_scene_actor_event
		  >
	{
		BC_COMPONENT(mediate, true, false)
		friend class bc_entity_manager;

	public:
		bc_mediate_component(bc_actor_id p_actor_id, bc_actor_component_id p_id);

		bc_mediate_component(bc_mediate_component&&) noexcept;

		~bc_mediate_component() override;

		bc_mediate_component& operator=(bc_mediate_component&&) noexcept;

		const bcCHAR* get_entity_name() const noexcept;

		bc_scene* get_scene() const noexcept;

		const physics::bc_bound_box& get_prev_bound_box() const noexcept;

		const physics::bc_bound_box& get_bound_box() const noexcept;

		const core::bc_matrix4f& get_world_transform() const noexcept;

		core::bc_vector3f get_position() const noexcept;

		void initialize(const bc_actor_component_initialize_context& p_context) override;
		
		void debug_draw(const bc_actor_component_debug_draw_context& p_context) override;

	private:
		void on_event(const bc_component_event_context<bc_world_transform_actor_event>& p_context) override;

		void on_event(const bc_component_event_context<bc_bound_box_changed_actor_event>& p_context) override;

		void on_event(const bc_component_event_context<bc_added_to_scene_actor_event>& p_context) override;

		void on_event(const bc_component_event_context<bc_removed_from_scene_actor_event>& p_context) override;

		void _set_entity_name(const bcCHAR* p_entity_name) noexcept;

		const bcCHAR* m_entity_name;
		bool m_added_to_scene;
		bc_scene* m_scene;

		platform::bc_clock::small_time m_last_bound_box_change_time;
		physics::bc_bound_box m_prev_bound_box;
		physics::bc_bound_box m_bound_box;
		core::bc_matrix4f m_world_transform;
	};

	inline bc_mediate_component::bc_mediate_component(bc_mediate_component&&) noexcept = default;

	inline bc_mediate_component::~bc_mediate_component() = default;

	inline bc_mediate_component& bc_mediate_component::operator=(bc_mediate_component&&) noexcept = default;

	inline const bcCHAR* bc_mediate_component::get_entity_name() const noexcept
	{
		return m_entity_name;
	}

	inline bc_scene* bc_mediate_component::get_scene() const noexcept
	{
		return m_scene;
	}

	inline const physics::bc_bound_box& bc_mediate_component::get_prev_bound_box() const noexcept
	{
		return m_prev_bound_box;
	}

	inline const physics::bc_bound_box& bc_mediate_component::get_bound_box() const noexcept
	{
		return m_bound_box;
	}

	inline const core::bc_matrix4f& bc_mediate_component::get_world_transform() const noexcept
	{
		return m_world_transform;
	}

	inline core::bc_vector3f bc_mediate_component::get_position() const noexcept
	{
		return m_world_transform.get_translation();
	}

	inline void bc_mediate_component::_set_entity_name(const bcCHAR* p_entity_name) noexcept
	{
		m_entity_name = p_entity_name;
	}
}
