// [09/12/2016 MRB]

#pragma once

#include "Physics/bcPhysicsApi.h"

namespace black_cat::physics
{
	template<bc_physics_api TApi>
	struct bc_platform_physics_reference_pack
	{
	};

	/**
		 * \brief Base class for reference counted objects
		 * \tparam TApi 
		 */
	template< bc_physics_api TApi >
	class bc_platform_physics_reference
	{
	public:
		using platform_pack = bc_platform_physics_reference_pack<TApi>;

	public:
		virtual bool is_valid() const noexcept = 0;

		bool operator==(const bc_platform_physics_reference& p_other) const noexcept;

		bool operator!=(const bc_platform_physics_reference& p_other) const noexcept;

		bool operator==(std::nullptr_t) const noexcept;

		bool operator!=(std::nullptr_t) const noexcept;

		virtual platform_pack& get_platform_pack() noexcept = 0;
			
		virtual const platform_pack& get_platform_pack() const noexcept = 0;
		
	protected:
		bc_platform_physics_reference() noexcept;

		bc_platform_physics_reference(const bc_platform_physics_reference&) noexcept;

		virtual ~bc_platform_physics_reference();

		bc_platform_physics_reference& operator=(const bc_platform_physics_reference&) noexcept;
	};

	using bc_physics_reference = bc_platform_physics_reference< g_current_physics_api >;
}
