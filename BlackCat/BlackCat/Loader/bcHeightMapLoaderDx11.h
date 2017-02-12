// [09/13/2016 MRB]

#pragma once

#include "Core/File/bcContentLoader.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class bc_height_map_dx11 : public game::bc_height_map
	{
	public:
		bc_height_map_dx11(const core::bc_vector3f& p_position,
			bcUINT16 p_xz_multiplier,
			bcUINT16 p_y_multiplier,
			bcUINT16 p_distance_detail,
			bcUINT16 p_height_detail,
			graphic::bc_texture2d_content_ptr& p_height_map,
			graphic::bc_resource_view_ptr& p_height_map_view,
			graphic::bc_buffer_ptr& p_parameter_cbuffer,
			game::bc_render_state_ptr& p_render_state,
			graphic::bc_buffer_ptr& p_chunk_info_buffer,
			graphic::bc_resource_view_ptr& p_chunk_info_view,
			graphic::bc_resource_view_ptr& p_chunk_info_unordered_view,
			physics::bc_height_field_ref&& p_px_height_map);

		bc_height_map_dx11(bc_height_map_dx11&& p_other) = default;

		~bc_height_map_dx11() = default;

		bc_height_map_dx11& operator=(bc_height_map_dx11&& p_other) = default;
		
		const graphic::bc_texture2d_ptr& get_height_map() const
		{
			return m_heightmap->get_resource();
		}

		const graphic::bc_resource_view_ptr& get_height_map_view() const
		{
			return m_height_map_view;
		}

		const graphic::bc_buffer_ptr& get_parameter_cbuffer() const
		{
			return m_parameter_cbuffer;
		}

		const graphic::bc_resource_view_ptr& get_chunk_info_view() const
		{
			return m_chunk_info_view;
		}

		const graphic::bc_resource_view_ptr& get_chunk_info_unordered_view() const
		{
			return m_chunk_info_unordered_view;
		}

	protected:

	private:
		bcUINT16 m_distance_detail;
		bcUINT16 m_height_detail;

		graphic::bc_texture2d_content_ptr m_heightmap;
		graphic::bc_resource_view_ptr m_height_map_view;
		graphic::bc_buffer_ptr m_parameter_cbuffer;
		graphic::bc_buffer_ptr m_chunk_info_buffer;
		graphic::bc_resource_view_ptr m_chunk_info_view;
		graphic::bc_resource_view_ptr m_chunk_info_unordered_view;
	};

	class BC_BLACKCAT_DLL bc_height_map_loader_dx11 : public core::bc_base_content_loader
	{
	public:
		void content_offline_processing(core::bc_content_loader_context& p_context) const override;

		void content_processing(core::bc_content_loader_context& p_context) const override;

	protected:

	private:
		constexpr static bcUINT16 s_chund_size = 64;
	};
}
