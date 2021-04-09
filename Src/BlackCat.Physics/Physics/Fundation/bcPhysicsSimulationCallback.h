// [12/02/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Math/bcVector3f.h"
#include "Physics/bcPhysicsApi.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		class bc_platform_actor;
		using bc_actor = bc_platform_actor< g_current_physics_api >;

		template< bc_physics_api TApi >
		class bc_platform_rigid_actor;
		using bc_rigid_actor = bc_platform_rigid_actor< g_current_physics_api >;

		template< bc_physics_api TApi >
		class bc_platform_shape;
		using bc_shape = bc_platform_shape< g_current_physics_api >;

		template< bc_physics_api TApi >
		class bc_platform_joint;
		using bc_joint = bc_platform_joint< g_current_physics_api >;

		enum class bc_trigger_type
		{
			in_touch,
			lost_touch
		};

		enum class bc_contact_type
		{
			in_contact,
			lost_contact
		};

		template< bc_physics_api TApi >
		struct bc_platform_trigger_pair_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_trigger_pair
		{
		public:
			using platform_pack = bc_platform_trigger_pair_pack< TApi >;

		public:
			bc_platform_trigger_pair() noexcept;

			explicit bc_platform_trigger_pair(platform_pack& p_pack) noexcept;

			bc_platform_trigger_pair(const bc_platform_trigger_pair&) noexcept;

			~bc_platform_trigger_pair();

			bc_platform_trigger_pair& operator=(const bc_platform_trigger_pair&) noexcept;

			const bc_rigid_actor* get_trigger_actor() const noexcept;

			const bc_shape* get_trigger_shape() const noexcept;

			const bc_rigid_actor* get_other_actor() const noexcept;

			const bc_shape* get_other_shape() const noexcept;

			bc_trigger_type trigger_type() const noexcept;

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

		using bc_trigger_pair = bc_platform_trigger_pair< g_current_physics_api >;

		template< bc_physics_api TApi >
		struct bc_platform_contact_pair_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_contact_pair
		{
		public:
			using platform_pack = bc_platform_contact_pair_pack< TApi >;

		public:
			bc_platform_contact_pair() noexcept;

			explicit bc_platform_contact_pair(platform_pack& p_pack) noexcept;

			bc_platform_contact_pair(const bc_platform_contact_pair&) noexcept;

			~bc_platform_contact_pair();

			bc_platform_contact_pair& operator=(const bc_platform_contact_pair&) noexcept;

			const bc_rigid_actor* get_actor0() const noexcept;

			const bc_rigid_actor* get_actor1() const noexcept;

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

		using bc_contant_pair = bc_platform_contact_pair< g_current_physics_api >;

		template< bc_physics_api TApi >
		struct bc_platform_contact_point_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_contact_point
		{
		public:
			using platform_pack = bc_platform_contact_point_pack< TApi >;

		public:
			bc_platform_contact_point() noexcept;

			explicit bc_platform_contact_point(platform_pack& p_pack) noexcept;

			bc_platform_contact_point(const bc_platform_contact_point&) noexcept;

			~bc_platform_contact_point();

			bc_platform_contact_point& operator=(const bc_platform_contact_point&) noexcept;

			core::bc_vector3f get_position() const noexcept;

			bcFLOAT get_separation() const noexcept;

			core::bc_vector3f get_normal() const noexcept;

			bcUINT32 get_face_index0() const noexcept;

			core::bc_vector3f get_impulse() const noexcept;

			bcUINT32 get_face_index1() const noexcept;

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

		using bc_contact_point = bc_platform_contact_point< g_current_physics_api >;

		template< bc_physics_api TApi >
		struct bc_platform_contact_shape_pair_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_contact_shape_pair
		{
		public:
			using platform_pack = bc_platform_contact_shape_pair_pack< TApi >;

		public:
			bc_platform_contact_shape_pair() noexcept;

			explicit bc_platform_contact_shape_pair(platform_pack& p_pack) noexcept;

			bc_platform_contact_shape_pair(const bc_platform_contact_shape_pair&) noexcept;

			~bc_platform_contact_shape_pair();

			bc_platform_contact_shape_pair& operator=(const bc_platform_contact_shape_pair&) noexcept;

			const bc_shape* get_shape0() const noexcept;

			const bc_shape* get_shape1() const noexcept;

			bcUINT32 get_contacts(bc_contact_point* p_buffer, bcUINT32 p_buffer_size) const noexcept;

			bcUINT32 get_contact_count() const noexcept;

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

		using bc_contact_shape_pair = bc_platform_contact_shape_pair< g_current_physics_api >;

		/**
		 * \brief An interface class that the user can implement in order to receive simulation events.
		 * States should not be modified from within the callbacks. In particular objects should not
		 * be created or destroyed. If state modification is needed then the changes should be stored to a buffer
		 * and performed after the simulation step.
		 * \n Threading: It is not necessary to make this class thread safe as it will only be called in the context of the user thread.
		 */
		class bci_physics_simulation_callback
		{
		public:
			virtual ~bci_physics_simulation_callback() = default;

			virtual void on_joint_break(bc_joint* p_joints, bcUINT32 p_count) = 0;

			virtual void on_wake(bc_actor* p_actors, bcUINT32 p_count) = 0;

			virtual void on_sleep(bc_actor* p_actors, bcUINT32 p_count) = 0;

			/**
			 * \brief This is called with the current trigger pair events.
			 * \param[in] p_pairs - The trigger pairs which caused events.
			 * \param[in] p_count - The number of trigger pairs.
			 */
			virtual void on_trigger(bc_trigger_pair* p_pairs, bcUINT32 p_count) = 0;

			/**
			 * \brief This is called when certain contact events occur.
			 * \param[in] p_pair Information on the two actors whose shapes triggered a contact report.
			 * \param[in] p_shape_pairs The contact pairs of two actors for which contact reports have been requested.
			 * \param[in] p_count The number of provided contact pairs.
			 */
			virtual void on_contact(const bc_contant_pair& p_pair, const bc_contact_shape_pair* p_shape_pairs, bcUINT32 p_count) = 0;
		};
	}
}