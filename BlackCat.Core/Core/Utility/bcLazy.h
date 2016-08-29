// [11/29/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Utility/bcDelegate.hpp"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"

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
			bc_lazy() : bc_lazy(bc_delegate< type*() >(&this_type::_default_initializer),
				bc_delegate< void(type*) >(&this_type::_default_cleanup))
			{
			}

			bc_lazy(initializer_type&& p_initializer, cleanup_type&& p_cleanup)
				: m_initializer(std::move(p_initializer)),
				m_cleanup(std::move(p_cleanup)),
				m_pointer(nullptr)
			{
			}

			bc_lazy(const this_type& p_other)
			{
				_assign(p_other);
			}

			bc_lazy(this_type&& p_other)
			{
				_assign(std::move(p_other));
			}

			~bc_lazy()
			{
				_cleanup();
			}

			this_type& operator =(const this_type& p_other)
			{
				_assign(p_other);

				return *this;
			}

			this_type& operator =(this_type&& p_other)
			{
				_assign(std::move(p_other));

				return *this;
			}

			T* operator ->() const { return get(); }

			T& operator *() const { return *get(); }

			T* get() const { return m_pointer == nullptr ? (m_pointer = m_initializer()) : m_pointer; }

			bool is_set() const { return m_pointer; }

			bool operator ==(std::nullptr_t) const
			{
				return !is_set();
			}

			bool operator !=(std::nullptr_t) const
			{
				return is_set();
			}

		protected:

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

			bc_concurrent_lazy(const this_type& p_other)
			{
				_assign(p_other);
			}

			bc_concurrent_lazy(this_type&& p_other)
			{
				_assign(std::move(p_other));
			}

			~bc_concurrent_lazy()
			{
				T* l_pointer = m_pointer.load(core_platform::bc_memory_order::acquire);

				if (l_pointer) 
					m_cleanup(l_pointer);
			}

			this_type& operator =(const this_type& p_other)
			{
				_assign(p_other);

				return *this;
			}

			this_type& operator =(this_type&& p_other)
			{
				_assign(std::move(p_other));

				return *this;
			}

			T* operator ->() const { return get(); }

			T& operator *() const { return *get(); }

			T* get() const
			{
				T* l_pointer;
				static core_platform::bc_mutex s_mutex;

				if ((l_pointer = m_pointer.load(core_platform::bc_memory_order::acquire)) == nullptr)
				{
					core_platform::bc_lock_guard< core_platform::bc_mutex > l_gaurd(s_mutex);

					if ((l_pointer = m_pointer.load(core_platform::bc_memory_order::relaxed)) == nullptr)
					{
						l_pointer = m_initializer();
						m_pointer.store(l_pointer, core_platform::bc_memory_order::release);
					}
				}

				return l_pointer;
			}

			bool is_set() const
			{
				return m_pointer.load(core_platform::bc_memory_order::relaxed);
			}

			bool operator ==(std::nullptr_t) const
			{
				return !is_set();
			}

			bool operator !=(std::nullptr_t) const
			{
				return is_set();
			}

		protected:

		private:
			static T* _default_initializer() { return new type(); }

			static void _default_cleanup(type* p_pointer) { delete p_pointer; }

			void _cleanup()
			{
				T* l_pointer = m_pointer.load(core_platform::bc_memory_order::acquire);

				if (l_pointer) 
					m_cleanup(l_pointer);
			}

			void _assign(const this_type& p_other)
			{
				_cleanup();

				m_initializer = p_other.m_initializer;
				m_cleanup = p_other.m_cleanup;

				T* l_other_pointer = p_other.m_pointer.load(core_platform::bc_memory_order::acquire);
				T* l_pointer = m_pointer.load(core_platform::bc_memory_order::acquire);

				if (l_other_pointer)
				{
					if (!l_pointer)
						l_pointer = m_initializer();

					*l_pointer = *l_other_pointer;

					m_pointer.store(l_pointer, core_platform::bc_memory_order::release);
				}
				else if (l_pointer)
				{
					m_cleanup(l_pointer);
					m_pointer.store(nullptr, core_platform::bc_memory_order::relaxed);
				}
			}

			void _assign(this_type&& p_other)
			{
				_cleanup();

				m_initializer = std::move(p_other.m_initializer);
				m_cleanup = std::move(p_other.m_cleanup);

				T* l_other_pointer = p_other.m_pointer.load(core_platform::bc_memory_order::acquire);
				T* l_pointer = m_pointer.load(core_platform::bc_memory_order::acquire);

				if (l_pointer)
					m_cleanup(l_pointer);

				m_pointer.store(l_other_pointer, core_platform::bc_memory_order::release);
				p_other.m_pointer.store(nullptr, core_platform::bc_memory_order::relaxed);
			}

			initializer_type m_initializer;
			cleanup_type m_cleanup;
			mutable core_platform::bc_atomic<T*> m_pointer;
		};
	}
}