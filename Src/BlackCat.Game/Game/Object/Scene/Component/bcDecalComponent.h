// [22/02/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Container/bcIteratorAdapter.h"
#include "Core/Container/bcArray.h"
#include "Core/Container/bcVector.h"
#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_decal_manager;
		
	class BC_GAME_DLL bc_decal_component : public bc_render_component, public core::bc_const_iterator_adapter<core::bc_vector<bc_decal_instance_ptr>>
	{
		BC_COMPONENT(dcl, true, false)
		friend class platform::bc_lock_guard<bc_decal_component>;

	public:
		bc_decal_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept;

		bc_decal_component(bc_decal_component&&) noexcept;

		~bc_decal_component() override;

		bc_decal_component& operator=(bc_decal_component&&) noexcept;

		bc_actor get_actor() const noexcept override;

		void add_decal(core::bc_string_view p_name,
		               const core::bc_vector3f& p_local_pos,
		               const core::bc_matrix3f& p_local_rotation,
		               const core::bc_matrix4f& p_initial_world_transform);

		void add_decal(core::bc_string_view p_name,
		               const core::bc_vector3f& p_local_pos,
		               const core::bc_matrix3f& p_local_rotation,
		               bc_actor_render_group p_render_group,
		               const core::bc_matrix4f& p_initial_world_transform);

		void add_decal(core::bc_string_view p_name,
		               const core::bc_vector3f& p_local_pos,
		               const core::bc_matrix3f& p_local_rotation,
		               const core::bc_matrix4f& p_initial_world_transform,
		               bc_mesh_node::node_index_t p_attached_node);
			
		void add_decal(core::bc_string_view p_name,
		               const core::bc_vector3f& p_local_pos,
		               const core::bc_matrix3f& p_local_rotation,
		               bc_actor_render_group p_render_group,
		               const core::bc_matrix4f& p_initial_world_transform,
		               bc_mesh_node::node_index_t p_attached_node);

		void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;

		void load_instance(const bc_actor_component_load_context& p_context) override;
			
		void write_instance(const bc_actor_component_write_context& p_context) override;
			
		void handle_event(const bc_actor_component_event_context& p_context) override;

		void render(const bc_actor_component_render_context& p_context) const override;

	private:
		void _add_decal_if_needed(bc_decal_instance_ptr p_decal, bool p_need_update) noexcept;

		void lock();

		void unlock();

		bc_decal_manager_container* m_decal_manager;
		bcFLOAT m_mesh_scale;
		bool m_use_hierarchy_transforms;

		platform::bc_atomic_flag m_decals_lock;
		bcUINT32 m_temporary_decals_slot;
		core::bc_array<bc_decal_instance_ptr, 10> m_temporary_decals;
		container_type m_persistent_decals;
	};
}
