// [03/22/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcDelegate.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcPhysicsReference.h"
#include "Physics/bcPhysicsRef.h"
#include "Physics/Body/bcRigidDynamic.h"
#include "Physics/Fundation/bcCControllerSimulationCallback.h"
#include "Physics/Fundation/bcMaterial.h"
#include "Physics/Collision/bcSceneQuery.h"

namespace black_cat
{
	namespace physics
	{
		struct bc_ccontroller_desc
		{
			bc_ccontroller_desc(const core::bc_vector3f& p_position, bcFLOAT p_capsule_height, bcFLOAT p_capsule_radius, bc_material p_material) noexcept;

			bc_ccontroller_desc& with_up(const core::bc_vector3f& p_up) noexcept;
			
			bc_ccontroller_desc& with_slop_limit(bcFLOAT p_slope_limit) noexcept;
			
			bc_ccontroller_desc& with_contact_offset(bcFLOAT p_contact_offset) noexcept;
			
			bc_ccontroller_desc& with_step_offset(bcFLOAT p_step_offset) noexcept;
			
			bc_ccontroller_desc& with_density(bcFLOAT p_density) noexcept;

			bc_ccontroller_desc& with_hit_callback(bci_ccontroller_hit_callback* p_callback) noexcept;
			
			/**
			 * \brief The position of the character
			 */
			core::bc_vector3f m_position;
			/**
			 * \brief Specifies the 'up' direction.
			 * \n Default value is (0, 1, 0)
			 */
			core::bc_vector3f m_up;
			/**
			 * \brief The maximum slope which the character can walk up.
			 * The value is expressed in degree.
			 * \n Default value is 30 degree.
			 */
			bcFLOAT m_slope_limit;
			/**
			 * \brief The contact offset used by the controller.
			 * \n Default value is 0.1.
			 */
			bcFLOAT m_contact_offset;
			/**
			 * \brief Defines the maximum height of an obstacle which the character can climb.
			 * \n Default value is 0.5.
			 */
			bcFLOAT m_step_offset;
			/**
			 * \brief Density of underlying kinematic actor
			 * \n Default value is 10.
			 */
			bcFLOAT m_density;
			/**
			 * \brief The height of the capsule.
			 */
			bcFLOAT m_capsule_height;
			/**
			 * \brief The radius of the capsule.
			 */
			bcFLOAT m_capsule_radius;
			/**
			 * \brief The material which is used for controller
			 */
			bc_material m_material;
			/**
			 * \brief Specifies a user hit callback.
			 */
			bci_ccontroller_hit_callback* m_hit_callback;
		};

		inline bc_ccontroller_desc::bc_ccontroller_desc(const core::bc_vector3f& p_position, bcFLOAT p_capsule_height, bcFLOAT p_capsule_radius, bc_material p_material) noexcept
			: m_position(p_position),
			m_up(core::bc_vector3f::up()),
			m_slope_limit(30),
			m_contact_offset(p_capsule_radius * 0.25f),
			m_step_offset(0.5f),
			m_density(10),
			m_capsule_height(p_capsule_height),
			m_capsule_radius(p_capsule_radius),
			m_material(p_material),
			m_hit_callback(nullptr)
		{
		}

		inline bc_ccontroller_desc& bc_ccontroller_desc::with_up(const core::bc_vector3f& p_up) noexcept
		{
			m_up = p_up;
			return *this;
		}

		inline bc_ccontroller_desc& bc_ccontroller_desc::with_slop_limit(bcFLOAT p_slope_limit) noexcept
		{
			m_slope_limit = p_slope_limit;
			return *this;
		}

		inline bc_ccontroller_desc& bc_ccontroller_desc::with_contact_offset(bcFLOAT p_contact_offset) noexcept
		{
			m_contact_offset = p_contact_offset;
			return *this;
		}

		inline bc_ccontroller_desc& bc_ccontroller_desc::with_step_offset(bcFLOAT p_step_offset) noexcept
		{
			m_step_offset = p_step_offset;
			return *this;
		}

		inline bc_ccontroller_desc& bc_ccontroller_desc::with_density(bcFLOAT p_density) noexcept
		{
			m_density = p_density;
			return *this;
		}

		inline bc_ccontroller_desc& bc_ccontroller_desc::with_hit_callback(bci_ccontroller_hit_callback* p_callback) noexcept
		{
			m_hit_callback = p_callback;
			return *this;
		}

		enum class bc_ccontroller_collision_flag : bcUBYTE
		{
			sides = core::bc_enum::value(0),		// Character is colliding to the sides.
			up = core::bc_enum::value(1),			// Character has collision above.
			down = core::bc_enum::value(2)			// Character has collision below.
		};
		
		template<bc_physics_api TApi>
		struct bc_platform_ccontroller_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_ccontroller : public bc_physics_reference
		{
		public:
			using platform_pack = bc_platform_ccontroller_pack<TApi>;
			
		public:
			bc_platform_ccontroller() noexcept;

			explicit bc_platform_ccontroller(platform_pack& p_pack) noexcept;

			bc_platform_ccontroller(const bc_platform_ccontroller& p_other) noexcept;

			~bc_platform_ccontroller() override;

			bc_platform_ccontroller& operator=(const bc_platform_ccontroller& p_other) noexcept;

			bc_rigid_dynamic get_actor() const noexcept;
			
			/**
			 * \brief The position retrieved by this function is the center of the collision shape.
			 * \return 
			 */
			core::bc_vector3f get_position() const noexcept;
			
			/**
			 * \brief The position controlled by this function is the center of the collision shape.
			 * \param p_position 
			 */
			void set_position(const core::bc_vector3f& p_position) noexcept;

			/**
			 * \brief Retrieve the "foot" position of the controller, i.e. the position of the bottom of the CCT's shape.
			 * \return
			 */
			core::bc_vector3f get_foot_position() const noexcept;

			/**
			 * \brief The position controlled by this function is the bottom of the collision shape, a.k.a. the foot position.
			 * \param p_position 
			 */
			void set_foot_position(const core::bc_vector3f& p_position) noexcept;

			core::bc_vector3f get_up() const noexcept;

			void set_up(const core::bc_vector3f& p_position) noexcept;
			
			bcFLOAT get_step_offset() const noexcept;
			
			void set_step_offset(bcFLOAT p_offset) noexcept;

			bcFLOAT get_contact_offset() const noexcept;
			
			void set_contact_offset(bcFLOAT p_offset) noexcept;

			bcFLOAT get_slope_limit() const noexcept;

			void set_slope_limit(bcFLOAT p_offset) noexcept;
			
			void resize(bcFLOAT p_height, bool p_align_foot = true) noexcept;

			/**
			 * \brief Moves the character
			 * \param p_displacement Displacement vector. It is typically a combination of vertical motion due to gravity and lateral motion when your character is moving.
			 * \param p_clock
			 * \param p_pre_filter
			 * \param p_post_filter 
			 * \param p_cc_vs_cc_filter 
			 * \return 
			 */
			bc_ccontroller_collision_flag move(const core::bc_vector3f& p_displacement,
				const core_platform::bc_clock::update_param& p_clock,
				bc_scene_query_pre_filter_callback* p_pre_filter = nullptr,
				bc_scene_query_post_filter_callback* p_post_filter = nullptr,
				bc_ccontroller_collision_filter_callback* p_cc_vs_cc_filter = nullptr) noexcept;
			
			bool is_valid() const noexcept override;

			void set_data(void* p_user_data) noexcept;

			void* get_data() const noexcept;
			
			platform_pack& get_platform_pack() noexcept override
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept override
			{
				return m_pack;
			}
		
		private:
			platform_pack m_pack;
		};

		using bc_ccontroller = bc_platform_ccontroller<g_current_physics_api>;
		using bc_ccontroller_ref = bc_physics_ref<bc_ccontroller>;
	}	
}