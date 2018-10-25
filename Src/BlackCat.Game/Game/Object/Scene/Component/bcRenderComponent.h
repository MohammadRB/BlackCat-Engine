// [04/28/2016 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scene/bcActor.h"
#include "Game/Object/Scene/bcActorComponent.h"
#include "Game/System/Render/bcRenderState.h"
#include "Game/System/Render/bcRenderInstance.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_system;

		class BC_GAME_DLL bc_render_component : public bc_iactor_component
		{
			BC_COMPONENT(render)

		public:
			explicit bc_render_component(bc_actor_component_index p_index);

			bc_render_component(bc_render_component&&) noexcept;

			~bc_render_component();

			bc_render_component& operator=(bc_render_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param) override;

			void render(const bc_render_state* p_render_state, const bc_render_instance& p_render_instance) const;

		protected:

		private:
			bc_render_system* m_render_system;
		};
	}
}
