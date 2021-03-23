// [09/13/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Container/bcVector.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "PhysicsImp/Shape/bcHeightField.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Render/Material/bcMeshMaterial.h"
#include "Game/System/Render/Material/bcColliderMaterial.h"

namespace black_cat
{
	namespace game
	{
		struct bc_height_map_material
		{
			bc_mesh_material_ptr m_mesh_material;
			bc_collider_material_description m_collider_material;
		};
		
		class BC_GAME_DLL bc_height_map : public core::bci_content
		{
			BC_CONTENT(hgt_map)

		public:
			bc_height_map(bcUINT16 p_width,
				bcUINT16 p_height,
				bcUINT16 p_xz_multiplier,
				bcFLOAT p_y_multiplier,
				bcFLOAT p_physics_y_scale,
				bc_render_state_ptr p_render_state,
				graphic::bc_buffer_ref p_vertex_buffer,
				graphic::bc_buffer_ref p_index_buffer,
				core::bc_vector<bc_height_map_material> p_materials,
				physics::bc_height_field_ref p_px_height_map,
				void* p_px_height_map_deserialize_buffer);

			bc_height_map(bc_height_map&& p_other) noexcept;

			virtual ~bc_height_map();

			bc_height_map& operator=(bc_height_map&& p_other) noexcept;

			bcUINT16 get_width() const noexcept;

			bcUINT16 get_height() const noexcept;

			bcUINT16 get_xz_multiplier() const noexcept;

			bcFLOAT get_y_multiplier() const noexcept;

			bcFLOAT get_physics_y_scale() const noexcept;

			const bc_render_state* get_render_state() const noexcept;

			bc_render_state_ptr get_render_state_ptr() const noexcept;

			graphic::bc_buffer get_vertex_buffer() const noexcept;

			graphic::bc_buffer get_index_buffer() const noexcept;

			physics::bc_height_field get_px_height_field() const noexcept;

			bcSIZE get_materials_count() const noexcept;

			const bc_height_map_material& get_material(bcSIZE p_index) const noexcept;
			
			const bc_height_map_material* get_materials() const noexcept;

		private:
			bcUINT16 m_width;
			bcUINT16 m_height;
			bcUINT16 m_xz_multiplier;
			bcFLOAT m_y_multiplier;
			bcFLOAT m_physics_y_scale;

			bc_render_state_ptr m_render_state;
			graphic::bc_buffer_ref m_vertex_buffer;
			graphic::bc_buffer_ref m_index_buffer;
			physics::bc_height_field_ref m_px_height_map;
			void* m_px_height_map_deserialize_buffer;
			core::bc_vector<bc_height_map_material> m_materials;
		};

		using bc_height_map_ptr = core::bc_content_ptr< bc_height_map >;

		inline bcUINT16 bc_height_map::get_width() const noexcept
		{
			return m_width;
		}

		inline bcUINT16 bc_height_map::get_height() const noexcept
		{
			return m_height;
		}

		inline bcUINT16 bc_height_map::get_xz_multiplier() const noexcept
		{
			return m_xz_multiplier;
		}

		inline bcFLOAT bc_height_map::get_y_multiplier() const noexcept
		{
			return m_y_multiplier;
		}

		inline bcFLOAT bc_height_map::get_physics_y_scale() const noexcept
		{
			return m_physics_y_scale;
		}

		inline const bc_render_state* bc_height_map::get_render_state() const noexcept
		{
			return m_render_state.get();
		}

		inline bc_render_state_ptr bc_height_map::get_render_state_ptr() const noexcept
		{
			return m_render_state;
		}

		inline graphic::bc_buffer bc_height_map::get_vertex_buffer() const noexcept
		{
			return m_vertex_buffer.get();
		}

		inline graphic::bc_buffer bc_height_map::get_index_buffer() const noexcept
		{
			return m_index_buffer.get();
		}

		inline physics::bc_height_field bc_height_map::get_px_height_field() const noexcept
		{
			return m_px_height_map.get();
		}

		inline bcSIZE bc_height_map::get_materials_count() const noexcept
		{
			return m_materials.size();
		}

		inline const bc_height_map_material& bc_height_map::get_material(bcSIZE p_index) const noexcept
		{
			return m_materials[p_index];
		}
		
		inline const bc_height_map_material* bc_height_map::get_materials() const noexcept
		{
			return m_materials.data();
		}
	}
}