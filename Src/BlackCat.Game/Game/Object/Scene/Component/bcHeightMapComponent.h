// [09/14/2016 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scene/bcActor.h"
#include "Game/Object/Scene/bcActorComponent.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_height_map_component : public bc_iactor_component
		{
			BC_COMPONENT(height_map)

		public:
			explicit bc_height_map_component(bc_actor_component_index p_index);

			bc_height_map_component(bc_height_map_component&&) noexcept;

			~bc_height_map_component();

			bc_height_map_component& operator=(bc_height_map_component&&) noexcept;

			const bc_height_map* get_height_map() const
			{
				return m_height_map.get();
			}

			bc_actor get_actor() const noexcept override;

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param) override;

			void render(const bc_render_component& p_render_component) const;

		protected:

		private:
			bc_height_map_ptr m_height_map;
		};
	}
}
