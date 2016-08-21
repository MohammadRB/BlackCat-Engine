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
		// Same as core_platform::bc_thread_local but provide auto initialization. if type is an integral it will initialize to zero
		template<typename T, bc_alloc_type t_alloc_type = bc_alloc_type::unknown>
		class bc_thread_local
		{
			using type = T;
			using this_type = bc_thread_local;

		public:
			bc_thread_local()
				: m_local(&_default_clean_up)
			{
				static_assert(t_alloc_type != bc_alloc_type::unknown_movale, "movable pointers are not allowed");
			}

			bc_thread_local(this_type&& p_other)
			{
				_assign(std::move(p_other));
			}

			~bc_thread_local()
			{
			}

			this_type& operator =(this_type&& p_other)
			{
				_assign(std::move(p_other));

				return *this;
			}

			bcInline type* get() const noexcept(true)
			{
				type* l_pointer = m_local.get();

				if (!l_pointer)
				{
					l_pointer = _create<T>();
					m_local.set(l_pointer);
				}

				return l_pointer;
			}

			bcInline bool set(type* p_pointer) noexcept(true)
			{
				return m_local.set(p_pointer);
			}

			bcInline type* operator ->() const noexcept(true)
			{
				return get();
			}

			bcInline type& operator *() const noexcept(true)
			{
				return *get();
			}

			bcInline type* release() noexcept(true)
			{
				return m_local.release();
			}

		protected:

		private:
			static void _default_clean_up(type* p_pointer)
			{
				bcDelete(p_pointer);
			}

			bcInline void _assign(this_type&& p_other)
			{
				m_local = std::move(m_local);
			}

			template< typename T >
			bcInline typename std::enable_if< std::is_integral< T >::value, T* >::type _create() const
			{
				return bcNew(T(0), t_alloc_type);
			}

			template< typename T >
			bcInline typename std::enable_if< !std::is_integral< T >::value, T* >::type _create() const
			{
				return bcNew(T(), t_alloc_type);
			}

			mutable core_platform::bc_thread_local<type> m_local;
		};
	}
}