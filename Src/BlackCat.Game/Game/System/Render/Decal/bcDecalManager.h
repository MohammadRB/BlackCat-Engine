// [21/02/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcObjectPool.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/Render/Decal/bcDecal.h"
#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Game/System/Render/Decal/bcDecalManagerContainer.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_render_system;
	class bc_material_manager;
		
	struct _bc_decal_desc_entry
	{
		core::bc_string m_name;
		core::bc_string m_material_name;
		core::bc_nullable<bcUINT32> m_u0;
		core::bc_nullable<bcUINT32> m_v0;
		core::bc_nullable<bcUINT32> m_u1;
		core::bc_nullable<bcUINT32> m_v1;
		bcFLOAT m_width;
		bcFLOAT m_height;
		bcFLOAT m_depth;
		bcFLOAT m_view_distance;
		bc_actor_render_group m_group;
		bool m_auto_remove;
	};

	class _bc_decal_entry : public bc_decal
	{
	public:
		using string_hash = std::hash<core::bc_string_view>;
			
	public:
		_bc_decal_entry(bc_decal p_decal, bcSIZE p_hash)
			: bc_decal(std::move(p_decal)),
			  m_hash(p_hash)
		{
		}

		bcSIZE m_hash;
	};
		
	class BC_GAME_DLL bc_decal_manager
	{
	private:
		using string_hash = _bc_decal_entry::string_hash;
		using decal_desc_map = core::bc_unordered_map_program<bcSIZE, _bc_decal_desc_entry>;
		using decal_map = core::bc_unordered_map<bcSIZE, bc_decal*>;

	public:
		explicit bc_decal_manager(bc_material_manager& p_material_manager);

		bc_decal_manager(bc_decal_manager&&) noexcept;

		~bc_decal_manager();

		bc_decal_manager& operator=(bc_decal_manager&&) noexcept;

		bc_decal_manager_container create_container();

		/**
			 * \brief Read decal json file and load decal descriptors
			 * \param p_decal_file 
			 */
		void read_decal_file(const bcECHAR* p_decal_file);

		core::bc_vector_frame<core::bc_string_view> get_decal_names() const;

		bool is_decal_auto_remove(core::bc_string_view p_name) const noexcept;
			
		/**
			 * \brief Try to found decal and load all associated resources
			 * \n ThreadSafe
			 * \param p_name 
			 * \return 
			 */
		bc_decal_ptr load_decal(core::bc_string_view p_name) noexcept;
			
		/**
			 * \brief Find decal description and load associated resources from its descriptor. If decal has already been loaded return pointer to it.
			 * Throw exception if material descriptor not found.
			 * \ThreadSafe
			 * \param p_name 
			 * \return 
			 */
		bc_decal_ptr load_decal_throw(core::bc_string_view p_name);
			
		void destroy_decal(bc_decal* p_decal);
			
	private:
		bcFLOAT _calculate_default_view_distance(bcFLOAT p_width, bcFLOAT p_height, bcFLOAT p_depth);

		bc_material_manager* m_material_manager;

		decal_desc_map m_decal_descriptions;
		platform::bc_mutex m_decals_mutex;
		core::bc_concurrent_object_pool<_bc_decal_entry> m_decals_pool;
		decal_map m_decals;
	};
}
