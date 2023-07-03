// [22/03/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Physics/Shape/bcShape.h"
#include "Physics/Body/bcRigidActor.h"
#include "Physics/bcPhysicsApi.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		class bc_platform_ccontroller;
		using bc_ccontroller = bc_platform_ccontroller<g_current_physics_api>;
		
		/**
		 * \brief Filtering callback for CCT vs CCT.
		 * \n To make each CCT collide against all other CCTs, return true.
		 * \n To make each CCT freely go through all other CCTs, just return false.
		 */
		using bc_ccontroller_collision_filter_callback = core::bc_delegate<bool(const bc_ccontroller&, const bc_ccontroller&)>;
		
		template< bc_physics_api TApi >
		class bc_platform_ccontroller;
		
		template< bc_physics_api TApi >
		struct bc_platform_ccontroller_shape_hit_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_ccontroller_shape_hit
		{
		public:
			using platform_pack = bc_platform_ccontroller_shape_hit_pack<TApi>;
			
		public:
			bc_platform_ccontroller_shape_hit() = default;

			explicit bc_platform_ccontroller_shape_hit(platform_pack& p_pack);

			bc_platform_ccontroller_shape_hit(const bc_platform_ccontroller_shape_hit&) noexcept = default;

			~bc_platform_ccontroller_shape_hit() = default;

			bc_platform_ccontroller_shape_hit& operator=(const bc_platform_ccontroller_shape_hit&) noexcept = default;

			/**
			 * \brief Current controller
			 * \return 
			 */
			bc_platform_ccontroller<TApi> get_ccontroller() const noexcept;
			
			core::bc_vector3f get_world_pos() const noexcept;
			
			core::bc_vector3f get_world_normal() const noexcept;
			
			core::bc_vector3f get_move_dir() const noexcept;
			
			bcFLOAT get_move_length() const noexcept;
			
			/**
			 * \brief Touched actor.
			 * \return 
			 */
			bc_rigid_actor get_actor() const noexcept;
			
			/**
			 * \brief Touched shape.
			 * \return 
			 */
			bc_shape get_shape() const noexcept;

			/**
			 * \brief Touched triangle index (only for meshes/height-fields)
			 * \return 
			 */
			bcUINT32 get_triangle_index() const noexcept;
			
			platform_pack& get_platform_pack()
			{
				return m_pack;
			}
		
		private:
			platform_pack m_pack;
		};

		template< bc_physics_api TApi >
		struct bc_platform_ccontroller_controller_hit_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_ccontroller_controller_hit
		{
		public:
			using platform_pack = bc_platform_ccontroller_controller_hit_pack<TApi>;

		public:
			bc_platform_ccontroller_controller_hit() = default;

			explicit bc_platform_ccontroller_controller_hit(platform_pack& p_pack);

			bc_platform_ccontroller_controller_hit(const bc_platform_ccontroller_controller_hit&) noexcept = default;

			~bc_platform_ccontroller_controller_hit() = default;

			bc_platform_ccontroller_controller_hit& operator=(const bc_platform_ccontroller_controller_hit&) noexcept = default;

			/**
			 * \brief Current controller
			 * \return
			 */
			bc_platform_ccontroller<TApi> get_ccontroller() const noexcept;

			core::bc_vector3f get_world_pos() const noexcept;

			core::bc_vector3f get_world_normal() const noexcept;

			core::bc_vector3f get_move_dir() const noexcept;

			bcFLOAT get_move_length() const noexcept;

			/**
			 * \brief Touched controller.
			 * \return
			 */
			bc_platform_ccontroller<TApi> get_other_ccontroller() const noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_ccontroller_shape_hit = bc_platform_ccontroller_shape_hit<g_current_physics_api>;
		using bc_ccontroller_controller_hit = bc_platform_ccontroller_controller_hit<g_current_physics_api>;
		
		class bci_ccontroller_hit_callback
		{
		public:
			virtual ~bci_ccontroller_hit_callback() = default;

			/**
			 * \brief Called when current controller hits a shape.
			 * \n This is called when the CCT moves and hits a shape. This will not be called when a moving shape hits a non-moving CCT.
			 * \param p_hit 
			 */
			virtual void on_shape_hit(const bc_ccontroller_shape_hit& p_hit) = 0;
			
			/**
			 * \brief Called when current controller hits another controller.
			 * \param p_hit 
			 */
			virtual void on_ccontroller_hit(const bc_ccontroller_controller_hit& p_hit) = 0;
		};
	}	
}