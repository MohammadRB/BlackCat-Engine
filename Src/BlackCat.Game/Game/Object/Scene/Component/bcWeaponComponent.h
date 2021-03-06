// [03/27/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Game/System/Render/Light/bcLight.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Bullet/bcBullet.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;
		
		enum class bc_weapon_class : bcUBYTE
		{
			rifle
		};
		
		class BC_GAME_DLL bc_weapon_component : public bci_actor_component
		{
			BC_COMPONENT(weapon, false, true)

		public:
			bc_weapon_component(bc_actor_index p_actor_index, bc_actor_component_index p_index);

			bc_weapon_component(bc_weapon_component&&) noexcept = default;

			~bc_weapon_component() override = default;

			bc_weapon_component& operator=(bc_weapon_component&&) noexcept = default;

			bc_actor get_actor() const noexcept override;
			
			bc_weapon_class get_class() const noexcept;
			
			const core::bc_vector3f& get_up_ls() const noexcept;

			const core::bc_vector3f& get_forward_ls() const noexcept;
			
			const core::bc_vector3f& get_main_hand_offset_ls() const noexcept;
			
			const core::bc_vector3f& get_second_hand_offset_ls() const noexcept;

			bcFLOAT get_rate_of_fire_seconds() const noexcept;

			bc_bullet shoot(const core::bc_vector3f& p_aim_direction_ws);
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void update(const bc_actor_component_update_content& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;
			
		private:
			bc_weapon_class m_class;
			core::bc_vector3f m_up_ls;
			core::bc_vector3f m_forward_ls;
			core::bc_vector3f m_main_hand_offset_ls;
			core::bc_vector3f m_second_hand_offset_ls;
			core::bc_vector3f m_fire_offset_ls;
			const bcCHAR* m_fire_particle;
			core::bc_vector3f m_fire_light_color;
			bcFLOAT m_fire_light_radius;
			bcFLOAT m_fire_light_intensity;
			bcFLOAT m_rate_of_fire_seconds;
			bcFLOAT m_bullet_speed;
			bcFLOAT m_bullet_mass;

			bc_scene* m_scene;
			bc_light_ptr m_light;
			bcFLOAT m_light_age;
		};

		inline bc_weapon_class bc_weapon_component::get_class() const noexcept
		{
			return m_class;
		}

		inline const core::bc_vector3f& bc_weapon_component::get_up_ls() const noexcept
		{
			return m_up_ls;
		}

		inline const core::bc_vector3f& bc_weapon_component::get_forward_ls() const noexcept
		{
			return m_forward_ls;
		}
		
		inline const core::bc_vector3f& bc_weapon_component::get_main_hand_offset_ls() const noexcept
		{
			return m_main_hand_offset_ls;
		}

		inline const core::bc_vector3f& bc_weapon_component::get_second_hand_offset_ls() const noexcept
		{
			return m_second_hand_offset_ls;
		}

		inline bcFLOAT bc_weapon_component::get_rate_of_fire_seconds() const noexcept
		{
			return m_rate_of_fire_seconds;
		}
	}
}