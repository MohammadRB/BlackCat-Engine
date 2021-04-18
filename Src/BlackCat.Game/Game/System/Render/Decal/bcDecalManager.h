// [02/21/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Container/bcListPool.h"
#include "Core/Concurrency/bcTask.h"
#include "Core/Messaging/Query/bcQueryContext.h"
#include "Core/Messaging/Query/bcQueryProviderHandle.h"
#include "Core/Utility/bcObjectPool.h"
#include "Game/System/Render/Decal/bcDecal.h"
#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Game/System/Render/Decal/bcDecalIteratorBuffer.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_system;
		class bc_material_manager;
		
		struct _bc_decal_desc_entry
		{
			core::bc_string m_material_name;
			bcUINT32 m_u0;
			bcUINT32 m_v0;
			bcUINT32 m_u1;
			bcUINT32 m_v1;
			bcFLOAT m_width;
			bcFLOAT m_height;
			bcFLOAT m_depth;
			bcFLOAT m_lod_scale;
			bc_render_group m_group;
			bool m_auto_remove;
		};

		class _bc_decal_entry : public bc_decal
		{
		public:
			using string_hash = std::hash<const bcCHAR*>;
			
		public:
			_bc_decal_entry(bc_decal p_decal, string_hash::result_type p_hash)
				: bc_decal(std::move(p_decal)),
				m_hash(p_hash)
			{
			}

			string_hash::result_type m_hash;
		};

		class _bc_decal_instance_entry : public bc_decal_instance
		{
		public:
			using iterator_type = core::bc_list_pool<_bc_decal_instance_entry>::iterator;
			
		public:
			_bc_decal_instance_entry(bc_decal_instance p_instance)
				: bc_decal_instance(std::move(p_instance)),
				m_iterator(nullptr, nullptr)
			{
			}

			iterator_type m_iterator;
		};
		
		class BC_GAME_DLL bc_decal_manager
		{
		private:
			using string_hash = _bc_decal_entry::string_hash;
			using decal_desc_map = core::bc_unordered_map_program<string_hash::result_type, _bc_decal_desc_entry>;
			using decal_map = core::bc_unordered_map<string_hash::result_type, bc_decal*>;
			using decal_instance_container = core::bc_list_pool<_bc_decal_instance_entry>;

		public:
			using iterator_buffer = bc_decal_iterator_buffer<decal_instance_container>;
			
		public:
			explicit bc_decal_manager(bc_material_manager& p_material_manager);

			bc_decal_manager(bc_decal_manager&&) noexcept = delete;

			~bc_decal_manager();

			bc_decal_manager& operator=(bc_decal_manager&&) noexcept = delete;

			/**
			 * \brief Read decal json file and load decal descriptors
			 * \param p_decal_file 
			 */
			void read_decal_file(const bcECHAR* p_decal_file);

			/**
			 * \brief Try to found decal and load all associated resources
			 * \n ThreadSafe
			 * \param p_name 
			 * \return 
			 */
			bc_decal_ptr load_decal(const bcCHAR* p_name) noexcept;
			
			/**
			 * \brief Find decal description and load associated resources from its descriptor. If decal has already been loaded return pointer to it.
			 * Throw exception if material descriptor not found.
			 * \ThreadSafe
			 * \param p_name 
			 * \return 
			 */
			bc_decal_ptr load_decal_throw(const bcCHAR* p_name);

			/**
			 * \brief Create a decal without a reference to owning actor.
			 * Decals without reference will be removed automatically.
			 * \n ThreadSafe
			 * \param p_decal_name 
			 * \param p_local_position 
			 * \param p_local_rotation
			 * \return 
			 */
			bc_decal_instance* create_decal(const bcCHAR* p_decal_name,
				const core::bc_vector3f& p_local_position,
				const core::bc_matrix3f& p_local_rotation);

			bc_decal_instance* create_decal(const bcCHAR* p_decal_name,
				const core::bc_vector3f& p_local_position,
				const core::bc_matrix3f& p_local_rotation,
				bc_render_group p_render_group);

			/**
			 * \brief Create a decal with a strong reference to owning actor.
			 * Decals with strong reference will be removed when their reference count reach to zero.
			 * \n ThreadSafe
			 * \param p_decal_name 
			 * \param p_actor 
			 * \param p_local_position 
			 * \param p_local_rotation 
			 * \param p_attached_node_index 
			 * \return 
			 */
			bc_decal_instance_ptr create_decal(const bcCHAR* p_decal_name,
				const bc_actor& p_actor,
				const core::bc_vector3f& p_local_position,
				const core::bc_matrix3f& p_local_rotation,
				bc_mesh_node::node_index_t p_attached_node_index = bc_mesh_node::s_invalid_index);

			bc_decal_instance_ptr create_decal(const bcCHAR* p_decal_name,
				const bc_actor& p_actor,
				const core::bc_vector3f& p_local_position,
				const core::bc_matrix3f& p_local_rotation,
				bc_render_group p_render_group,
				bc_mesh_node::node_index_t p_attached_node_index = bc_mesh_node::s_invalid_index);

			void update_decal_lifespans(const core_platform::bc_clock::update_param& p_clock) noexcept;
			
			core::bc_task<void> update_decal_lifespans_async(const core_platform::bc_clock::update_param& p_clock) noexcept;
			
			void destroy_decal(bc_decal* p_decal);
			
			void destroy_decal_instance(bc_decal_instance* p_instance);
			
		private:
			core::bc_query_context_ptr _get_query_context() const;

			bc_material_manager* m_material_manager;
			core::bc_query_provider_handle m_decals_query_handle;
			bcFLOAT m_update_interval_seconds;

			core::bc_concurrent_object_pool<_bc_decal_entry> m_decals_pool;
			core_platform::bc_mutex m_decals_mutex;
			decal_desc_map m_decal_descriptions;
			decal_map m_decals;

			decal_instance_container m_decal_instances_pool;
			mutable core_platform::bc_mutex m_decal_instances_mutex;
		};
	}
}