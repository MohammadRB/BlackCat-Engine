// [06/07/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Math/bcMatrix4f.h"
#include "Game/Object/Mesh/bcMesh.h"

namespace black_cat
{
	namespace game
	{
		class bc_sub_mesh_transformation
		{
		public:
			bc_sub_mesh_transformation();

			explicit bc_sub_mesh_transformation(const bc_mesh_node& p_root_node);

			bc_sub_mesh_transformation(const bc_sub_mesh_transformation&);

			bc_sub_mesh_transformation(bc_sub_mesh_transformation&&) noexcept;

			~bc_sub_mesh_transformation();

			bc_sub_mesh_transformation& operator=(const bc_sub_mesh_transformation&);

			bc_sub_mesh_transformation& operator=(bc_sub_mesh_transformation&&) noexcept;

			bc_mesh_node::node_index get_root_node_index() const;

			core::bc_vector3f get_node_translation(const bc_mesh_node& p_node) const;

			const core::bc_matrix4f& get_node_transform(const bc_mesh_node& p_node) const;

			void set_node_transformation(const bc_mesh_node& p_node, const core::bc_matrix4f& p_transform);

			void copy_transformations_from(const core::bc_matrix4f* p_transform);

			void copy_transformations_to(core::bc_matrix4f* p_transform) const;

			bcSIZE size() const noexcept;
			
		private:
			bc_mesh_node::node_index m_root_node_index;
			core::bc_vector_movable<core::bc_matrix4f> m_transformations;
		};

		inline bc_sub_mesh_transformation::bc_sub_mesh_transformation(): m_root_node_index
			(bc_mesh_node::s_invalid_index)
		{
		}

		inline bc_sub_mesh_transformation::bc_sub_mesh_transformation(const bc_mesh_node& p_root_node):
			m_root_node_index(p_root_node.get_transformation_index()),
			m_transformations(p_root_node.get_all_children_count() + 1, core::bc_matrix4f::identity())
		{
		}

		inline bc_sub_mesh_transformation::bc_sub_mesh_transformation(const bc_sub_mesh_transformation&) = default;

		inline bc_sub_mesh_transformation::bc_sub_mesh_transformation(bc_sub_mesh_transformation&&) noexcept = default;

		inline bc_sub_mesh_transformation::~bc_sub_mesh_transformation() = default;

		inline bc_sub_mesh_transformation& bc_sub_mesh_transformation::operator=(const bc_sub_mesh_transformation&) = default;

		inline bc_sub_mesh_transformation& bc_sub_mesh_transformation::operator=(bc_sub_mesh_transformation&&) noexcept = default;

		inline bc_mesh_node::node_index bc_sub_mesh_transformation::get_root_node_index() const
		{
			return m_root_node_index;
		}

		inline core::bc_vector3f bc_sub_mesh_transformation::get_node_translation(const bc_mesh_node& p_node) const
		{
			return m_transformations[p_node.get_transformation_index() - m_root_node_index].get_translation();
		}

		inline const core::bc_matrix4f& bc_sub_mesh_transformation::get_node_transform(const bc_mesh_node& p_node) const
		{
			bcAssert
			(
				m_root_node_index != bc_mesh_node::s_invalid_index && p_node.get_transformation_index() >=
				m_root_node_index
			);

			return m_transformations[p_node.get_transformation_index() - m_root_node_index];
		}

		inline void bc_sub_mesh_transformation::set_node_transformation(const bc_mesh_node& p_node, const core::bc_matrix4f& p_transform)
		{
			m_transformations.at(p_node.get_transformation_index() - m_root_node_index) = p_transform;
		}

		inline void bc_sub_mesh_transformation::copy_transformations_from(const core::bc_matrix4f* p_transform)
		{
			std::memcpy(m_transformations.data(), p_transform, sizeof(decltype(m_transformations)::value_type) * m_transformations.size());
		}

		inline void bc_sub_mesh_transformation::copy_transformations_to(core::bc_matrix4f* p_transform) const
		{
			std::memcpy(p_transform, m_transformations.data(), sizeof(decltype(m_transformations)::value_type) * m_transformations.size());
		}

		inline bcSIZE bc_sub_mesh_transformation::size() const noexcept
		{
			return m_transformations.size();
		}
	}
}