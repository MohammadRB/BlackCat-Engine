// [09/13/2016 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/Material/bcMeshMaterial.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class bc_height_map_dx11 : public game::bc_height_map
	{
	public:
		bc_height_map_dx11(bcUINT16 p_xz_multiplier,
			bcFLOAT p_y_multiplier,
			bcFLOAT p_physics_y_scale,
			bcUINT16 p_distance_detail,
			bcUINT16 p_height_detail,
			game::bc_render_state_ptr p_render_state,
			graphic::bc_buffer_ptr p_vertex_buffer,
			graphic::bc_buffer_ptr p_index_buffer,
			graphic::bc_texture2d_ptr p_height_map,
			graphic::bc_resource_view_ptr p_height_map_view,
			graphic::bc_texture2d_content_ptr p_texture_map,
			graphic::bc_resource_view_ptr p_texture_map_view,
			graphic::bc_buffer_ptr p_parameter_cbuffer,
			graphic::bc_buffer_ptr p_chunk_info_buffer,
			graphic::bc_resource_view_ptr p_chunk_info_view,
			graphic::bc_resource_view_ptr p_chunk_info_unordered_view,
			graphic::bc_buffer_ptr p_material_properties_buffer,
			core::bc_vector<game::bc_mesh_material_ptr> p_materials,
			physics::bc_height_field_ref p_px_height_map,
			void* p_px_height_map_deserialize_buffer);

		bc_height_map_dx11(bc_height_map_dx11&&) = default;

		~bc_height_map_dx11() = default;

		bc_height_map_dx11& operator=(bc_height_map_dx11&&) = default;
		
		graphic::bc_texture2d get_height_map() const
		{
			return m_height_map.get();
		}

		graphic::bc_resource_view get_height_map_view() const
		{
			return m_height_map_view.get();
		}

		graphic::bc_texture2d get_texture_map() const
		{
			return m_texture_map->get_resource();
		}

		graphic::bc_texture2d_content& get_texture_map_content() const
		{
			return *m_texture_map;
		}

		graphic::bc_resource_view get_texture_map_view() const
		{
			return m_texture_map_view.get();
		}

		graphic::bc_buffer get_parameter_cbuffer() const
		{
			return m_parameter_cbuffer.get();
		}

		graphic::bc_resource_view get_chunk_info_view() const
		{
			return m_chunk_info_view.get();
		}

		graphic::bc_resource_view get_chunk_info_unordered_view() const
		{
			return m_chunk_info_unordered_view.get();
		}

		const core::bc_vector<game::bc_mesh_material_ptr>& get_materials() const
		{
			return m_materials;
		}

	protected:

	private:
		bcUINT16 m_distance_detail;
		bcUINT16 m_height_detail;

		graphic::bc_texture2d_ptr m_height_map;
		graphic::bc_resource_view_ptr m_height_map_view;
		graphic::bc_texture2d_content_ptr m_texture_map;
		graphic::bc_resource_view_ptr m_texture_map_view;
		graphic::bc_buffer_ptr m_parameter_cbuffer;
		graphic::bc_buffer_ptr m_chunk_info_buffer;
		graphic::bc_resource_view_ptr m_chunk_info_view;
		graphic::bc_resource_view_ptr m_chunk_info_unordered_view;
		graphic::bc_buffer_ptr m_material_properties_buffer;
		core::bc_vector<game::bc_mesh_material_ptr> m_materials;
	};

	class BC_DLL bc_height_map_loader_dx11 : public core::bc_base_content_loader
	{
	public:
		bool support_offline_processing() const override;

		void content_offline_processing(core::bc_content_loading_context& p_context) const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;
		
		void content_processing(core::bc_content_saving_context& p_context) const override;

	protected:
		virtual std::pair< graphic::bc_texture_config, graphic::bc_resource_view_config > get_height_map_texture_config(bcUINT32 p_width, bcUINT32 p_height) const;

		virtual std::pair< graphic::bc_texture_config, graphic::bc_resource_view_config > get_texture_map_texture_config(bcUINT32 p_width, bcUINT32 p_height) const;

	private:
		constexpr static bcUINT16 s_chunk_size = 64;
	};
}