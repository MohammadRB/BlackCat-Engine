// [11/30/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcPhysicsReference.h"
#include "Physics/bcPhysicsRef.h"

namespace black_cat
{
	namespace physics
	{
		using bc_material_index = bcUINT16;

		template< bc_physics_api TApi >
		struct bc_platform_material_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_material : public bc_platform_physics_reference<TApi>
		{
		public:
			using platform_pack = bc_platform_material_pack< TApi >;

		public:
			bc_platform_material();

			bc_platform_material(const bc_platform_material&) noexcept;

			~bc_platform_material();

			bc_platform_material& operator=(const bc_platform_material&) noexcept;

			bcFLOAT get_static_friction() const noexcept;

			/**
			 * \brief Sets the coefficient of static friction.
			 * \param p_friction Coefficient of static friction. Range: [0, PX_MAX_F32)
			 */
			void set_static_friction(bcFLOAT p_friction) noexcept;

			bcFLOAT get_dynamic_friction() const noexcept;

			/**
			* \brief Sets the coefficient of dynamic friction.
			* \param p_friction Coefficient of dynamic friction. Range: [0, PX_MAX_F32)
			*/
			void set_dynamic_friction(bcFLOAT p_friction) noexcept;

			bcFLOAT get_restitution() const noexcept;

			/**
			 * \brief Sets the coefficient of restitution.
			 * \param p_restitution Coefficient of restitution. Range: [0,1]
			 */
			void set_restitution(bcFLOAT p_restitution) noexcept;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_material = bc_platform_material< g_current_physics_api >;
		using bc_material_ref = bc_physics_ref< bc_material >;
	}
}