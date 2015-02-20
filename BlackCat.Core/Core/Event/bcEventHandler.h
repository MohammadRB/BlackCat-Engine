// [2/11/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcDelegate.h"

namespace black_cat
{
	namespace core
	{
		using bc_event_handle = bcSIZE;

		template< typename T >
		class bc_event_handler;
			
		template< typename R, typename ...A >
		class bc_event_handler< R(A...) >
		{
		public:
			using this_type = bc_event_handler< R(A...) >;
			using delegate_type = bc_delegate< R(A...) >;
			using return_type = R;

		public:
			bc_event_handler() noexcept(true)
			{
			}

			bc_event_handler(bcUINT32 p_reserve) noexcept(true)
			{
				m_delegates.reserve(p_reserve);
			}

			~bc_event_handler() noexcept(true)
			{
			}

			bcUINT32 delegate_count() const noexcept(true)
			{
				return m_delegates.size();
			}

			template < typename ...A >
			void operator()(A&&... p_args) 
			{
				for (auto l_i = m_delegates.begin(), l_e = m_delegates.end(); l_i != l_e; ++l_i)
				{
					(*l_i)(std::forward<A>(p_args)...);
				}
			}

			bc_event_handle add_delegate(delegate_type&& p_del)
			{
				bc_event_handle l_index = m_delegates.size();
				
				m_delegates.push_back(std::move(p_del));

				return l_index;
			}

			void remove_delegate(bc_event_handle p_index)
			{
				m_delegates.erase(m_delegates.begin() + p_index);
			}

			void clear()
			{
				m_delegates.clear();
			}

		protected:

		private:
			bc_vector< delegate_type > m_delegates;
		};
	}
}