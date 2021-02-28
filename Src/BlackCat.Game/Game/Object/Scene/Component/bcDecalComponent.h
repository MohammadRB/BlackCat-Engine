// [02/22/2021 MRB]

#pragma once

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
		class BC_GAME_DLL bc_decal_component : public bc_render_component, public core::bc_const_iterator_adapter<core::bc_vector<bc_decal_instance_ptr>>
		{
			BC_COMPONENT(hgt_map, true, false)
			
		public:
			bc_decal_component(bc_actor_index p_actor_index, bc_actor_component_index p_index);

			bc_decal_component(bc_decal_component&&) noexcept;

			~bc_decal_component() override;

			bc_decal_component& operator=(bc_decal_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			void add_decal(const bcCHAR* p_decal_name,
				const core::bc_vector3f& p_local_pos,
				const core::bc_matrix4f& p_initial_world_transform,
				bc_mesh_node::node_index_t p_attached_node);

			void handle_event(const bc_actor_component_event_context& p_context) override;

			void render(const bc_actor_component_render_context& p_context) const override;
			
		private:
			container_type m_decals;
		};
	}
}