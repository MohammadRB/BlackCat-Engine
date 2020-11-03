// [11/03/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcExport.h"
#include "Game/System/Render/Light/bcLight.h"

namespace black_cat
{
	namespace game
	{
		/**
		 * \brief Copied light information with transformations applied
		 */
		class BC_GAME_DLL bc_light_instance
		{
		public:
			explicit bc_light_instance(const bc_light& p_light) noexcept;

			bc_light_instance(const bc_light_instance& p_other) noexcept;

			~bc_light_instance();

			bc_light_instance& operator=(const bc_light_instance& p_other) noexcept;
			
			bc_light_type get_type() const noexcept
			{
				return m_type;
			}

			const physics::bc_bound_box& get_bound_box() const noexcept
			{
				return m_bound_box;
			}

			core::bc_vector3f get_min_bound() const noexcept;

			core::bc_vector3f get_max_bound() const noexcept;

			bc_direct_light* as_direct_light() noexcept;

			const bc_direct_light* as_direct_light() const noexcept;

			bc_point_light* as_point_light() noexcept;

			const bc_point_light* as_point_light() const noexcept;

			bc_spot_light* as_spot_light() noexcept;

			const bc_spot_light* as_spot_light() const noexcept;
			
		private:
			union
			{
				bc_direct_light m_direct_light;
				bc_point_light m_point_light;
				bc_spot_light m_spot_light;
			};
			bc_light_type m_type;
			physics::bc_bound_box m_bound_box;
		};
	}
}
