// [12/21/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Utility/bcTemplateMetaType.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_physics_system : public core::bc_initializable<>
		{
		public:
			bc_physics_system();

			bc_physics_system(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move);

			~bc_physics_system();

			bc_physics_system& operator=(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move);;

			physics::bc_physics& get_physics() noexcept
			{
				return m_physics;
			}

			bcFLOAT get_height_field_y_scale() const noexcept
			{
				return s_height_field_y_scale_ratio;
			}

			void update(core_platform::bc_clock::update_param p_clock_update_param);

			physics::bc_mesh_buffer create_height_field(bcUINT32 p_num_row, bcUINT32 p_num_column, bcFLOAT* p_samples);

			physics::bc_height_field_ref create_height_field(physics::bc_mesh_buffer& p_buffer);

		protected:
			void _initialize() override;

			void _destroy() override;

		private:
			/**
			* \brief Convert physics system height map height value to render system height value
			* \param p_physics_height
			* \return
			*/
			static bcFLOAT height_to_float(bcINT16 p_physics_height)
			{
				return p_physics_height * s_height_field_y_scale_ratio;
			}

			/**
			* \brief Convert render system height map height value to physics system height value
			* \param p_height
			* \return
			*/
			static bcINT16 height_to_int16(bcFLOAT p_height)
			{
				return p_height / s_height_field_y_scale_ratio;
			}

			physics::bc_physics m_physics;

			static constexpr bcFLOAT s_height_field_y_scale_ratio = 0.1f;
		};
	}
}