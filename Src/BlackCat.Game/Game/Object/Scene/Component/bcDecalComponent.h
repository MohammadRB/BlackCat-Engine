// [02/22/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Container/bcIteratorAdapter.h"
#include "Core/Container/bcVector.h"
#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_decal_manager;
		
		class BC_GAME_DLL bc_decal_component : public bc_render_component, public core::bc_const_iterator_adapter<core::bc_vector<bc_decal_instance_ptr>>
		{
			BC_COMPONENT(dcl, true, false)
			
		public:
			bc_decal_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept;

			bc_decal_component(bc_decal_component&&) noexcept;

			~bc_decal_component() override;

			bc_decal_component& operator=(bc_decal_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			void add_decal(const bcCHAR* p_decal_name,
				const core::bc_vector3f& p_local_pos,
				const core::bc_matrix3f& p_local_rotation,
				const core::bc_matrix4f& p_initial_world_transform);

			void add_decal(const bcCHAR* p_decal_name,
				const core::bc_vector3f& p_local_pos,
				const core::bc_matrix3f& p_local_rotation,
				bc_render_group p_render_group,
				const core::bc_matrix4f& p_initial_world_transform);

			void add_decal(const bcCHAR* p_decal_name,
				const core::bc_vector3f& p_local_pos,
				const core::bc_matrix3f& p_local_rotation,
				const core::bc_matrix4f& p_initial_world_transform,
				bc_mesh_node::node_index_t p_attached_node);
			
			void add_decal(const bcCHAR* p_decal_name,
				const core::bc_vector3f& p_local_pos,
				const core::bc_matrix3f& p_local_rotation,
				bc_render_group p_render_group,
				const core::bc_matrix4f& p_initial_world_transform,
				bc_mesh_node::node_index_t p_attached_node);

			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;

			void render(const bc_actor_component_render_context& p_context) const override;

		private:
			void _add_decal(bc_decal_instance_ptr p_decal) noexcept;

			const bcUINT32 m_decals_count = 10;
			
			bc_decal_manager* m_decal_manager;
			core_platform::bc_atomic<bcUINT32> m_decals_slot;
			container_type m_decals;
			bcFLOAT m_mesh_scale;
			bool m_use_hierarchy_transforms;
		};
	}
}