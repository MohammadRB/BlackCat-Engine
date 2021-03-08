// [09/14/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcDecalResolverComponent.h"
#include "Game/Object/Mesh/bcHeightMap.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_height_map_component : public bc_render_component, public bc_decal_resolver_component
		{
			BC_COMPONENT(hgt_map, true, false)

		public:
			bc_height_map_component(bc_actor_index p_actor_index, bc_actor_component_index p_index);

			bc_height_map_component(bc_height_map_component&&) noexcept;

			~bc_height_map_component() override;

			bc_height_map_component& operator=(bc_height_map_component&&) noexcept;

			core::bc_vector3f get_world_position() const
			{
				return m_transform.get_translation();
			}

			const bc_height_map& get_height_map() const
			{
				return *m_height_map;
			}

			bc_height_map_ptr get_height_map_ptr() const
			{
				return m_height_map;
			}

			bc_actor get_actor() const noexcept override;

			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void write_instance(const bc_actor_component_write_context& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;
			
			void render(const bc_actor_component_render_context& p_context) const override;

			void add_decal(const bcCHAR* p_decal_name, const core::bc_vector3f& p_world_position, const core::bc_vector3f& p_dir) override;
			
		private:
			bc_height_map_ptr m_height_map;
			core::bc_matrix4f m_transform;
		};
	}
}