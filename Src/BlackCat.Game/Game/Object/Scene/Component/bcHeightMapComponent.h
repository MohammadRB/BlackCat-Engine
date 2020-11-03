// [09/14/2016 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Mesh/bcHeightMap.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_height_map_component : public bc_render_component
		{
			BC_COMPONENT(hgt_map, true, false)

		public:
			explicit bc_height_map_component(bc_actor_component_index p_index);

			bc_height_map_component(bc_height_map_component&&) noexcept;

			~bc_height_map_component();

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

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void write_instance(bc_actor& p_actor, core::bc_json_key_value& p_parameters) override;

			void handle_event(bc_actor& p_actor, const bc_actor_event& p_event) override;
			
			void render(bc_render_state_buffer& p_buffer) const override;
			
		private:
			bc_height_map_ptr m_height_map;
			core::bc_matrix4f m_transform;
		};
	}
}