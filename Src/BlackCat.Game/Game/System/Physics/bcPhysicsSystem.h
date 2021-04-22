// [12/21/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Utility/bcObjectPool.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Utility/bcTemplateMetaType.h"
#include "Physics/bcUtility.h"
#include "PhysicsImp/Body/bcRigidBody.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "Game/System/Render/Material/bcColliderMaterial.h"
#include "Game/System/Physics/bcPxShapeData.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_json_key_value;
	}
	
	namespace game
	{
		class bc_material_manager;
		class bc_actor;
		class bc_mesh_component;
		class bc_height_map_component;

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
			 * \brief Retrieve stored pointer of game actor
			 * \param p_px_actor
			 * \return
			 */
			bc_actor get_game_actor(const physics::bc_actor& p_px_actor) const noexcept;
			
			/**
			 * \brief Store a pointer of game actor inside physics actor so it can be accessible within physics callbacks
			 * \param p_px_actor 
			 * \param p_actor 
			 */
			void set_game_actor(physics::bc_actor& p_px_actor, const bc_actor& p_actor) noexcept;

			void create_px_shapes_from_height_map(const bc_material_manager& p_material_manager, 
				physics::bc_rigid_static& p_rigid_static, 
				const bc_height_map_component& p_height_map);

			void create_px_shapes_from_mesh(const bc_material_manager& p_material_manager, 
				physics::bc_rigid_body& p_px_actor, 
				const bc_mesh_component& p_mesh, 
				const core::bc_json_key_value* p_collider_materials);

			void clear_px_shapes_data(const physics::bc_rigid_body& p_px_actor);
			
			void update(const core_platform::bc_clock::update_param& p_clock_update_param);

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

			static constexpr bcFLOAT s_height_field_y_scale_ratio = 0.1f;
			
			physics::bc_physics m_physics;
			core::bc_concurrent_object_pool<bc_px_shape_data> m_shape_data_pool;
		};

		inline physics::bc_physics& bc_physics_system::get_physics() noexcept
		{
			return m_physics;
		}

		inline bcFLOAT bc_physics_system::get_height_field_y_scale() const noexcept
		{
			return s_height_field_y_scale_ratio;
		}

		inline bc_actor bc_physics_system::get_game_actor(const physics::bc_actor& p_px_actor) const noexcept
		{
			return bc_actor(static_cast<bc_actor_index>(reinterpret_cast<bcINTPTR>(p_px_actor.get_data())));
		}
		
		inline void bc_physics_system::set_game_actor(physics::bc_actor& p_px_actor, const bc_actor& p_actor) noexcept
		{
			p_px_actor.set_data(reinterpret_cast<void*>(static_cast<bcINTPTR>(p_actor.get_index())));
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