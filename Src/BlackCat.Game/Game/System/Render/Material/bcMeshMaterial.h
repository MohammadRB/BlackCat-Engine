// [03/06/2017 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
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
		class bc_mesh_material;
		class bc_material_manager;

		class BC_GAME_DLL _bc_mesh_material_ptr_deleter
		{
		public:
			_bc_mesh_material_ptr_deleter();

			_bc_mesh_material_ptr_deleter(bc_material_manager* p_material_manager);

			void operator()(bc_mesh_material* p_material) const;

			bc_material_manager* m_material_manager;
		};

		struct bc_mesh_material_parameter
		{
			BC_CBUFFER_ALIGN
			core::bc_vector4f m_diffuse;
			BC_CBUFFER_ALIGN
			bcFLOAT m_specular_intensity = 1;
			bcFLOAT m_specular_power = 1;
			bcINT32 m_has_normal_map = false;
		};

		class bc_mesh_material : public core::bc_ref_count
		{
			friend class bc_material_manager;

		public:
			bc_mesh_material();

			bc_mesh_material(bc_mesh_material&&) noexcept;

			~bc_mesh_material();

			bc_mesh_material& operator=(bc_mesh_material&&) noexcept;

			const core::bc_vector4f& get_diffuse() const noexcept;

			bcFLOAT get_specular_intensity() const noexcept;

			bcFLOAT get_specular_power() const noexcept;

			graphic::bc_texture2d get_diffuse_map() const noexcept;

			graphic::bc_texture2d get_normal_map() const noexcept;

			graphic::bc_texture2d get_specular_map() const noexcept;

			graphic::bc_resource_view get_diffuse_map_view() const noexcept;

			graphic::bc_resource_view get_normal_map_view() const noexcept;

			graphic::bc_resource_view get_specular_map_view() const noexcept;

			graphic::bc_buffer get_parameters_cbuffer() const noexcept;

		private:
			bc_mesh_material_parameter m_parameters;

			graphic::bc_texture2d_content_ptr m_diffuse_map;
			graphic::bc_texture2d_content_ptr m_normal_map;
			graphic::bc_texture2d_content_ptr m_specular_map;
			graphic::bc_resource_view_ref m_diffuse_map_view;
			graphic::bc_resource_view_ref m_normal_map_view;
			graphic::bc_resource_view_ref m_specular_map_view;
			graphic::bc_buffer_ref m_parameter_cbuffer;
		};

		using bc_mesh_material_ptr = core::bc_ref_count_ptr< bc_mesh_material, _bc_mesh_material_ptr_deleter >;

		inline bc_mesh_material::bc_mesh_material()
		{
			m_parameters.m_specular_intensity = 1;
			m_parameters.m_specular_power = 1;
		}

		inline bc_mesh_material::bc_mesh_material(bc_mesh_material&&) noexcept = default;

		inline bc_mesh_material::~bc_mesh_material() = default;

		inline bc_mesh_material& bc_mesh_material::operator=(bc_mesh_material&&) noexcept = default;

		inline const core::bc_vector4f& bc_mesh_material::get_diffuse() const noexcept
		{
			return m_parameters.m_diffuse;
		}

		inline bcFLOAT bc_mesh_material::get_specular_intensity() const noexcept
		{
			return m_parameters.m_specular_intensity;
		}

		inline bcFLOAT bc_mesh_material::get_specular_power() const noexcept
		{
			return m_parameters.m_specular_power;
		}

		inline graphic::bc_texture2d bc_mesh_material::get_diffuse_map() const noexcept
		{
			return m_diffuse_map->get_resource();
		}

		inline graphic::bc_texture2d bc_mesh_material::get_normal_map() const noexcept
		{
			return m_normal_map->get_resource();
		}

		inline graphic::bc_texture2d bc_mesh_material::get_specular_map() const noexcept
		{
			return m_specular_map->get_resource();
		}

		inline graphic::bc_resource_view bc_mesh_material::get_diffuse_map_view() const noexcept
		{
			return m_diffuse_map_view.get();
		}

		inline graphic::bc_resource_view bc_mesh_material::get_normal_map_view() const noexcept
		{
			return m_normal_map_view.get();
		}

		inline graphic::bc_resource_view bc_mesh_material::get_specular_map_view() const noexcept
		{
			return m_specular_map_view.get();
		}

		inline graphic::bc_buffer bc_mesh_material::get_parameters_cbuffer() const noexcept
		{
			return m_parameter_cbuffer.get();
		}
	}
}