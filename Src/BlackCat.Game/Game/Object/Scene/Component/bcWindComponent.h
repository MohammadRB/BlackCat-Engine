// [12/12/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcNullable.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "PhysicsImp/Shape/bcBoundBox.h"

namespace black_cat
{
	namespace game
	{
		enum class bc_wind_type
		{
			direct, point
		};

		struct bc_direct_wind
		{
		public:
			bc_direct_wind(const core::bc_vector3f& p_direction, bcFLOAT p_power)
				: m_direction(p_direction),
				m_power(p_power)
			{
			}
			
			core::bc_vector3f m_direction;
			bcFLOAT m_power;
		};

		struct bc_point_wind
		{
		public:
			bc_point_wind(const core::bc_vector3f& p_position, bcFLOAT p_radios, bcFLOAT p_power)
				: m_position(p_position),
				m_radios(p_radios),
				m_power(p_power)
			{
			}
			
			core::bc_vector3f m_position;
			bcFLOAT m_radios;
			bcFLOAT m_power;
		};

		class BC_GAME_DLL bc_wind
		{
		public:
			explicit bc_wind(const bc_direct_wind& p_direct);

			explicit bc_wind(const bc_point_wind& p_point);

			bc_wind(const bc_wind& p_other) noexcept;

			~bc_wind();

			bc_wind& operator=(const bc_wind& p_other) noexcept;

			void set_transformation(const core::bc_matrix4f& p_transformation) noexcept;
			
			const physics::bc_bound_box& get_bound_box() const noexcept;
			
			bc_direct_wind* as_direct_wind() noexcept;
			
			const bc_direct_wind* as_direct_wind() const noexcept;
			
			bc_point_wind* as_point_wind() noexcept;

			const bc_point_wind* as_point_wind() const noexcept;

		private:
			void _calculate_bound_box() noexcept;
			
			union
			{
				bc_direct_wind m_direct;
				bc_point_wind m_point;
			};
			bc_wind_type m_type;
			physics::bc_bound_box m_bound_box;
		};
		
		class BC_GAME_DLL bc_wind_component : public bc_iactor_component
		{
			BC_COMPONENT(wind, true, false)

		public:
			explicit bc_wind_component(bc_actor_component_index p_index);

			bc_wind_component(bc_wind_component&& p_other) noexcept = default;

			~bc_wind_component() = default;

			bc_wind_component& operator=(bc_wind_component&& p_other) noexcept = default;

			bc_actor get_actor() const noexcept override;

			const bc_wind* get_wind() const noexcept;

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void handle_event(bc_actor& p_actor, const bc_actor_event& p_event) override;

			void update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param) override;

		private:			
			core::bc_nullable<bc_wind> m_wind;
		};
	}
}