// [06/07/2016 MRB]

#pragma once

#include "Core/Container/bcIteratorAdapter.h"
#include "Core/Container/bcVector.h"
#include "Core/Math/bcMatrix4f.h"
#include "Game/Object/Mesh/bcMesh.h"

namespace black_cat
{
	namespace game
	{
		class bc_sub_mesh_transform : public core::bc_iterator_adapter<core::bc_vector_movable<core::bc_matrix4f>>
		{
		public:
			bc_sub_mesh_transform();

			explicit bc_sub_mesh_transform(const bc_mesh_node& p_root_node);

			bc_sub_mesh_transform(const bc_sub_mesh_transform&);

			bc_sub_mesh_transform(bc_sub_mesh_transform&&) noexcept;

			~bc_sub_mesh_transform();

			bc_sub_mesh_transform& operator=(const bc_sub_mesh_transform&);

			bc_sub_mesh_transform& operator=(bc_sub_mesh_transform&&) noexcept;

			bc_mesh_node::node_index_t get_root_node_index() const;

			core::bc_vector3f get_node_translation(const bc_mesh_node& p_node) const;

			core::bc_matrix4f& get_node_transform(const bc_mesh_node& p_node);
			
			const core::bc_matrix4f& get_node_transform(const bc_mesh_node& p_node) const;

			void set_node_transformation(const bc_mesh_node& p_node, const core::bc_matrix4f& p_transform);

			void copy_transformations_from(const core::bc_matrix4f* p_transform);

			void copy_transformations_to(core::bc_matrix4f* p_transform) const;

			bcSIZE size() const noexcept;
			
		private:
			bc_mesh_node::node_index_t m_root_node_index;
			core::bc_vector_movable<core::bc_matrix4f> m_transformations;
		};

		inline bc_sub_mesh_transform::bc_sub_mesh_transform()
			: bc_iterator_adapter(m_transformations),
			m_root_node_index(bc_mesh_node::s_invalid_index)
		{
		}

		inline bc_sub_mesh_transform::bc_sub_mesh_transform(const bc_mesh_node& p_root_node)
			: bc_iterator_adapter(m_transformations),
			m_root_node_index(p_root_node.get_transform_index()),
			m_transformations(p_root_node.get_all_children_count() + 1, core::bc_matrix4f::identity())
		{
		}

		inline bc_sub_mesh_transform::bc_sub_mesh_transform(const bc_sub_mesh_transform& p_other)
			: bc_iterator_adapter(m_transformations),
			m_root_node_index(p_other.m_root_node_index),
			m_transformations(p_other.m_transformations)
		{
		}

		inline bc_sub_mesh_transform::bc_sub_mesh_transform(bc_sub_mesh_transform&& p_other) noexcept
			: bc_iterator_adapter(m_transformations),
			m_root_node_index(p_other.m_root_node_index),
			m_transformations(std::move(p_other.m_transformations))
		{
		}

		inline bc_sub_mesh_transform::~bc_sub_mesh_transform() = default;

		inline bc_sub_mesh_transform& bc_sub_mesh_transform::operator=(const bc_sub_mesh_transform& p_other)
		{
			m_root_node_index = p_other.m_root_node_index;
			m_transformations = p_other.m_transformations;
			
			return *this;
		}

		inline bc_sub_mesh_transform& bc_sub_mesh_transform::operator=(bc_sub_mesh_transform&& p_other) noexcept
		{
			m_root_node_index = p_other.m_root_node_index;
			m_transformations = std::move(p_other.m_transformations);
			
			return *this;
		}

		inline bc_mesh_node::node_index_t bc_sub_mesh_transform::get_root_node_index() const
		{
			return m_root_node_index;
		}

		inline core::bc_vector3f bc_sub_mesh_transform::get_node_translation(const bc_mesh_node& p_node) const
		{
			return m_transformations[p_node.get_transform_index() - m_root_node_index].get_translation();
		}

		inline core::bc_matrix4f& bc_sub_mesh_transform::get_node_transform(const bc_mesh_node& p_node)
		{
			BC_ASSERT
			(
				m_root_node_index != bc_mesh_node::s_invalid_index && p_node.get_transform_index() >=
				m_root_node_index
			);

			return m_transformations[p_node.get_transform_index() - m_root_node_index];
		}

		inline const core::bc_matrix4f& bc_sub_mesh_transform::get_node_transform(const bc_mesh_node& p_node) const
		{
			return const_cast<bc_sub_mesh_transform&>(*this).get_node_transform(p_node);
		}

		inline void bc_sub_mesh_transform::set_node_transformation(const bc_mesh_node& p_node, const core::bc_matrix4f& p_transform)
		{
			m_transformations.at(p_node.get_transform_index() - m_root_node_index) = p_transform;
		}

		inline void bc_sub_mesh_transform::copy_transformations_from(const core::bc_matrix4f* p_transform)
		{
			std::memcpy(m_transformations.data(), p_transform, sizeof(decltype(m_transformations)::value_type) * m_transformations.size());
		}

		inline void bc_sub_mesh_transform::copy_transformations_to(core::bc_matrix4f* p_transform) const
		{
			std::memcpy(p_transform, m_transformations.data(), sizeof(decltype(m_transformations)::value_type) * m_transformations.size());
		}

		inline bcSIZE bc_sub_mesh_transform::size() const noexcept
		{
			return m_transformations.size();
		}
	}
}