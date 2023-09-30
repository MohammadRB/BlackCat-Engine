// [02/12/2016 MRB]

#pragma once

#include "Core/Utility/bcEnumOperand.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/Collision/bcCollisionGroup.h"

namespace black_cat::physics
{
	template< bc_physics_api TApi >
	class bc_platform_actor;
	using bc_actor = bc_platform_actor< g_current_physics_api >;

	template< bc_physics_api TApi >
	class bc_platform_shape;
	using bc_shape = bc_platform_shape< g_current_physics_api >;

	enum class bc_filter_object_flag
	{
		rigid_static = core::bc_enum::value(0),
		rigid_dynamic = core::bc_enum::value(1),
		kinematic = core::bc_enum::value(2),
		trigger = core::bc_enum::value(3)
	};

	/**
	 * \brief Filter callback to specify handling of collision pairs.
	 * States should not be modified from within the callbacks.
	 * \n Threading: The callbacks may execute in user threads or simulation threads, possibly simultaneously.
	 */
	class bci_contact_filter_callback
	{
	public:
		virtual ~bci_contact_filter_callback() = default;

		/**
		 * \brief Filter method to specify how a pair of potentially colliding objects should be processed.
		 * \param[in] p_attr0 The filter attribute of the first object
		 * \param[in] p_collision_flag0 The custom filter data of the first object
		 * \param[in] p_actor0 Actor pointer of the first object
		 * \param[in] p_shape0 Shape pointer of the first object (NULL if the object has no shapes, for example in the case of a particle system)
		 * \param[in] p_attr1 The filter attribute of the second object
		 * \param[in] p_collision_flag1 The custom filter data of the second object
		 * \param[in] p_actor1 Actor pointer of the second object
		 * \param[in] p_shape1 Shape pointer of the second object (NULL if the object has no shapes, for example in the case of a  particle system)
		 * \return Filter flags defining whether the pair should be discarded, temporarily ignored or processed and whether the pair
		 * should be tracked and send a report on pair deletion through the filter callback
		 */
		virtual bool pair_found(bc_filter_object_flag p_attr0, 
		                        bc_collision_filter p_collision_flag0, 
		                        const bc_actor* p_actor0, 
		                        const bc_shape* p_shape0,
		                        bc_filter_object_flag p_attr1,
		                        bc_collision_filter p_collision_flag1,
		                        const bc_actor* p_actor1,
		                        const bc_shape* p_shape1) = 0;
	};
}
