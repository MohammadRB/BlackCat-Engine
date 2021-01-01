// [04/17/2020 MRB]

#pragma once

#include "Core/Container/bcUnorderedMap.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcIteratorAdapter.h"
#include "Game/bcExport.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/State/bcRenderState.h"

namespace black_cat
{
	namespace game
	{
		struct _bc_render_state_buffer_entry_hash
		{
			using argument_type = bc_render_state_ptr;
			using result_type = _bc_render_state_handle::handle_type;

			result_type operator ()(const argument_type& p_arg) const noexcept
			{
				return p_arg.get_handle().m_handle;
			}
		};

		struct _bc_render_state_buffer_entry_equal
		{
			using result_type = _bc_render_state_handle::handle_type;
			using first_argument_type = bc_render_state_ptr;
			using second_argument_type = bc_render_state_ptr;

			bool operator()(const first_argument_type& p_1, const second_argument_type& p_2) const
			{
				return p_1.get_handle().m_handle == p_2.get_handle().m_handle;
			}
		};

		class BC_GAME_DLL bc_render_state_buffer
		{
		private:
			using instance_map_type = core::bc_unordered_map
			<
				bc_render_state_ptr,
				core::bc_vector_movable< bc_render_instance >,
				_bc_render_state_buffer_entry_hash,
				_bc_render_state_buffer_entry_equal
			>;
			using skinned_instance_map_type = core::bc_unordered_map
			<
				bc_render_state_ptr,
				core::bc_vector_movable< bc_skinned_render_instance >,
				_bc_render_state_buffer_entry_hash,
				_bc_render_state_buffer_entry_equal
			>;
			
		public:
			using instance_iterator = core::bc_const_iterator_adapter< instance_map_type >;
			using skinned_instance_iterator = core::bc_const_iterator_adapter< skinned_instance_map_type >;
			
		public:
			bc_render_state_buffer();

			bc_render_state_buffer(bc_render_state_buffer&&) noexcept;

			~bc_render_state_buffer();

			bc_render_state_buffer& operator=(bc_render_state_buffer&&) noexcept;

			instance_iterator get_instances() const noexcept;

			skinned_instance_iterator get_skinned_instances() const noexcept;
			
			void add_render_instance(bc_render_state_ptr p_render_state, const bc_render_instance& p_instance);

			void add_skinned_render_instance(bc_render_state_ptr p_render_state, bc_skinned_render_instance p_instance);

		private:
			instance_map_type m_render_states;
			skinned_instance_map_type m_skinned_render_states;
		};
	}
}