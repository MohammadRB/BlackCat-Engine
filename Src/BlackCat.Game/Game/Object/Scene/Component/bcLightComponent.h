// [03/01/2019 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scene/bcActor.h"
#include "Game/Object/Scene/bcActorComponent.h"
#include "Game/System/Render/Light/bcLight.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_light_component : public bc_iactor_component
		{
			BC_COMPONENT(light)

		public:
			explicit bc_light_component(bc_actor_component_index p_index);
			
			bc_light_component(bc_light_component&& p_other) noexcept = default;

			~bc_light_component() = default;

			bc_light_component& operator=(bc_light_component&& p_other) noexcept = default;

			bc_actor get_actor() const noexcept override;

			core::bc_vector3f get_world_position() const noexcept;

			const core::bc_matrix4f& get_world_transform() const noexcept;

			void set_world_transform(const core::bc_matrix4f& p_transform) noexcept;

			const bc_light* get_light() const noexcept;

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param) override;

		protected:

		private:
			bc_light_ptr m_light;
		};
	}
}