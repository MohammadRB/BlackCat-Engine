// [06/07/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/bcException.h"
#include "Game/Object/Mesh/bcMesh.h"

namespace black_cat
{
	namespace game
	{
		class bc_mesh_part_transformation
		{
		public:
			friend class bc_mesh_part;

		public:
			bc_mesh_part_transformation()
				: m_root_node_index(-1)
			{
			}

			bc_mesh_part_transformation(bc_mesh_node::node_indexing p_root_node_index, core::bc_vector<core::bc_matrix4f>&& p_transformations)
				: m_root_node_index(p_root_node_index),
				m_transformations(std::move(p_transformations))
			{
			}

			bc_mesh_part_transformation(const bc_mesh_part_transformation&) = default;

			bc_mesh_part_transformation(bc_mesh_part_transformation&&) = default;

			~bc_mesh_part_transformation() = default;

			bc_mesh_part_transformation& operator=(const bc_mesh_part_transformation&) = default;

			bc_mesh_part_transformation& operator=(bc_mesh_part_transformation&&) = default;

		protected:

		private:
			bc_mesh_node::node_indexing m_root_node_index;
			core::bc_vector<core::bc_matrix4f> m_transformations;
		};

		class BC_GAME_DLL bc_mesh_part
		{
		public:
			bc_mesh_part();

			bc_mesh_part(bc_mesh_ptr p_mesh);

			bc_mesh_part(bc_mesh_ptr p_mesh, const bcCHAR* p_node);

			bc_mesh_part(const bc_mesh_part&);

			~bc_mesh_part();

			bc_mesh_part& operator=(const bc_mesh_part&);

			bc_mesh_ptr get_mesh() const;

			const bc_mesh_node* get_node() const;

			const bc_mesh_node* get_parent_node(const bc_mesh_node* p_node) const;

			const core::bc_vector< bc_mesh_node* >& get_node_childs(const bc_mesh_node* p_node) const;

			const core::bc_matrix4f* get_node_transformation(const bc_mesh_node* p_node) const;

			const bc_mesh_material* get_node_mesh_material(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const;

			const bc_render_state* get_node_mesh_render_state(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const;

			const core::bc_matrix4f* get_node_absolute_transformation(const bc_mesh_part_transformation& p_transformations, const bc_mesh_node* p_node) const;

			bc_mesh_part_transformation calculate_absolute_transformations(const core::bc_matrix4f& p_world) const;

		protected:

		private:
			void _calculate_absolute_transformations(const core::bc_matrix4f& p_parent_transformation,
				bc_mesh_node** p_begin, 
				bc_mesh_node** p_end,
				core::bc_vector<core::bc_matrix4f>& p_result) const;

			bc_mesh_ptr m_mesh;
			const bc_mesh_node* m_node;
		};

		inline bc_mesh_part::bc_mesh_part() 
			: m_mesh(nullptr),
			m_node(nullptr)
		{
		}

		inline bc_mesh_part::bc_mesh_part(bc_mesh_ptr p_mesh)
		{
			if(p_mesh == nullptr)
			{
				throw bc_invalid_argument_exception("null argument");
			}

			m_mesh = p_mesh;
			m_node = p_mesh->get_root();
		}

		inline bc_mesh_part::bc_mesh_part(bc_mesh_ptr p_mesh, const bcCHAR* p_node)
		{
			if (p_mesh == nullptr)
			{
				throw bc_invalid_argument_exception("null argument");
			}

			m_mesh = p_mesh;
			m_node = p_mesh->find_node(p_node);

			if (m_node == nullptr)
			{
				throw bc_invalid_argument_exception("null argument");
			}
		}

		inline bc_mesh_part::bc_mesh_part(const bc_mesh_part& p_other)
			: m_mesh(p_other.m_mesh),
			m_node(p_other.m_node)
		{
		}

		inline bc_mesh_part::~bc_mesh_part()
		{
		}

		inline bc_mesh_part& bc_mesh_part::operator=(const bc_mesh_part& p_other)
		{
			m_mesh = p_other.m_mesh;
			m_node = p_other.m_node;

			return *this;
		}

		inline bc_mesh_ptr bc_mesh_part::get_mesh() const
		{
			return m_mesh;
		}

		inline const bc_mesh_node* bc_mesh_part::get_node() const
		{
			return m_node;
		}

		inline const bc_mesh_node* bc_mesh_part::get_parent_node(const bc_mesh_node* p_node) const
		{
			return m_mesh->get_node_parent(p_node);
		}
		
		inline const core::bc_vector<bc_mesh_node*>& bc_mesh_part::get_node_childs(const bc_mesh_node* p_node) const
		{
			return m_mesh->get_node_childs(p_node);
		}

		inline const core::bc_matrix4f* bc_mesh_part::get_node_transformation(const bc_mesh_node* p_node) const
		{
			return m_mesh->get_node_transformation(p_node);
		}

		inline const bc_mesh_material* bc_mesh_part::get_node_mesh_material(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_material(p_node, p_mesh_index);
		}

		inline const bc_render_state* bc_mesh_part::get_node_mesh_render_state(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_render_state(p_node, p_mesh_index);
		}

		inline const core::bc_matrix4f* bc_mesh_part::get_node_absolute_transformation(const bc_mesh_part_transformation& p_transformations, const bc_mesh_node* p_node) const
		{
			auto l_root_node_index = p_transformations.m_root_node_index;
			auto l_node_index = p_node->get_transformation_index();

			bcAssert(l_root_node_index != bc_mesh_node::node_indexing(-1) && l_node_index >= l_root_node_index);

			return &p_transformations.m_transformations[l_node_index - l_root_node_index];
		}

		inline bc_mesh_part_transformation bc_mesh_part::calculate_absolute_transformations(const core::bc_matrix4f& p_world) const
		{
			core::bc_vector<core::bc_matrix4f> p_result;
			bc_mesh_node* l_node = const_cast< bc_mesh_node* >(m_node);
			bc_mesh_node** l_begin = &l_node;

			_calculate_absolute_transformations(p_world, l_begin, l_begin + 1, p_result);

			return bc_mesh_part_transformation(m_node->get_transformation_index(), std::move(p_result));
		}

		inline void bc_mesh_part::_calculate_absolute_transformations(const core::bc_matrix4f& p_parent_transformation,
			bc_mesh_node** p_begin,
			bc_mesh_node** p_end,
			core::bc_vector<core::bc_matrix4f>& p_result) const
		{
			for(; p_begin != p_end; ++p_begin)
			{
				const bc_mesh_node* l_node = *p_begin;

				auto l_transformation = *m_mesh->get_node_transformation(l_node) * p_parent_transformation;
				p_result.push_back(l_transformation);

				const auto& l_node_childs = m_mesh->get_node_childs(l_node);

				if(l_node_childs.size() > 0)
				{
					bc_mesh_node** l_begin = &*m_mesh->get_node_childs(l_node).begin();
					bc_mesh_node** l_end = &*m_mesh->get_node_childs(l_node).end();

					_calculate_absolute_transformations
					(
						l_transformation,
						l_begin,
						l_end,
						p_result
					);
				}
			}
		}
	}
}