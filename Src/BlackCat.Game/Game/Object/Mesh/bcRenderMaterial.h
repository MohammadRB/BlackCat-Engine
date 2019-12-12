// [03/06/2017 MRB]

#pragma once

#include "Core/Math/bcVector4f.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		struct bc_render_material;
		class bc_material_manager;

		class BC_GAME_DLL _bc_material_ptr_deleter
		{
		public:
			_bc_material_ptr_deleter();

			_bc_material_ptr_deleter(bc_material_manager* p_material_manager);

			void operator()(bc_render_material* p_material) const;

			bc_material_manager* m_material_manager;
		};

		struct bc_render_material_parameter
		{
			BC_CBUFFER_ALIGN
			core::bc_vector4f m_diffuse;
			BC_CBUFFER_ALIGN
			bcFLOAT m_specular_intensity = 1;
			bcFLOAT m_specular_power = 1;
			bcINT32 m_has_normal_map = false;
		};

		class bc_render_material : public core::bc_ref_count
		{
			friend class bc_material_manager;

		public:
			bc_render_material()
			{
				m_parameters.m_specular_intensity = 1;
				m_parameters.m_specular_power = 1;
			}

			bc_render_material(bc_render_material&&) = default;

			~bc_render_material() = default;

			bc_render_material& operator=(bc_render_material&&) = default;

			const core::bc_vector4f& get_diffuse() const
			{
				return m_parameters.m_diffuse;
			}

			bcFLOAT get_specular_intensity() const
			{
				return m_parameters.m_specular_intensity;
			}

			bcFLOAT get_specular_power() const
			{
				return m_parameters.m_specular_power;
			}

			graphic::bc_texture2d get_diffuse_map() const
			{
				return m_diffuse_map->get_resource();
			}

			graphic::bc_texture2d get_normal_map() const
			{
				return m_normal_map->get_resource();
			}

			graphic::bc_texture2d get_specular_map() const
			{
				return m_specular_map->get_resource();
			}

			graphic::bc_resource_view get_diffuse_map_view() const
			{
				return m_diffuse_map_view.get();
			}

			graphic::bc_resource_view get_normal_map_view() const
			{
				return m_normal_map_view.get();
			}

			graphic::bc_resource_view get_specular_map_view() const
			{
				return m_specular_map_view.get();
			}

			graphic::bc_buffer get_parameters_cbuffer() const
			{
				return m_parameter_cbuffer.get();
			}
			
		private:
			bc_render_material_parameter m_parameters;

			graphic::bc_texture2d_content_ptr m_diffuse_map;
			graphic::bc_texture2d_content_ptr m_normal_map;
			graphic::bc_texture2d_content_ptr m_specular_map;
			graphic::bc_resource_view_ptr m_diffuse_map_view;
			graphic::bc_resource_view_ptr m_normal_map_view;
			graphic::bc_resource_view_ptr m_specular_map_view;
			graphic::bc_buffer_ptr m_parameter_cbuffer;
		};

		using bc_render_material_ptr = core::bc_ref_count_ptr< bc_render_material, _bc_material_ptr_deleter >;
	}
}