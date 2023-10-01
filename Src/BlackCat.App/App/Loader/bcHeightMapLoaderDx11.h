// [13/09/2016 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "PhysicsImp/Shape/bcHeightField.h"
#include "Game/System/Render/Material/bcMeshMaterial.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "App/bcExport.h"

namespace black_cat::game
{
	class bc_render_system;
	class bc_physics_system;
	class bc_material_manager;
}

namespace black_cat
{
	struct _bc_parameter_buffer;
	struct _bc_material_properties;

	class bc_height_map_dx11 : public game::bc_height_map
	{
	public:
		bc_height_map_dx11(bcUINT32 p_xz_multiplier,
			bcFLOAT p_y_multiplier,
			bcFLOAT p_physics_y_scale,
			bcUINT32 p_distance_detail,
			bcUINT32 p_height_detail,
			game::bc_render_state_ptr p_render_state,
			graphic::bc_buffer_ref p_vertex_buffer,
			graphic::bc_buffer_ref p_index_buffer,
			graphic::bc_texture2d_ref p_height_map,
			graphic::bc_resource_view_ref p_height_map_view,
			graphic::bc_texture2d_content_ptr p_texture_map,
			graphic::bc_resource_view_ref p_texture_map_view,
			graphic::bc_buffer_ref p_parameter_cbuffer,
			graphic::bc_buffer_ref p_chunk_info_buffer,
			graphic::bc_resource_view_ref p_chunk_info_view,
			graphic::bc_resource_view_ref p_chunk_info_unordered_view,
			graphic::bc_buffer_ref p_material_properties_buffer,
			core::bc_vector<game::bc_height_map_material> p_materials,
			physics::bc_height_field_ref p_px_height_map,
			void* p_px_height_map_deserialize_buffer);

		bc_height_map_dx11(bc_height_map_dx11&&) = default;

		~bc_height_map_dx11() override = default;

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

	private:
		bcUINT32 m_distance_detail;
		bcUINT32 m_height_detail;

		graphic::bc_texture2d_ref m_height_map;
		graphic::bc_resource_view_ref m_height_map_view;
		graphic::bc_texture2d_content_ptr m_texture_map;
		graphic::bc_resource_view_ref m_texture_map_view;
		graphic::bc_buffer_ref m_parameter_cbuffer;
		graphic::bc_buffer_ref m_chunk_info_buffer;
		graphic::bc_resource_view_ref m_chunk_info_view;
		graphic::bc_resource_view_ref m_chunk_info_unordered_view;
		graphic::bc_buffer_ref m_material_properties_buffer;
	};

	class BC_DLL bc_height_map_loader_dx11 : public core::bc_base_content_loader
	{
	public:
		bool support_offline_processing() const override;

		void content_offline_processing(core::bc_content_loading_context& p_context) const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;
		
		void content_processing(core::bc_content_saving_context& p_context) const override;

	protected:
		virtual std::pair<graphic::bc_texture_config, graphic::bc_resource_view_config> get_height_map_texture_config(bcUINT32 p_width, bcUINT32 p_height) const;

		virtual std::pair<graphic::bc_texture_config, graphic::bc_resource_view_config> get_texture_map_texture_config(bcUINT32 p_width, bcUINT32 p_height) const;

	private:
		core::bc_estring_frame _get_height_map_texture_name(core::bc_estring_view p_file_path) const;

		core::bc_estring_frame _get_texture_map_texture_name(core::bc_estring_view p_file_path) const;

		void _create_texture_map(core::bc_content_loading_context& p_context, 
			game::bc_render_system& p_render_system, 
			bcUINT32 p_width, 
			bcUINT32 p_height) const;

		physics::bc_height_field_ref _create_px_height_field(game::bc_physics_system& p_physics_system,
			const bcINT16* p_samples, 
			bcUINT32 p_sample_count, 
			bcUINT32 p_width, 
			bcUINT32 p_height) const;

		void _copy_px_height_field(const physics::bc_height_field& p_px_height_field, 
			bcFLOAT p_px_y_scale, 
			bcFLOAT* p_dest) const;

		core::bc_vector<game::bc_height_map_material> _read_materials(game::bc_material_manager& p_material_manager,
			_bc_material_properties& p_material_properties,
			const core::bc_vector<core::bc_any>* p_material_names_param) const;

		void _create_vertices_indices(bcUINT32 p_width,
			bcUINT32 p_height,
			bcINT32 p_xz_multiplier,
			core::bc_vector_frame<game::bc_vertex_pos_tex>& p_vertices,
			core::bc_vector_frame<bcUINT16>& p_indices,
			core::bc_vector_frame<std::array<bcINT32, 3>>& p_chunk_infos) const;

		bc_height_map_dx11 _create_height_map(game::bc_render_system& p_render_system,
			graphic::bc_texture2d_ref p_height_map_texture,
			graphic::bc_texture2d_content_ptr p_texture_map_texture,
			graphic::bc_resource_view_config& p_height_map_view_config,
			graphic::bc_resource_view_config& p_texture_map_view_config,
			core::bc_vector_frame<game::bc_vertex_pos_tex>& p_vertices,
			core::bc_vector_frame<bcUINT16>& p_indices,
			core::bc_vector_frame<std::array<bcINT32, 3>>& p_chunk_infos,
			core::bc_vector<game::bc_height_map_material>& p_materials,
			_bc_parameter_buffer& p_parameters,
			_bc_material_properties& p_material_properties,
			physics::bc_height_field_ref p_px_height_map,
			physics::bc_serialize_buffer p_px_deserialized_buffer,
			bcFLOAT p_px_y_scale,
			bcINT32 p_distance_detail,
			bcINT32 p_height_detail) const;

		constexpr static bcUINT32 s_chunk_size = 64;
	};
}