// [03/27/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		enum class bc_weapon_class : bcUBYTE
		{
			rifle
		};
		
		class BC_GAME_DLL bc_weapon_component : public bci_actor_component
		{
			BC_COMPONENT(weapon, false, false)

		public:
			bc_weapon_component(bc_actor_index p_actor_index, bc_actor_component_index p_index);

			bc_weapon_component(bc_weapon_component&&) noexcept = default;

			~bc_weapon_component() override = default;

			bc_weapon_component& operator=(bc_weapon_component&&) noexcept = default;

			bc_actor get_actor() const noexcept override;
			
			bc_weapon_class get_class() const noexcept;
			
			const bcCHAR* get_main_hand_node_name() const noexcept;
			
			const bcCHAR* get_second_hand_node_name() const noexcept;

			const core::bc_vector3f& get_main_hand_local_offset() const noexcept;
			
			const core::bc_vector3f& get_second_hand_local_offset() const noexcept;

			const core::bc_vector3f& get_local_up() const noexcept;
			
			const core::bc_vector3f& get_local_forward() const noexcept;

			bcFLOAT get_rate_of_fire_seconds() const noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;
		
		private:
			bc_weapon_class m_class;
			const bcCHAR* m_main_hand_node_name;
			const bcCHAR* m_second_hand_node_name;
			core::bc_vector3f m_main_hand_local_offset;
			core::bc_vector3f m_second_hand_local_offset;
			core::bc_vector3f m_local_up;
			core::bc_vector3f m_local_forward;
			bcFLOAT m_rate_of_fire_seconds;
		};

		inline bc_weapon_class bc_weapon_component::get_class() const noexcept
		{
			return m_class;
		}
		
		inline const bcCHAR* bc_weapon_component::get_main_hand_node_name() const noexcept
		{
			return m_main_hand_node_name;
		}

		inline const bcCHAR* bc_weapon_component::get_second_hand_node_name() const noexcept
		{
			return m_second_hand_node_name;
		}

		inline const core::bc_vector3f& bc_weapon_component::get_main_hand_local_offset() const noexcept
		{
			return m_main_hand_local_offset;
		}

		inline const core::bc_vector3f& bc_weapon_component::get_second_hand_local_offset() const noexcept
		{
			return m_second_hand_local_offset;
		}

		inline const core::bc_vector3f& bc_weapon_component::get_local_up() const noexcept
		{
			return m_local_up;
		}
		
		inline const core::bc_vector3f& bc_weapon_component::get_local_forward() const noexcept
		{
			return m_local_forward;
		}

		inline bcFLOAT bc_weapon_component::get_rate_of_fire_seconds() const noexcept
		{
			return m_rate_of_fire_seconds;
		}
	}
}