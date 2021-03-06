// [11/29/2014 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/CorePCH.h"
#include "Core/Utility/bcTemplateMetaType.h"
#include "Core/Utility/bcDelegate.h"
#include "Core/Concurrency/bcConcurrency.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_lazy
		{
		public:
			using type = T;
			using this_type = bc_lazy;
			using initializer_type = bc_delegate< type*() >;
			using cleanup_type = bc_delegate< void(type*) >;

		public:
			bc_lazy()
				: bc_lazy(bc_delegate< type*() >(&this_type::_default_initializer),
				bc_delegate< void(type*) >(&this_type::_default_cleanup))
			{
			}

			bc_lazy(initializer_type&& p_initializer, cleanup_type&& p_cleanup)
				: m_initializer(std::move(p_initializer)),
				m_cleanup(std::move(p_cleanup)),
				m_pointer(nullptr)
			{
			}

			bc_lazy(const this_type& p_other) noexcept(bc_type_traits< T >::is_no_throw_copy)
			{
				_assign(p_other);
			}

			bc_lazy(this_type&& p_other) noexcept
			{
				_assign(std::move(p_other));
			}

			~bc_lazy()
			{
				_cleanup();
			}

			this_type& operator =(const this_type& p_other) noexcept(bc_type_traits< T >::is_no_throw_copy)
			{
				_assign(p_other);

				return *this;
			}

			this_type& operator =(this_type&& p_other) noexcept
			{
				_assign(std::move(p_other));

				return *this;
			}

			T* operator ->() noexcept
			{ 
				return get(); 
			}

			const T* operator ->() const noexcept
			{
				return get();
			}

			T& operator *() 
			{ 
				return *get(); 
			}

			const T& operator *() const
			{
				return *get();
			}

			T* get()
			{
				return m_pointer == nullptr ? (m_pointer = m_initializer()) : m_pointer;
			}

			const T* get() const
			{
				return m_pointer == nullptr ? (m_pointer = m_initializer()) : m_pointer;
			}

			bool is_set() const noexcept
			{
				return m_pointer;
			}

			bool operator ==(std::nullptr_t) const noexcept
			{
				return !is_set();
			}

			bool operator !=(std::nullptr_t) const noexcept
			{
				return is_set();
			}

		private:
			static T* _default_initializer() { return new type(); }

			static void _default_cleanup(type* p_pointer) { delete p_pointer; }

			void _cleanup()
			{
				if (m_pointer) m_cleanup(m_pointer);
			}

			void _assign(const this_type& p_other)
			{
				_cleanup();

				m_initializer = p_other.m_initializer;
				m_cleanup = p_other.m_cleanup;

				if (p_other.m_pointer)
				{
					if (!m_pointer)
						m_pointer = m_initializer();

					*m_pointer = *p_other.m_pointer;
				}
				else if (m_pointer)
				{
					m_cleanup(m_pointer);
					m_pointer = nullptr;
				}
			}

			void _assign(this_type&& p_other)
			{
				_cleanup();

				m_initializer = std::move(p_other.m_initializer);
				m_cleanup = std::move(p_other.m_cleanup);

				if (m_pointer)
					m_cleanup(m_pointer);

				m_pointer = p_other.m_pointer;
				p_other.m_pointer = nullptr;
			}

			initializer_type m_initializer;
			cleanup_type m_cleanup;
			mutable T* m_pointer;
		};

		template< typename T >
		class bc_concurrent_lazy
		{
		public:
			using type = T;
			using this_type = bc_concurrent_lazy;
			using initializer_type = bc_delegate< type*() >;
			using cleanup_type = bc_delegate< void(type*) >;

		public:
			bc_concurrent_lazy() : bc_concurrent_lazy(bc_delegate< type*() >(&this_type::_default_initializer),
				bc_delegate< void(type*) >(&this_type::_default_cleanup))
			{
			}

			bc_concurrent_lazy(initializer_type&& p_initializer, cleanup_type&& p_cleanup)
				: m_initializer(std::move(p_initializer)),
				m_cleanup(std::move(p_cleanup)),
				m_pointer(nullptr)
			{
			}

			bc_concurrent_lazy(const this_type& p_other) noexcept(bc_type_traits< T >::is_no_throw_copy)
			{
				_assign(p_other);
			}

			bc_concurrent_lazy(this_type&& p_other) noexcept
			{
				_assign(std::move(p_other));
			}

			~bc_concurrent_lazy()
			{
				T* l_pointer = m_pointer.load(core_platform::bc_memory_order::acquire);

				if (l_pointer) 
					m_cleanup(l_pointer);
			}

			this_type& operator =(const this_type& p_other) noexcept(bc_type_traits< T >::is_no_throw_copy)
			{
				_assign(p_other);

				return *this;
			}

			this_type& operator =(this_type&& p_other) noexcept
			{
				_assign(std::move(p_other));

				return *this;
			}

			T* operator ->() noexcept
			{ 
				return get();
			}

			const T* operator ->() const noexcept
			{
				return get();
			}

			T& operator *() 
			{ 
				return *get(); 
			}

			const T& operator *() const
			{
				return *get();
			}

			T* get()
			{
				return bc_concurrency::double_check_lock(m_pointer, m_mutex, m_initializer);
			}

			const T* get() const
			{
				return bc_concurrency::double_check_lock(m_pointer, m_mutex, m_initializer);
			}

			bool is_set() const noexcept
			{
				return m_pointer.load(core_platform::bc_memory_order::relaxed);
			}

			bool operator ==(std::nullptr_t) const noexcept
			{
				return !is_set();
			}

			bool operator !=(std::nullptr_t) const noexcept
			{
				return is_set();
			}

		private:
			static T* _default_initializer()
			{
				return new type();
			}

			static void _default_cleanup(type* p_pointer)
			{
				delete p_pointer;
			}

			void _cleanup()
			{
				T* l_pointer = m_pointer.load();
				if (l_pointer)
				{
					m_cleanup(l_pointer);
				}
			}

			void _assign(const this_type& p_other)
			{
				_cleanup();

				m_initializer = p_other.m_initializer;
				m_cleanup = p_other.m_cleanup;

				T* l_other_pointer = p_other.m_pointer.load();
				T* l_pointer = m_pointer.load();

				if (l_other_pointer)
				{
					if (!l_pointer)
					{
						l_pointer = m_initializer();
					}

					*l_pointer = *l_other_pointer;
					m_pointer.store(l_pointer);
				}
				else if (l_pointer)
				{
					m_cleanup(l_pointer);
					m_pointer.store(nullptr);
				}
			}

			void _assign(this_type&& p_other)
			{
				_cleanup();

				m_initializer = std::move(p_other.m_initializer);
				m_cleanup = std::move(p_other.m_cleanup);

				m_pointer.store(p_other.m_pointer.load());
				p_other.m_pointer.store(nullptr);
			}

			initializer_type m_initializer;
			cleanup_type m_cleanup;
			mutable core_platform::bc_mutex m_mutex;
			mutable core_platform::bc_atomic<T*> m_pointer;
		};
	}
}