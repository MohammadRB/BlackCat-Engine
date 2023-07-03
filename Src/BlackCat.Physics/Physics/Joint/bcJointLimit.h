// [05/12/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"

namespace black_cat
{
	namespace physics
	{
		struct bc_joint_limit
		{
			bc_joint_limit()
				: m_restitution(0),
				m_stiffness(0),
				m_damping(0)
			{
			}

			bc_joint_limit(bcFLOAT p_stiffness, bcFLOAT p_damping)
				: m_restitution(0),
				m_stiffness(p_stiffness),
				m_damping(p_damping)
			{
			}

			/**
			 * \brief Controls the amount of bounce when the joint hits a limit.
			 * \n A restitution value of 1.0 causes the joint to bounce back with the velocity which it hit the limit. A value of zero causes the joint to stop dead.
			 *
			 * <b>Range:</b> [0,1]<br>
			 * <b>Default:</b> 0.0
			 */
			const bcFLOAT m_restitution;
			
			/**
			* \brief if greater than zero, the limit is soft, i.e. a spring pulls the joint back to the limit
			* 
			* Range: [0, PX_MAX_F32)
			* Default: 0.0
			*/
			const bcFLOAT m_stiffness;
			
			/**
			* \brief if spring is greater than zero, this is the damping of the limit spring
			* 
			* Range: [0, PX_MAX_F32)
			* Default: 0.0
			*/
			const bcFLOAT m_damping;
		};

		struct bc_joint_linear_limit : public bc_joint_limit
		{
			explicit bc_joint_linear_limit(bcFLOAT p_limit)
				: bc_joint_limit(),
				m_limit(p_limit)
			{
			}

			bc_joint_linear_limit(bcFLOAT p_limit, bcFLOAT p_stiffness, bcFLOAT p_damping)
				: bc_joint_limit(p_stiffness, p_damping),
				m_limit(p_limit)
			{
			}

			/**
			 * \brief the extent of the limit.
			 * Range:(0, PX_MAX_F32)
			 * Default: PX_MAX_F32
			 */
			const bcFLOAT m_limit;
		};

		struct bc_joint_linear_limit_pair : public bc_joint_limit
		{
			explicit bc_joint_linear_limit_pair(bcFLOAT p_upper, bcFLOAT p_lower)
				: bc_joint_limit(),
				m_upper(p_upper),
				m_lower(p_lower)
			{
			}

			bc_joint_linear_limit_pair(bcFLOAT p_upper, bcFLOAT p_lower, bcFLOAT p_stiffness, bcFLOAT p_damping)
				: bc_joint_limit(p_stiffness, p_damping),
				m_upper(p_upper),
				m_lower(p_lower)
			{
			}

			/**
			* \brief the range of the limit.
			* Unit: Angular: Radians
			* Default: 0.0
			*/
			const bcFLOAT m_upper, m_lower;
		};

		struct bc_joint_angular_limit : public bc_joint_limit
		{
			bc_joint_angular_limit(bcFLOAT p_upper, bcFLOAT p_lower)
				: bc_joint_limit(),
				m_upper(p_upper),
				m_lower(p_lower)
			{
			}

			bc_joint_angular_limit(bcFLOAT p_upper, bcFLOAT p_lower, bcFLOAT p_stiffness, bcFLOAT p_damping)
				: bc_joint_limit(p_stiffness, p_damping),
				m_upper(p_upper),
				m_lower(p_lower)
			{
			}

			/**
			 * \brief the range of the limit.
			 * Unit: Angular: Radians
			 * Default: 0.0
			 */
			const bcFLOAT m_upper, m_lower;
		};

		struct bc_joint_cone_limit : public bc_joint_limit
		{
			bc_joint_cone_limit(bcFLOAT p_y_angle, bcFLOAT p_z_angle)
				: bc_joint_limit(),
				m_y_angle(p_y_angle),
				m_z_angle(p_z_angle)
			{
			}

			bc_joint_cone_limit(bcFLOAT p_y_angle, bcFLOAT p_z_angle, bcFLOAT p_stiffness, bcFLOAT p_damping)
				: bc_joint_limit(p_stiffness, p_damping),
				m_y_angle(p_y_angle),
				m_z_angle(p_z_angle)
			{
			}

			/**
			 * \brief the maximum angle from the Y axis of the constraint frame.
			 * Unit: Angular: Radians
			 * Range: Angular: (0,PI)<br>
			 * Default: PI/2
			 */
			const bcFLOAT m_y_angle;

			/**
			* \brief the maximum angle from the Z axis of the constraint frame.
			* Unit: Angular: Radians
			* Range: Angular: (0,PI)<br>
			* Default: PI/2
			*/
			const bcFLOAT m_z_angle;
		};
	}
}