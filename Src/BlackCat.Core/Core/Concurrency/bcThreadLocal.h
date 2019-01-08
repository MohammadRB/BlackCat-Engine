//  [10/28/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcThreadLocal.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"

namespace black_cat
{
	namespace core
	{
		// TODO instead of using template argument use bc_object_allocator or use parameter
		/**
		 * \brief Same as core_platform::bc_thread_local but provide auto initialization. if type is an integral it will initialize to zero
		 * \tparam T 
		 * \tparam t_alloc_type 
		 */
		template<typename T, bc_alloc_type t_alloc_type = bc_alloc_type::unknown>
		class bc_thread_local
		{
			using type = T;
			using this_type = bc_thread_local;

		public:
			bc_thread_local()
				: m_local(&_default_clean_up)
			{
				static_assert(t_alloc_type != bc_alloc_type::unknown_movable, "movable pointers are not allowed");
			}

			bc_thread_local(this_type&& p_other) noexcept
				: m_local(std::move(p_other.m_local))
			{
			}

			~bc_thread_local() = default;

			this_type& operator =(this_type&& p_other) noexcept
			{
				m_local = std::move(m_local);

				return *this;
			}

			type* get() const noexcept
			{
				type* l_pointer = m_local.get();

				if (!l_pointer)
				{
					l_pointer = _create<T>();
					m_local.set(l_pointer);
				}

				return l_pointer;
			}

			bool set(type* p_pointer) noexcept
			{
				return m_local.set(p_pointer);
			}

			type* operator ->() const noexcept
			{
				return get();
			}

			type& operator *() const noexcept
			{
				return *get();
			}

			type* release() noexcept
			{
				return m_local.release();
			}

		protected:

		private:
			static void _default_clean_up(type* p_pointer)
			{
				bcDelete(p_pointer);
			}

			template< typename T >
			typename std::enable_if< std::is_integral< T >::value, T* >::type _create() const
			{
				return bcNew(T(0), t_alloc_type);
			}

			template< typename T >
			typename std::enable_if< !std::is_integral< T >::value, T* >::type _create() const
			{
				return bcNew(T(), t_alloc_type);
			}

			mutable core_platform::bc_thread_local<type> m_local;
		};
	}
}