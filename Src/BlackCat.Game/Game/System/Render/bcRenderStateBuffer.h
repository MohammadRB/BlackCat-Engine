// [17/04/2020 MRB]

#pragma once

#include "Core/Container/bcUnorderedMap.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcIteratorAdapter.h"
#include "Game/bcExport.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Render/Decal/bcDecal.h"

namespace black_cat
{
	namespace game
	{
		struct _bc_render_state_buffer_entry_hash
		{
			bcSIZE operator ()(const bc_render_state_ptr& p_arg) const noexcept
			{
				return std::hash<void*>()(p_arg.get());
			}
		};

		struct _bc_render_state_buffer_entry_equal
		{
			bool operator()(const bc_render_state_ptr& p_1, const bc_render_state_ptr& p_2) const
			{
				return p_1.get() == p_2.get();
			}
		};

		struct _bc_render_state_buffer_decal_entry_hash
		{
			bcSIZE operator ()(const bc_decal_ptr& p_arg) const noexcept
			{
				return std::hash<void*>()(p_arg.get());
			}
		};

		struct _bc_render_state_buffer_decal_entry_equal
		{
			bool operator()(const bc_decal_ptr& p_1, const bc_decal_ptr& p_2) const
			{
				return p_1.get() == p_2.get();
			}
		};

		class BC_GAME_DLL bc_render_state_buffer
		{
		private:
			using instance_map_type = core::bc_unordered_map
			<
				bc_render_state_ptr,
				core::bc_vector_movable<bc_render_instance>,
				_bc_render_state_buffer_entry_hash,
				_bc_render_state_buffer_entry_equal
			>;
			using skinned_instance_map_type = core::bc_unordered_map
			<
				bc_render_state_ptr,
				core::bc_vector_movable<bc_skinned_render_instance>,
				_bc_render_state_buffer_entry_hash,
				_bc_render_state_buffer_entry_equal
			>;
			using decal_instance_map_type = core::bc_unordered_map
			<
				bc_decal_ptr,
				core::bc_vector_movable<bc_render_instance>,
				_bc_render_state_buffer_decal_entry_hash,
				_bc_render_state_buffer_decal_entry_equal
			>;
			
		public:
			using instance_iterator = core::bc_const_iterator_adapter<instance_map_type>;
			using skinned_instance_iterator = core::bc_const_iterator_adapter<skinned_instance_map_type>;
			using decal_instance_iterator = core::bc_const_iterator_adapter<decal_instance_map_type>;
			
		public:
			bc_render_state_buffer();

			bc_render_state_buffer(bc_render_state_buffer&&) noexcept;

			~bc_render_state_buffer();

			bc_render_state_buffer& operator=(bc_render_state_buffer&&) noexcept;

			instance_iterator get_instances() const noexcept;

			skinned_instance_iterator get_skinned_instances() const noexcept;

			decal_instance_iterator get_decal_instances() const noexcept;
			
			void add_render_instance(bc_render_state_ptr p_render_state, const bc_render_instance& p_instance);

			void add_skinned_render_instance(bc_render_state_ptr p_render_state, bc_skinned_render_instance p_instance);

			void add_decal_instance(bc_decal_ptr p_decal, const bc_render_instance& p_instance);
			
		private:
			instance_map_type m_render_states;
			skinned_instance_map_type m_skinned_render_states;
			decal_instance_map_type m_decals;
		};

		inline bc_render_state_buffer::instance_iterator bc_render_state_buffer::get_instances() const noexcept
		{
			return instance_iterator(m_render_states);
		}

		inline bc_render_state_buffer::skinned_instance_iterator bc_render_state_buffer::get_skinned_instances() const noexcept
		{
			return skinned_instance_iterator(m_skinned_render_states);
		}

		inline bc_render_state_buffer::decal_instance_iterator bc_render_state_buffer::get_decal_instances() const noexcept
		{
			return decal_instance_iterator(m_decals);
		}

		inline void bc_render_state_buffer::add_render_instance(bc_render_state_ptr p_render_state, const bc_render_instance& p_instance)
		{
			auto& l_entry = m_render_states[std::move(p_render_state)];
			l_entry.push_back(p_instance);
		}

		inline void bc_render_state_buffer::add_skinned_render_instance(bc_render_state_ptr p_render_state, bc_skinned_render_instance p_instance)
		{
			auto& l_entry = m_skinned_render_states[std::move(p_render_state)];
			l_entry.push_back(std::move(p_instance));
		}

		inline void bc_render_state_buffer::add_decal_instance(bc_decal_ptr p_decal, const bc_render_instance& p_instance)
		{
			auto& l_entry = m_decals[std::move(p_decal)];
			l_entry.push_back(p_instance);
		}
	}
}