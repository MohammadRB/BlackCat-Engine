// [30/04/2021 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Game/System/Render/Material/bcMeshMaterial.h"

namespace black_cat::game
{
	struct bc_light_flare
	{
		bc_light_flare(bcFLOAT p_surface_width,
		               bcFLOAT p_surface_height,
		               bcFLOAT p_surface_depth,
		               bcFLOAT p_intensity,
		               bcFLOAT p_size,
		               bc_mesh_material_ptr p_mask_material)
			: m_surface_width(p_surface_width),
			  m_surface_height(p_surface_height),
			  m_surface_depth(p_surface_depth),
			  m_intensity(p_intensity),
			  m_size(p_size),
			  m_mask_material(std::move(p_mask_material)),
			  m_u0(0),
			  m_v0(0),
			  m_u1(1),
			  m_v1(1)
		{
		}
			
		bc_light_flare(bcFLOAT p_surface_width,
		               bcFLOAT p_surface_height,
		               bcFLOAT p_surface_depth,
		               bcFLOAT p_intensity,
		               bcFLOAT p_size,
		               bc_mesh_material_ptr p_mask_material,
		               bcFLOAT p_u0,
		               bcFLOAT p_v0,
		               bcFLOAT p_u1,
		               bcFLOAT p_v1)
			: m_surface_width(p_surface_width),
			  m_surface_height(p_surface_height),
			  m_surface_depth(p_surface_depth),
			  m_intensity(p_intensity),
			  m_size(p_size),
			  m_mask_material(std::move(p_mask_material)),
			  m_u0(p_u0),
			  m_v0(p_v0),
			  m_u1(p_u1),
			  m_v1(p_v1)
		{
			m_u0 = m_u0 / m_mask_material->get_diffuse_map().get_width();
			m_u1 = m_u1 / m_mask_material->get_diffuse_map().get_width();
			m_v0 = m_v0 / m_mask_material->get_diffuse_map().get_height();
			m_v1 = m_v1 / m_mask_material->get_diffuse_map().get_height();
		}

		bcFLOAT get_surface_width() const noexcept
		{
			return m_surface_width;
		}

		bcFLOAT get_surface_height() const noexcept
		{
			return m_surface_height;
		}

		bcFLOAT get_surface_depth() const noexcept
		{
			return m_surface_depth;
		}

		bcFLOAT get_intensity() const noexcept
		{
			return m_intensity;
		}

		bcFLOAT get_size() const noexcept
		{
			return m_size;
		}

		void set_intensity(bcFLOAT p_intensity) noexcept
		{
			m_intensity = p_intensity;
		}
			
		bc_mesh_material* get_mask_material() const noexcept
		{
			return m_mask_material.get();
		}
			
		bc_mesh_material_ptr get_mask_material_ptr() const noexcept
		{
			return m_mask_material;
		}

		graphic::bc_texture2d get_mask_texture() const noexcept
		{
			return m_mask_material->get_diffuse_map();
		}

		bcFLOAT get_u0() const noexcept
		{
			return m_u0;
		}

		bcFLOAT get_v0() const noexcept
		{
			return m_v0;
		}

		bcFLOAT get_u1() const noexcept
		{
			return m_u1;
		}

		bcFLOAT get_v1() const noexcept
		{
			return m_v1;
		}
		
	private:
		bcFLOAT m_surface_width;
		bcFLOAT m_surface_height;
		bcFLOAT m_surface_depth;
		bcFLOAT m_intensity;
		bcFLOAT m_size;
		bc_mesh_material_ptr m_mask_material;
		bcFLOAT m_u0;
		bcFLOAT m_v0;
		bcFLOAT m_u1;
		bcFLOAT m_v1;
	};
}
