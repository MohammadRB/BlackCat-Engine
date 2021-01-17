// [12/22/2018 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Memory/bcPtr.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;
		
		/**
		 * \brief Register mediate component as last component to receive published events as last component
		 */
		class BC_GAME_DLL bc_mediate_component : public bci_actor_component
		{
			BC_COMPONENT(mediate, true, true)

		public:
			static constexpr const bcCHAR* s_position_json_key = "position";
			static constexpr const bcCHAR* s_entity_name_json_key = "entity_name";

		public:
			explicit bc_mediate_component(bc_actor_component_index p_index);

			bc_mediate_component(bc_mediate_component&&) = default;

			~bc_mediate_component() = default;
			
			bc_mediate_component& operator=(bc_mediate_component&&) = default;

			bc_actor get_actor() const noexcept override;

			const bcCHAR* get_entity_name() const;

			void set_entity_name(const bcCHAR* p_entity_name);

			void set_controller(core::bc_unique_ptr< bci_actor_controller > p_controller);
			
			const physics::bc_bound_box& get_prev_bound_box() const noexcept;
			
			const physics::bc_bound_box& get_bound_box() const noexcept;

			const core::bc_matrix4f& get_transform() const noexcept;
			
			core::bc_vector3f get_position() const noexcept;

			void initialize(bc_actor_component_initialize_context& p_context) override;
			
			void update(bc_actor_component_update_content& p_context) override;
			
			void handle_event(bc_actor_component_event_context& p_context) override;

			void debug_draw(bc_actor_component_debug_draw_context& p_context) override;
			
		private:
			void _handle_event(bc_actor& p_actor, const bc_actor_event& p_event);

			const bcCHAR* m_entity_name;
			
			bc_scene* m_scene;
			core::bc_matrix4f m_transform;
			bool m_bound_box_changed;
			physics::bc_bound_box m_prev_bound_box;
			physics::bc_bound_box m_bound_box;
			core::bc_unique_ptr<bci_actor_controller> m_controller;
		};

		inline const bcCHAR* bc_mediate_component::get_entity_name() const
		{
			return m_entity_name;
		}
		
		inline void bc_mediate_component::set_entity_name(const bcCHAR* p_entity_name)
		{
			m_entity_name = p_entity_name;
		}

		inline const physics::bc_bound_box& bc_mediate_component::get_prev_bound_box() const noexcept
		{
			return m_prev_bound_box;
		}
		
		inline const physics::bc_bound_box& bc_mediate_component::get_bound_box() const noexcept
		{
			return m_bound_box;
		}

		inline const core::bc_matrix4f& bc_mediate_component::get_transform() const noexcept
		{
			return m_transform;
		}
		
		inline core::bc_vector3f bc_mediate_component::get_position() const noexcept
		{
			return m_transform.get_translation();
		}
	}
}