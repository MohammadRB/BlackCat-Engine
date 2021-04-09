// [04/13/2017 MRB]

#pragma once

#include "Physics/bcPhysicsApi.h"
#include "Physics/Fundation/bcMemoryBuffer.h"
#include "Physics/Shape/bcHeightField.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		struct bc_platform_serialize_buffer_pack
		{
		};

		/**
		 * \brief Collection class for serialization and deserialization.
		 * \tparam TApi 
		 */
		template< bc_physics_api TApi >
		class bc_platform_serialize_buffer
		{
		public:
			using platform_pack = bc_platform_serialize_buffer_pack< TApi >;

		public:
			bc_platform_serialize_buffer() noexcept;

			explicit bc_platform_serialize_buffer(platform_pack& p_pack);

			bc_platform_serialize_buffer(bc_platform_serialize_buffer&&) noexcept;

			~bc_platform_serialize_buffer();

			bc_platform_serialize_buffer& operator=(bc_platform_serialize_buffer&&) noexcept;

			/**
			 * \brief Get number of objects in the collection
			 * \return 
			 */
			bcUINT32 get_object_count() const noexcept;

			/**
			 * \brief Get buffer used to deserialize objects. This buffer must persist while deserialized objects
			 * exist. This buffer allocated with aligned allocation routines and must be deallocate with aligned
			 * deallocation routines.
			 * This method can be called only one time.
			 * \return 
			 */
			void* get_deserialize_buffer() noexcept;

			/**
			 * \brief Try to retrieve a bc_height_field variable from deserialized collection at given index.
			 * \param p_index 
			 * \param p_height_field 
			 * \return True if succeeded
			 */
			bool get(bcUINT32 p_index, bc_height_field& p_height_field) noexcept;

			/**
			 * \brief Add a bc_height_field object to collection for serialization.
			 * \param p_height_field 
			 */
			void add(bc_height_field p_height_field) noexcept;

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

		using bc_serialize_buffer = bc_platform_serialize_buffer< g_current_physics_api >;
	}
}