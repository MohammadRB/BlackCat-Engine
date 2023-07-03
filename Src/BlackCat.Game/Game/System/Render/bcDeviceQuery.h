// [22/04/2022 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "GraphicImp/bcDeviceRef.h"
#include "Game/System/Render/bcRenderThread.h"

namespace black_cat
{
	namespace game
	{
		template<class TQuery, bcUINT32 TBufferSize = 2>
		class bc_device_query
		{
			enum class query_state
			{
				start_write, end_write, read
			};

		public:
			bc_device_query() noexcept;

			explicit bc_device_query(core::bc_array<graphic::bc_device_ref<TQuery>, TBufferSize> p_queries) noexcept;

			bc_device_query(bc_device_query&&) noexcept = default;

			~bc_device_query() = default;

			bc_device_query& operator=(bc_device_query&&) noexcept = default;

			bcUINT64 get_last_data() const noexcept;

			void start(bc_render_thread& p_render_thread) noexcept;

			void end(bc_render_thread& p_render_thread) noexcept;

			std::pair<bool, bcUINT64> get_data(bc_render_thread& p_render_thread);

		private:
			bcUINT64 m_last_data;
			bcUINT16 m_read_index;
			bcUINT16 m_write_index;
			core::bc_array<std::pair<query_state, graphic::bc_device_ref<TQuery>>, TBufferSize> m_queries;
		};

		template<class TQuery, bcUINT32 TBufferSize>
		bc_device_query<TQuery, TBufferSize>::bc_device_query() noexcept
			: m_last_data(0),
			m_read_index(0),
			m_write_index(0)
		{
		}

		template<class TQuery, bcUINT32 TBufferSize>
		bc_device_query<TQuery, TBufferSize>::bc_device_query(core::bc_array<graphic::bc_device_ref<TQuery>, TBufferSize> p_queries) noexcept
			: bc_device_query()
		{
			for (auto l_ite = 0U; l_ite < TBufferSize; ++l_ite)
			{
				m_queries[l_ite].first = query_state::read;
				m_queries[l_ite].second = std::move(p_queries[l_ite]);
			}
		}

		template<class TQuery, bcUINT32 TBufferSize>
		bcUINT64 bc_device_query<TQuery, TBufferSize>::get_last_data() const noexcept
		{
			return m_last_data;
		}

		template<class TQuery, bcUINT32 TBufferSize>
		void bc_device_query<TQuery, TBufferSize>::start(bc_render_thread& p_render_thread) noexcept
		{
			auto& l_query = m_queries[m_write_index];

			// Do not write onto previous queries until their result are fetched
			if(l_query.first != query_state::read)
			{
				return;
			}

			p_render_thread.start_query(l_query.second.get());
			l_query.first = query_state::start_write;
		}

		template<class TQuery, bcUINT32 TBufferSize>
		void bc_device_query<TQuery, TBufferSize>::end(bc_render_thread& p_render_thread) noexcept
		{
			auto& l_query = m_queries[m_write_index];

			// Do not write onto previous queries if has not started or has not read (in case of queries which only have end)
			if (l_query.first != query_state::start_write && l_query.first != query_state::read)
			{
				return;
			}

			p_render_thread.end_query(l_query.second.get());
			l_query.first = query_state::end_write;
			m_write_index = (m_write_index + 1) % TBufferSize;
		}

		template<class TQuery, bcUINT32 TBufferSize>
		std::pair<bool, bcUINT64> bc_device_query<TQuery, TBufferSize>::get_data(bc_render_thread& p_render_thread)
		{
			auto& l_query = m_queries[m_read_index];
			const auto l_result = p_render_thread.get_query_data(l_query.second.get());

			if(l_result.first)
			{
				l_query.first = query_state::read;
				m_last_data = l_result.second;
				m_read_index = (m_read_index + 1) % TBufferSize;
			}

			return l_result;
		}
	}
}
