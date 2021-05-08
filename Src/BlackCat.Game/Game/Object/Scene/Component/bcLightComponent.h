// [03/01/2019 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/System/Render/Light/bcLight.h"

namespace black_cat
{
	namespace game
	{
		struct _bc_light_component_light_holder
		{
			_bc_light_component_light_holder(const bc_direct_light& p_light)
				: m_type(bc_light_type::direct),
				m_direct_light(p_light)
			{
			}

			_bc_light_component_light_holder(const bc_point_light& p_light)
				: m_type(bc_light_type::point),
				m_point_light(p_light)
			{
			}

			_bc_light_component_light_holder(const bc_spot_light& p_light)
				: m_type(bc_light_type::spot),
				m_spot_light(p_light)
			{
			}
			
			~_bc_light_component_light_holder()
			{
				switch (m_type)
				{
				case bc_light_type::direct:
					m_direct_light.~bc_direct_light();
					break;
				case bc_light_type::point:
					m_point_light.~bc_point_light();
					break;
				case bc_light_type::spot:
					m_spot_light.~bc_spot_light();
					break;
				default:
					BC_ASSERT(false);
				}
			}

			bc_light_type m_type;
			union
			{
				bc_direct_light m_direct_light;
				bc_point_light m_point_light;
				bc_spot_light m_spot_light;
			};
		};
		
		class BC_GAME_DLL bc_light_component : public bci_actor_component
		{
			BC_COMPONENT(light, true, false)

		public:
			bc_light_component(bc_actor_index p_actor_index, bc_actor_component_index p_index);
			
			bc_light_component(bc_light_component&& p_other) noexcept = default;

			~bc_light_component() override;

			bc_light_component& operator=(bc_light_component&& p_other) noexcept = default;

			bc_actor get_actor() const noexcept override;

			core::bc_vector3f get_world_position() const noexcept;

			const core::bc_matrix4f& get_world_transform() const noexcept;

			bc_light* get_light() noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;
			
		private:
			bc_light_ptr m_light;
			_bc_light_component_light_holder* m_light_holder;
		};
	}
}