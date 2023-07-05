// [04/12/2016 MRB]

#pragma once

#include <utility>
#include "Core/Math/bcVector3f.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcPhysicsReference.h"
#include "Physics/bcPhysicsRef.h"
#include "Physics/Fundation/bcTransform.h"

namespace black_cat::physics
{
	template<bc_physics_api TApi>
	class bc_platform_rigid_actor;
	using bc_rigid_actor = bc_platform_rigid_actor<g_current_physics_api>;

	template<bc_physics_api TApi>
	struct bc_platform_joint_pack
	{
	};

	template<bc_physics_api TApi>
	class bc_platform_joint : public bc_platform_physics_reference<TApi>
	{
	public:
		using platform_pack = bc_platform_joint_pack<TApi>;

	public:
		bc_platform_joint() noexcept;

		explicit bc_platform_joint(platform_pack& p_pack) noexcept;

		bc_platform_joint(const bc_platform_joint&) noexcept;

		virtual ~bc_platform_joint() override;

		bc_platform_joint& operator=(const bc_platform_joint&) noexcept;

		virtual std::pair<bc_rigid_actor, bc_rigid_actor> get_actors() const noexcept;

		/**
			 * \brief Set the joint local pose for first actor.
			 * This is the relative pose which locates the joint frame relative to the actor.
			 * \param[in] p_local_pose the local pose for the actor this joint
			 */
		virtual void set_local_pose0(const bc_transform& p_local_pose) noexcept;

		/**
			* \brief Set the joint local pose for second actor.
			* This is the relative pose which locates the joint frame relative to the actor.
			* \param[in] p_local_pose the local pose for the actor this joint
			*/
		virtual void set_local_pose1(const bc_transform& p_local_pose) noexcept;

		/**
			 * \brief get the joint local pose for first actor.
			 */
		virtual bc_transform get_local_pose0() const noexcept;

		/**
			 * \brief get the joint local pose for second actor.
			 */
		virtual bc_transform get_local_pose1() const noexcept;

		/**
			 * \brief get the relative pose for this joint
			 * This function returns the pose of the joint frame of actor1 relative to actor0
			 */
		virtual bc_transform get_relative_transform() const noexcept;

		/**
			 * \brief get the relative linear velocity of the joint.
			 * This function returns the linear velocity of the origin of the constraint frame of actor1, 
			 * relative to the origin of the constraint frame of actor0. 
			 * The value is returned in the constraint frame of actor0
			 */
		virtual core::bc_vector3f get_relative_linear_velocity() const noexcept;

		/**
			 * \brief get the relative angular velocity of the joint
			 * This function returns the angular velocity of  actor1 relative to actor0. The value is returned in the constraint frame of actor0
			 */
		virtual core::bc_vector3f get_relative_angular_velocity() const noexcept;

		/**
			 * \brief set the break force for this joint.
			 * The force and torque are measured in the joint frame of the first actor
			 * \param[in] p_force the maximum force the joint can apply before breaking
			 * \param[in] p_torque the maximum torque the joint can apply before breaking
			 */
		virtual void set_break_force(bcFLOAT p_force, bcFLOAT p_torque) noexcept;

		/**
			 * \brief get the break force for this joint.
			 * \retrun an std::pair that first element is force and second is torque
			 */
		virtual std::pair<bcFLOAT, bcFLOAT> get_break_force() const noexcept;

		bool get_visualization() const noexcept;
			
		void set_visualization(bool p_value) noexcept;
			
		virtual bool is_broken() const noexcept;

		bool is_valid() const noexcept override;

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

	using bc_joint = bc_platform_joint<g_current_physics_api>;
	using bc_joint_ref = bc_physics_ref<bc_joint>;
}
