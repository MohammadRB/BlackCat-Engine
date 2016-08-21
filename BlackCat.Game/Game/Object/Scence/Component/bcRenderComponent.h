// [04/28/2016 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scence/bcActor.h"
#include "Game/Object/Scence/bcActorComponent.h"
#include "Game/Object/Scence/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_system;

		class BC_GAME_DLL bc_render_component : public bc_iactor_component
		{
		public:
			explicit bc_render_component(bc_actor_component_index p_index);

			bc_render_component(bc_render_component&&) noexcept;

			~bc_render_component();

			bc_render_component& operator=(bc_render_component&&) noexcept;

			void initialize(const core::bc_data_driven_parameter& p_parameters) override;

			void update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param) override;

			void render(bc_render_system& p_render_system);

			static constexpr const bcCHAR* component_name()
			{
				return core::g_cmp_render;
			}

		protected:

		private:
			const bc_mesh_component* m_mesh_component;
		};
	}
}
