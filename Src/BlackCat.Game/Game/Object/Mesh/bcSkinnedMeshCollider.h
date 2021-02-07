// [02/02/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcVector4f.h"
#include "Core/Container/bcVector.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		struct bc_skinned_mesh_collider_vertex
		{
			core::bc_vector3f m_position;
			core::bc_vector4i m_indices;
			core::bc_vector4f m_weights;
		};

		class BC_GAME_DLL bc_skinned_mesh_collider
		{
		public:
			bc_skinned_mesh_collider();

			bc_skinned_mesh_collider(bc_skinned_mesh_collider&&) noexcept;

			~bc_skinned_mesh_collider();

			bc_skinned_mesh_collider& operator=(bc_skinned_mesh_collider&&) noexcept;

			bcSIZE get_vertices_count() const noexcept;
			
			bcSIZE get_faces_count() const noexcept;

			const bc_skinned_mesh_collider_vertex* get_vertices() const noexcept;

			const bcUINT16* get_16bit_indices() const noexcept;
			
			const bcUINT32* get_32bit_indices() const noexcept;
			
			void add_mesh_collider(bc_skinned_mesh_collider_vertex* p_vertices, bcUINT16* p_indices, bcSIZE p_vertices_count, bcSIZE p_faces_count);
			
			void add_mesh_collider(bc_skinned_mesh_collider_vertex* p_vertices, bcUINT32* p_indices, bcSIZE p_vertices_count, bcSIZE p_faces_count);

			void shrink_to_fit();
			
		private:
			bcSIZE m_faces_count;
			core::bc_vector_movable<bc_skinned_mesh_collider_vertex> m_vertices;
			core::bc_vector_movable<bcUINT16> m_16bit_indices;
			core::bc_vector_movable<bcUINT32> m_32bit_indices;
		};

		inline bcSIZE bc_skinned_mesh_collider::get_vertices_count() const noexcept
		{
			return m_vertices.size();
		}

		inline bcSIZE bc_skinned_mesh_collider::get_faces_count() const noexcept
		{
			return m_faces_count;
		}

		inline const bc_skinned_mesh_collider_vertex* bc_skinned_mesh_collider::get_vertices() const noexcept
		{
			return m_vertices.data();
		}

		inline const bcUINT16* bc_skinned_mesh_collider::get_16bit_indices() const noexcept
		{
			return m_16bit_indices.data();
		}

		inline const bcUINT32* bc_skinned_mesh_collider::get_32bit_indices() const noexcept
		{
			return m_32bit_indices.data();
		}
	}	
}