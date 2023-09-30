// [03/12/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Math/bcVector3f.h"
#include "Physics/bcPhysicsApi.h"

namespace black_cat::physics
{
	template< bc_physics_api TApi >
	class bc_platform_rigid_actor;
	using bc_rigid_actor = bc_platform_rigid_actor< g_current_physics_api >;

	template< bc_physics_api TApi >
	class bc_platform_shape;
	using bc_shape = bc_platform_shape< g_current_physics_api >;

	template< bc_physics_api TApi >
	struct bc_platform_contact_modify_set_pack
	{
	};

	template< bc_physics_api TApi >
	class bc_platform_contact_modify_set
	{
	public:
		using platform_pack = bc_platform_contact_modify_set_pack< TApi >;

	public:
		bc_platform_contact_modify_set() noexcept;

		explicit bc_platform_contact_modify_set(platform_pack& p_pack) noexcept;

		bc_platform_contact_modify_set(const bc_platform_contact_modify_set&) noexcept;

		~bc_platform_contact_modify_set();

		bc_platform_contact_modify_set& operator=(const bc_platform_contact_modify_set&) noexcept;

		/**
		 * \brief Get the position of a specific contact point in the set.
		 */
		core::bc_vector3f get_point(bcUINT32 p_index) const noexcept;

		/**
		 * \brief Alter the position of a specific contact point in the set.
		 */
		void set_point(bcUINT32 p_index, const core::bc_vector3f& p_point) noexcept;

		/**
		 * \brief Get the contact normal of a specific contact point in the set.
		 * The contact normal points from the second shape to the first shape.
		 */
		core::bc_vector3f get_normal(bcUINT32 p_index) const noexcept;

		/**
		 * \brief Alter the contact normal of a specific contact point in the set.
		 * \note Changing the normal can cause contact points to be ignored.
		 * \note This must be a normalized vector.
		 */
		void set_normal(bcUINT32 p_index, const core::bc_vector3f& p_normal) noexcept;

		/**
		 * \brief Get the separation of a specific contact point in the set.
		 * This value can be either positive or negative. A negative value denotes penetration whereas a positive value denotes separation.
		 */
		bcFLOAT get_separation(bcUINT32 p_index) const noexcept;

		/**
		 * \brief Alter the separation of a specific contact point in the set.
		 */
		void set_separation(bcUINT32 p_index, bcFLOAT p_sep) noexcept;

		/**
		 * \brief Get the face index with respect to the first shape of the pair for a specific contact point in the set.
		 */
		bcUINT32 getInternalFaceIndex0(bcUINT32 p_index) const noexcept;

		/**
		 * \brief Get the face index with respect to the second shape of the pair for a specific contact point in the set.
		 */
		bcUINT32 getInternalFaceIndex1(bcUINT32 p_index) const noexcept;

		/**
		 * \brief Ignore the contact point.
		 * If a contact point is ignored then no force will get applied at this point. This can be used to disable collision in certain areas of a shape, for example.
		 */
		void ignore(bcUINT32 p_index) noexcept;

		/**
		 * \brief The number of contact points in the set.
		 */
		bcUINT32 size() const noexcept;

		platform_pack& get_platform_pack() noexcept
		{
			return m_pack;
		}

		const platform_pack& get_platform_pack() const noexcept
		{
			return m_pack;
		}

	private:
		platform_pack m_pack;
	};

	using bc_contact_modify_set = bc_platform_contact_modify_set<g_current_physics_api>;

	template< bc_physics_api TApi >
	struct bc_platform_contact_modify_pair_pack
	{
	};

	template< bc_physics_api TApi >
	class bc_platform_contact_modify_pair
	{
	public:
		using platform_pack = bc_platform_contact_modify_pair_pack<TApi>;

	public:
		bc_platform_contact_modify_pair() noexcept;

		explicit bc_platform_contact_modify_pair(platform_pack& p_pack) noexcept;

		bc_platform_contact_modify_pair(const bc_platform_contact_modify_pair&) noexcept;

		~bc_platform_contact_modify_pair();

		bc_platform_contact_modify_pair& operator=(const bc_platform_contact_modify_pair&) noexcept;

		const bc_rigid_actor& get_actor0() const noexcept;

		const bc_rigid_actor& get_actor1() const noexcept;

		const bc_shape& get_shape0() const noexcept;

		const bc_shape& get_shape1() const noexcept;

		bool is_ccd_contact() const noexcept;

		/**
		 * \brief An array of contact points between these two shapes.
		 * \return 
		 */
		bc_contact_modify_set& get_contacts() noexcept;

		platform_pack& get_platform_pack()
		{
			return m_pack;
		}

		const platform_pack& get_platform_pack() const noexcept
		{
			return m_pack;
		}
			
	private:
		platform_pack m_pack;
	};

	using bc_contact_modify_pair = bc_platform_contact_modify_pair<g_current_physics_api>;
		
	/**
	 * \brief An interface class that the user can implement in order to modify contact constraints.
	 * \n Threading: The callback execute in simulation threads.
	 */
	class bci_contact_modify_callback
	{
	public:
		virtual ~bci_contact_modify_callback() = default;
		
		/**
		 * \brief Passes modifiable arrays of contacts to the application.
		 */
		virtual void on_contact_modify(bc_contact_modify_pair* const p_pairs, bcUINT32 p_count) = 0;
	};
}
