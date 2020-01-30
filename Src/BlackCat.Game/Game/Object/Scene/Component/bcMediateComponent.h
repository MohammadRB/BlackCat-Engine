// [12/22/2018 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;
		
		/**
		 * \brief Register mediate component as last component to receive published events as last component
		 */
		class BC_GAME_DLL bc_mediate_component : public bc_iactor_component
		{
			BC_COMPONENT(mediate)

		public:
			static constexpr const bcCHAR* s_position_json_key = "position";
			static constexpr const bcCHAR* s_entity_name_json_key = "entity_name";

		public:
			explicit bc_mediate_component(bc_actor_component_index p_index);

			bc_mediate_component(bc_mediate_component&&) = default;

			~bc_mediate_component() = default;
			
			bc_mediate_component& operator=(bc_mediate_component&&) = default;

			bc_actor get_actor() const noexcept override;

			void set_entity_name(const bcCHAR* p_entity_name);

			const bcCHAR* get_entity_name() const;

			const physics::bc_bound_box& get_prev_bound_box() const noexcept;
			
			const physics::bc_bound_box& get_bound_box() const noexcept;

			core::bc_vector3f get_world_position() const noexcept;

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void write_instance(bc_actor& p_actor, core::bc_json_key_value& p_parameters) override;

			void handle_event(bc_actor& p_actor, const bc_actor_event& p_event) override;

			void update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param) override;
			
		private:
			const bcCHAR* m_entity_name;
			bc_scene* m_scene;
			bool m_bound_box_changed;
			physics::bc_bound_box m_bound_box;
			physics::bc_bound_box m_prev_bound_box;
		};

		inline void bc_mediate_component::set_entity_name(const bcCHAR* p_entity_name)
		{
			m_entity_name = p_entity_name;
		}

		inline const bcCHAR* bc_mediate_component::get_entity_name() const
		{
			return m_entity_name;
		}

		inline const physics::bc_bound_box& bc_mediate_component::get_prev_bound_box() const noexcept
		{
			return m_prev_bound_box;
		}
		
		inline const physics::bc_bound_box& bc_mediate_component::get_bound_box() const noexcept
		{
			return m_bound_box;
		}

		inline core::bc_vector3f bc_mediate_component::get_world_position() const noexcept
		{
			return m_bound_box.get_center();
		}
	}
}