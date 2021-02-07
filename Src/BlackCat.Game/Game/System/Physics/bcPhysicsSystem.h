// [12/21/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Utility/bcTemplateMetaType.h"
#include "Physics/bcUtility.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_mesh_component;
		class bc_height_map_component;
		class bc_actor;

		class BC_GAME_DLL bc_physics_system : public core::bc_initializable<>
		{
		public:
			bc_physics_system();
			
			bc_physics_system(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move);

			~bc_physics_system();
			
			bc_physics_system& operator=(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move);
			
			physics::bc_physics& get_physics() noexcept;

			bcFLOAT get_height_field_y_scale() const noexcept;

			core::bc_unique_ptr< bcINT16 > convert_height_field_samples(bcUINT32 p_num_row, bcUINT32 p_num_column, bcFLOAT* p_samples);

			/**
			 * \brief Store a pointer of game actor inside physics actor so it can be accessible within physics callbacks
			 * \param p_px_actor 
			 * \param p_actor 
			 */
			void set_game_actor(physics::bc_actor& p_px_actor, bc_actor p_actor) noexcept;

			/**
			 * \brief Retrieve stored pointer of game actor
			 * \param p_px_actor
			 * \return
			 */
			bc_actor get_game_actor(physics::bc_actor& p_px_actor) noexcept;

			void create_px_shapes_from_height_map(physics::bc_rigid_static& p_rigid_static, const bc_actor& p_actor, const bc_height_map_component& p_height_map);

			void create_px_shapes_from_mesh(physics::bc_rigid_body& p_px_actor, const bc_actor& p_actor, const bc_mesh_component& p_mesh);

			void update(core_platform::bc_clock::update_param p_clock_update_param);

		protected:
			void _initialize() override;

			void _destroy() override;

		private:
			/**
			* \brief Convert physics system height map height value to render system height value
			* \param p_physics_height
			* \return
			*/
			static bcFLOAT height_to_float(bcINT16 p_physics_height) noexcept;

			/**
			* \brief Convert render system height map height value to physics system height value
			* \param p_height
			* \return
			*/
			static bcINT16 height_to_int16(bcFLOAT p_height) noexcept;

			physics::bc_physics m_physics;

			static constexpr bcFLOAT s_height_field_y_scale_ratio = 0.1f;
		};

		inline physics::bc_physics& bc_physics_system::get_physics() noexcept
		{
			return m_physics;
		}

		inline bcFLOAT bc_physics_system::get_height_field_y_scale() const noexcept
		{
			return s_height_field_y_scale_ratio;
		}
		
		inline bcFLOAT bc_physics_system::height_to_float(bcINT16 p_physics_height) noexcept
		{
			return p_physics_height * s_height_field_y_scale_ratio;
		}

		inline bcINT16 bc_physics_system::height_to_int16(bcFLOAT p_height) noexcept
		{
			return p_height / s_height_field_y_scale_ratio;
		}
	}
}
