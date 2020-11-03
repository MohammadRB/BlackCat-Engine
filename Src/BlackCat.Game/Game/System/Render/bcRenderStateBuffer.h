// [04/17/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Container/bcVector.h"
#include "Game/bcExport.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/State/bcRenderState.h"

namespace black_cat
{
	namespace game
	{
		class bc_icamera;
		class bc_render_thread;
		
		struct _bc_render_state_buffer_entry_hash
		{
		public:
			using argument_type = bc_render_state_ptr;
			using result_type = _bc_render_state_handle::handle_type;

			result_type operator ()(const argument_type& p_arg) const noexcept
			{
				return p_arg.get_handle().m_handle;
			}
		};
		
		class BC_GAME_DLL bc_render_state_buffer
		{
		private:
			using map_type = core::bc_unordered_map< bc_render_state_ptr, core::bc_vector<bc_render_instance>, _bc_render_state_buffer_entry_hash >;
			
		public:
			using value_type = map_type::value_type;
			using pointer = map_type::pointer;
			using const_pointer = map_type::const_pointer;
			using reference = map_type::reference;
			using const_reference = map_type::const_reference;
			using difference_type = map_type::difference_type;
			using size_type = map_type::size_type;
			using iterator = map_type::iterator;
			using const_iterator = map_type::const_iterator;
			
		public:
			bc_render_state_buffer();

			bc_render_state_buffer(bc_render_state_buffer&&) noexcept;

			~bc_render_state_buffer();

			bc_render_state_buffer& operator=(bc_render_state_buffer&&) noexcept;

			iterator begin();

			const_iterator begin() const;

			const_iterator cbegin() const;

			iterator end();

			const_iterator end() const;

			const_iterator cend() const;

			void add_render_instance(bc_render_state_ptr p_render_state, const bc_render_instance& p_instance);

		private:
			map_type m_render_states;
		};
	}
}