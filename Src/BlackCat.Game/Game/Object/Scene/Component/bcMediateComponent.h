// [12/22/2018 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_mediate_component : public bc_iactor_component
		{
			BC_COMPONENT(mediate)

		public:
			explicit bc_mediate_component(bc_actor_component_index p_index);

			bc_mediate_component(bc_mediate_component&&) = default;

			~bc_mediate_component() = default;

			bc_mediate_component& operator=(bc_mediate_component&&) = default;

			bc_actor get_actor() const noexcept override;

			const physics::bc_bound_box& get_bound_box() const;

			void set_bound_box(const physics::bc_bound_box& p_bound_box);

			core::bc_vector3f get_world_position() const;

			void set_world_position(const core::bc_vector3f& p_position);

			void set_world_transform(const core::bc_matrix4f& p_transform);

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param) override;

		protected:

		private:
			physics::bc_bound_box m_bound_box;
		};

		inline const physics::bc_bound_box& bc_mediate_component::get_bound_box() const
		{
			return m_bound_box;
		}

		inline void bc_mediate_component::set_bound_box(const physics::bc_bound_box& p_bound_box)
		{
			m_bound_box = p_bound_box;
		}
	}
}