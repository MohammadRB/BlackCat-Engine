// [11/2/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcDelegate.hpp"

namespace black_cat::core
{
	using bc_event_handler_index = bcSIZE;

	template<typename T>
	class bc_event_handler;
			
	template<typename R, typename ...A>
	class bc_event_handler<R(A...)>
	{
	public:
		using this_type = bc_event_handler<R(A...)>;
		using delegate_type = bc_delegate<R(A...)>;
		using return_type = R;

	public:
		bc_event_handler() = default;

		explicit bc_event_handler(bcUINT32 p_reserve) noexcept
		{
			m_delegates.reserve(p_reserve);
		}

		~bc_event_handler() = default;

		bcUINT32 delegate_count() const noexcept
		{
			return m_delegates.size();
		}

		template <typename ...A>
		void operator()(A&&... p_args) const
		{
			for (auto l_i = m_delegates.begin(), l_e = m_delegates.end(); l_i != l_e; ++l_i)
			{
				if((*l_i) != nullptr)
				{
					(*l_i)(std::forward<A>(p_args)...);
				}
			}
		}

		bc_event_handler_index add_delegate(delegate_type&& p_del)
		{
			bc_event_handler_index l_index = 0;
				
			auto l_item = std::find_if(std::cbegin(m_delegates), std::cend(m_delegates), [&l_index](const delegate_type& p_del)
			{
				++l_index;
				return p_del == nullptr;
			});

			if(l_item != std::cend(m_delegates))
			{
				--l_index;
				*l_item = std::move(p_del);
			}
			else
			{
				m_delegates.push_back(std::move(p_del));
			}

			return l_index;
		}

		void replace_delegate(bc_event_handler_index p_index, delegate_type&& p_del)
		{
			m_delegates.at(p_index) = std::move(p_del);
		}

		void remove_delegate(bc_event_handler_index p_index)
		{
			m_delegates.at(p_index).reset();
		}

		void clear()
		{
			m_delegates.clear();
		}

	private:
		bc_vector<delegate_type> m_delegates;
	};
}
