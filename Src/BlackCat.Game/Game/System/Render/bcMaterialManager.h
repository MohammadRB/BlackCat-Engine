// [03/06/2017 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcRefCountPtr.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/Object/Mesh/bcMaterial.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
	}

	namespace game
	{
		class bc_file_system;
		class bc_render_system;

		class _bc_render_material_file_description
		{
		public:
			core::bc_vector4f m_diffuse_color;
			bcFLOAT m_specular_intensity;
			bcFLOAT m_specular_power;
			core::bc_string m_diffuse_map_name;
			core::bc_string m_normal_map_name;
			core::bc_string m_specular_map_name;
		};

		class bc_render_material_description
		{
		public:
			core::bc_vector4f m_diffuse;
			bcFLOAT m_specular_intensity;
			bcFLOAT m_specular_power;

			graphic::bc_texture2d_content_ptr m_diffuse_map;
			graphic::bc_texture2d_content_ptr m_normal_map;
			graphic::bc_texture2d_content_ptr m_specular_map;
		};

		class BC_GAME_DLL bc_material_manager
		{
		private:
			using string_hash = std::hash< const bcCHAR* >;
			using material_desc_map = core::bc_unordered_map_program< string_hash::result_type, _bc_render_material_file_description >;
			using material_map = core::bc_unordered_map< string_hash::result_type, core::bc_unique_ptr< bc_render_material > >;
			using default_diffuse_map = core::bc_unordered_map_program< bcUINT32, graphic::bc_texture2d_content_ptr >;

		public:
			bc_material_manager(core::bc_content_stream_manager& p_content_stream, bc_render_system& p_render_system);

			bc_material_manager(bc_material_manager&&) noexcept;

			~bc_material_manager();

			bc_material_manager& operator=(bc_material_manager&&) noexcept;

			/**
			 * \brief Read material json file and load material descriptors
			 * \param p_material_json_file 
			 */
			void read_material_file(const bcECHAR* p_material_json_file);

			/**
			 * \brief Try to found associated material description and load material from it's descriptor.
			 * If material has already loaded return pointer to it.
			 * \ThreadSafe
			 * \param p_name 
			 * \return null_ptr if material descriptor not found.
			 */
			bc_render_material_ptr load_material(const bcCHAR* p_name) noexcept;

			bc_render_material_ptr load_material(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name) noexcept;

			/**
			* \brief Found associated material description and load material from it's descriptor.
			* If material has already loaded return pointer to it.
			* Throw exception if material descriptor not found.
			* \ThreadSafe
			* \param p_name
			* \return
			*/
			bc_render_material_ptr load_material_throw(const bcCHAR* p_name);

			bc_render_material_ptr load_material_throw(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name);

			bc_render_material_ptr store_material(const bcCHAR* p_name, bc_render_material_description p_material);

			bc_render_material_ptr store_material(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name, bc_render_material_description p_material);

			void destroy_material(bc_render_material* p_material);

		protected:

		private:
			graphic::bc_texture2d_content_ptr _create_texture_from_color(core::bc_vector4f p_color);

			bc_render_material_ptr _store_material(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name, bc_render_material p_material);

			core::bc_content_stream_manager& m_content_stream_manager;
			bc_render_system& m_render_system;

			material_desc_map m_material_descriptions;
			core_platform::bc_mutex m_materials_mutex;
			material_map m_materials;

			graphic::bc_texture_config m_default_texture_config;
			graphic::bc_texture2d_content_ptr m_default_diffuse_map;
			graphic::bc_texture2d_content_ptr m_default_normal_map;
			graphic::bc_texture2d_content_ptr m_default_specular_map;
			default_diffuse_map m_default_color_textures;
		};
	}
}