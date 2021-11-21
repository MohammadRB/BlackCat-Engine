// [06/07/2016 MRB]

#pragma once

#include "Core/Container/bcIteratorAdapter.h"
#include "Core/Container/bcVector.h"
#include "Core/Math/bcMatrix4f.h"
#include "PhysicsImp/Fundation/bcTransform.h"
#include "Game/Object/Mesh/bcMeshNode.h"

namespace black_cat
{
	namespace game
	{
		template<typename T>
		class bc_sub_mesh_transform : public core::bc_iterator_adapter<core::bc_vector<T, core::bc_aligned_allocator_movable<T, 16>>>
		{
		private:
			using iterator_adapter_t = core::bc_iterator_adapter<core::bc_vector<T, core::bc_aligned_allocator_movable<T, 16>>>;
			using container_type = typename iterator_adapter_t::container_type;
			
		public:
			using transform_t = T;
		
		public:
			bc_sub_mesh_transform();

			explicit bc_sub_mesh_transform(const bc_mesh_node& p_root_node);

			bc_sub_mesh_transform(const bc_sub_mesh_transform&);

			bc_sub_mesh_transform(bc_sub_mesh_transform&&) noexcept;

			~bc_sub_mesh_transform();

			bc_sub_mesh_transform& operator=(const bc_sub_mesh_transform&);

			bc_sub_mesh_transform& operator=(bc_sub_mesh_transform&&) noexcept;

			bc_mesh_node::node_index_t get_root_node_index() const noexcept;

			transform_t& operator[](typename container_type::size_type p_index);

			const transform_t& operator[](typename container_type::size_type p_index) const;

			transform_t& at(typename container_type::size_type p_index);

			const transform_t& at(typename container_type::size_type p_index) const;
			
			transform_t& get_node_transform(const bc_mesh_node& p_node) noexcept;
			
			const transform_t& get_node_transform(const bc_mesh_node& p_node) const noexcept;

			void set_node_transform(const bc_mesh_node& p_node, const transform_t& p_transform) noexcept;

			void copy_transforms_from(const transform_t* p_transform) noexcept;

			void copy_transforms_to(transform_t* p_transform) const noexcept;
			
		private:
			bc_mesh_node::node_index_t m_root_node_index;
			container_type m_transformations;
		};

		using bc_sub_mesh_mat4_transform = bc_sub_mesh_transform<core::bc_matrix4f>;
		using bc_sub_mesh_px_transform = bc_sub_mesh_transform<physics::bc_transform>;

		template<typename T>
		bc_sub_mesh_transform<T>::bc_sub_mesh_transform()
			: iterator_adapter_t(m_transformations),
			m_root_node_index(bc_mesh_node::s_invalid_index)
		{
		}

		template<typename T>
		bc_sub_mesh_transform<T>::bc_sub_mesh_transform(const bc_mesh_node& p_root_node)
			: iterator_adapter_t(m_transformations),
			m_root_node_index(p_root_node.get_index()),
			m_transformations(p_root_node.get_all_children_count() + 1)
		{
		}

		template<typename T>
		bc_sub_mesh_transform<T>::bc_sub_mesh_transform(const bc_sub_mesh_transform& p_other)
			: iterator_adapter_t(m_transformations),
			m_root_node_index(p_other.m_root_node_index),
			m_transformations(p_other.m_transformations)
		{
		}

		template<typename T>
		bc_sub_mesh_transform<T>::bc_sub_mesh_transform(bc_sub_mesh_transform&& p_other) noexcept
			: iterator_adapter_t(m_transformations),
			m_root_node_index(p_other.m_root_node_index),
			m_transformations(std::move(p_other.m_transformations))
		{
		}

		template<typename T>
		bc_sub_mesh_transform<T>::~bc_sub_mesh_transform() = default;

		template<typename T>
		bc_sub_mesh_transform<T>& bc_sub_mesh_transform<T>::operator=(const bc_sub_mesh_transform& p_other)
		{
			m_root_node_index = p_other.m_root_node_index;
			m_transformations = p_other.m_transformations;
			
			return *this;
		}

		template<typename T>
		bc_sub_mesh_transform<T>& bc_sub_mesh_transform<T>::operator=(bc_sub_mesh_transform&& p_other) noexcept
		{
			m_root_node_index = p_other.m_root_node_index;
			m_transformations = std::move(p_other.m_transformations);
			
			return *this;
		}

		template<typename T>
		bc_mesh_node::node_index_t bc_sub_mesh_transform<T>::get_root_node_index() const noexcept
		{
			return m_root_node_index;
		}

		template<typename T>
		typename bc_sub_mesh_transform<T>::transform_t& bc_sub_mesh_transform<T>::operator[](typename container_type::size_type p_index)
		{
			return m_transformations[p_index];
		}

		template<typename T>
		const typename bc_sub_mesh_transform<T>::transform_t& bc_sub_mesh_transform<T>::operator[](typename container_type::size_type p_index) const
		{
			return m_transformations[p_index];
		}

		template<typename T>
		typename bc_sub_mesh_transform<T>::transform_t& bc_sub_mesh_transform<T>::at(typename container_type::size_type p_index)
		{
			return m_transformations.at(p_index);
		}

		template<typename T>
		const typename bc_sub_mesh_transform<T>::transform_t& bc_sub_mesh_transform<T>::at(typename container_type::size_type p_index) const
		{
			return m_transformations.at(p_index);
		}

		template<typename T>
		typename bc_sub_mesh_transform<T>::transform_t& bc_sub_mesh_transform<T>::get_node_transform(const bc_mesh_node& p_node) noexcept
		{
			BC_ASSERT
			(
				m_root_node_index != bc_mesh_node::s_invalid_index && 
				p_node.get_index()>= m_root_node_index
			);

			return m_transformations[p_node.get_index() - m_root_node_index];
		}

		template<typename T>
		const typename bc_sub_mesh_transform<T>::transform_t& bc_sub_mesh_transform<T>::get_node_transform(const bc_mesh_node& p_node) const noexcept
		{
			return const_cast<bc_sub_mesh_transform&>(*this).get_node_transform(p_node);
		}

		template<typename T>
		void bc_sub_mesh_transform<T>::set_node_transform(const bc_mesh_node& p_node, const transform_t& p_transform) noexcept
		{
			BC_ASSERT
			(
				m_root_node_index != bc_mesh_node::s_invalid_index &&
				p_node.get_index()>= m_root_node_index
			);
			
			m_transformations.at(p_node.get_index() - m_root_node_index) = p_transform;
		}

		template<typename T>
		void bc_sub_mesh_transform<T>::copy_transforms_from(const transform_t* p_transform) noexcept
		{
			std::memcpy(m_transformations.data(), p_transform, sizeof(decltype(m_transformations)::value_type) * m_transformations.size());
		}

		template<typename T>
		void bc_sub_mesh_transform<T>::copy_transforms_to(transform_t* p_transform) const noexcept
		{
			std::memcpy(p_transform, m_transformations.data(), sizeof(decltype(m_transformations)::value_type) * m_transformations.size());
		}
	}
}