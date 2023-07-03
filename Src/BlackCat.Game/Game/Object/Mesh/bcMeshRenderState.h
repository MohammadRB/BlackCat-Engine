// [11/02/2021 MRB]

#pragma once

#include "Core/Container/bcIteratorAdapter.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Render/Material/bcMeshMaterial.h"
#include "Game/Object/Mesh/bcMeshNode.h"

namespace black_cat
{
	namespace game
	{
		struct bc_mesh_render_state_entry
		{
			bc_mesh_render_state_entry(bc_mesh_node::node_index_t p_node_index, bc_render_state_ptr p_render_state, bc_mesh_material_ptr p_material)
				: m_node_index(p_node_index),
				m_render_state(std::move(p_render_state)),
				m_material(std::move(p_material))
			{
			}
			
			bc_mesh_node::node_index_t m_node_index;
			bc_render_state_ptr m_render_state;
			bc_mesh_material_ptr m_material;
		};

		class bc_mesh_render_state : public core::bc_const_iterator_adapter< core::bc_vector<bc_mesh_render_state_entry> >
		{
		public:
			using bc_const_iterator_adapter::begin;
			using bc_const_iterator_adapter::cbegin;
			using bc_const_iterator_adapter::end;
			using bc_const_iterator_adapter::cend;
			
		public:
			bc_mesh_render_state() noexcept;
			
			bc_mesh_render_state(core::bc_vector<bc_mesh_render_state_entry> p_render_states, bcUINT32 p_render_states_per_lod) noexcept;

			bc_mesh_render_state(bc_mesh_render_state&&) noexcept;

			~bc_mesh_render_state();

			bc_mesh_render_state& operator=(bc_mesh_render_state&&) noexcept;

			const_iterator begin(bcUINT32 p_lod) const noexcept;

			const_iterator cbegin(bcUINT32 p_lod) const noexcept;

			const_iterator end(bcUINT32 p_lod) const noexcept;

			const_iterator cend(bcUINT32 p_lod) const noexcept;
		
		private:
			container_type m_render_states;
			bcUINT32 m_render_state_per_lod;
		};
		
		inline bc_mesh_render_state::bc_mesh_render_state() noexcept
			: bc_const_iterator_adapter(m_render_states),
			m_render_state_per_lod(0)
		{
		}

		inline bc_mesh_render_state::bc_mesh_render_state(core::bc_vector<bc_mesh_render_state_entry> p_render_states, bcUINT32 p_render_states_per_lod) noexcept
			: bc_const_iterator_adapter(m_render_states),
			m_render_states(std::move(p_render_states)),
			m_render_state_per_lod(p_render_states_per_lod)
		{
		}

		inline bc_mesh_render_state::bc_mesh_render_state(bc_mesh_render_state&& p_other) noexcept
			: bc_const_iterator_adapter(m_render_states),
			m_render_states(std::move(p_other.m_render_states)),
			m_render_state_per_lod(p_other.m_render_state_per_lod)
		{	
		}

		inline bc_mesh_render_state::~bc_mesh_render_state() = default;

		inline bc_mesh_render_state& bc_mesh_render_state::operator=(bc_mesh_render_state&& p_other) noexcept
		{
			m_render_states = std::move(p_other.m_render_states);
			m_render_state_per_lod = p_other.m_render_state_per_lod;
			return *this;
		}

		inline bc_mesh_render_state::const_iterator bc_mesh_render_state::begin(bcUINT32 p_lod) const noexcept
		{
			const auto l_begin = begin();
			return l_begin + m_render_state_per_lod * p_lod;
		}

		inline bc_mesh_render_state::const_iterator bc_mesh_render_state::cbegin(bcUINT32 p_lod) const noexcept
		{
			return begin(p_lod);
		}

		inline bc_mesh_render_state::const_iterator bc_mesh_render_state::end(bcUINT32 p_lod) const noexcept
		{
			const auto l_begin = begin(p_lod);
			return l_begin + m_render_state_per_lod;
		}

		inline bc_mesh_render_state::const_iterator bc_mesh_render_state::cend(bcUINT32 p_lod) const noexcept
		{
			return end(p_lod);
		}
	}	
}