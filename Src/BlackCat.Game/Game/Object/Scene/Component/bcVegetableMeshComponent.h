// [09/06/2019 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_vegetable_mesh_component : public bc_mesh_component
		{
			BC_COMPONENT(vegetable_mesh)

		public:
			explicit bc_vegetable_mesh_component(bc_actor_component_index p_index);

			bc_vegetable_mesh_component(bc_vegetable_mesh_component&& p_other) noexcept;

			~bc_vegetable_mesh_component();

			bc_vegetable_mesh_component& operator=(bc_vegetable_mesh_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void render(bc_render_system& p_render_system) const override;

			void render(bc_render_system& p_render_system, bool p_render_leaf) const;

			void handle_event(const bc_actor& p_actor, const bc_actor_event& p_event) override;

			void update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param) override;
			
		private:
		};
	}
}