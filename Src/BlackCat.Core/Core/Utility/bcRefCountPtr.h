// [04/02/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Memory/bcPtr.h"

namespace black_cat
{
	namespace core
	{
		template< class T, class TDeleter >
		class bc_ref_count_ptr;
		template< class T, class TDeleter >
		class bc_ref_count_handle;

		class bc_ref_count
		{
		public:
			template< class T, class TDeleter >
			friend class bc_ref_count_ptr;
			template< class T, class TDeleter >
			friend class bc_ref_count_handle;

		protected:
			bc_ref_count()
				: m_ref_count(0)
			{
			}

			bc_ref_count(bc_ref_count&& p_other) noexcept
			{
				operator=(std::move(p_other));
			}

			~bc_ref_count() = default;

			bc_ref_count& operator=(bc_ref_count&& p_other) noexcept
			{
				m_ref_count.store(p_other.m_ref_count.load(core_platform::bc_memory_order::relaxed), core_platform::bc_memory_order::relaxed);
				p_other.m_ref_count.store(0, core_platform::bc_memory_order::relaxed);

				return *this;
			}

			bcUINT32 get_ref_count() const noexcept
			{
				return m_ref_count.load(core_platform::bc_memory_order::relaxed);
			}
			
			void add_ref() noexcept
			{
				m_ref_count.fetch_add(1U, core_platform::bc_memory_order::relaxed);
			}

			/**
			 * \brief Return true if reference count reaches zero
			 * \return 
			 */
			bool dec_ref() noexcept
			{
				const auto l_ref_count = m_ref_count.fetch_sub(1U, core_platform::bc_memory_order::relaxed) - 1;
				BC_ASSERT(l_ref_count >= 0);

				return l_ref_count == 0;
			}
			
		private:
			core_platform::bc_atomic<bcINT32> m_ref_count;
		};

		/**
		 * \brief A movable aware reference counting pointer.
		 * \tparam T 
		 * \tparam TDeleter 
		 */
		template< class T, class TDeleter >
		class bc_ref_count_ptr
		{
		public:
			using pointer = bc_handle_ptr< T >;
			using element_type = T;
			using element_ptr = T*;

			template< class T, class TDeleter >
			friend class bc_ref_count_ptr;

		public:
			bc_ref_count_ptr() noexcept;

			bc_ref_count_ptr(std::nullptr_t) noexcept;

			bc_ref_count_ptr(element_ptr p_pointer, TDeleter p_deleter) noexcept;

			template< class TOther >
			bc_ref_count_ptr(TOther* p_pointer, TDeleter p_deleter) noexcept;

			bc_ref_count_ptr(const bc_ref_count_ptr& p_other) noexcept;

			bc_ref_count_ptr(bc_ref_count_ptr&& p_other) noexcept;

			template< class TOther >
			bc_ref_count_ptr(const bc_ref_count_ptr< TOther, TDeleter >& p_other) noexcept;

			template< class TOther >
			bc_ref_count_ptr(bc_ref_count_ptr< TOther, TDeleter >&& p_other) noexcept;

			~bc_ref_count_ptr();

			bc_ref_count_ptr& operator=(std::nullptr_t) noexcept;

			bc_ref_count_ptr& operator=(const bc_ref_count_ptr& p_other) noexcept;

			bc_ref_count_ptr& operator=(bc_ref_count_ptr&& p_other) noexcept;

			template< class TOther >
			bc_ref_count_ptr& operator=(const bc_ref_count_ptr< TOther, TDeleter >& p_other) noexcept;

			template< class TOther >
			bc_ref_count_ptr& operator=(bc_ref_count_ptr< TOther, TDeleter >&& p_other) noexcept;

			void reset() noexcept;

			void reset(element_ptr p_pointer) noexcept;

			void reset(bc_ref_count_ptr& p_other) noexcept;

			void swap(bc_ref_count_ptr& p_other) noexcept;

			element_ptr get() const noexcept;

			bcUINT32 ref_count() const noexcept;

			element_type& operator*() const;

			element_ptr operator->() const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			operator bool() const noexcept;

		private:
			void _construct(element_ptr p_pointer, const TDeleter& p_deleter);

			void _destruct();

			template< typename T1 >
			void _assign(const bc_ref_count_ptr< T1, TDeleter >& p_other);

			template< typename T1 >
			void _assign(bc_ref_count_ptr< T1, TDeleter >&& p_other);

			pointer m_pointer;
			TDeleter m_deleter;
		};

		/**
		 * \brief Reference counting handle pointer.
		 * Handle must be nullptr constructable, assignable and comparable.
		 * TDeleter must have two operator(): one for deleting owing resource and another for dereferencing resource.
		 * \tparam T 
		 * \tparam TDeleter 
		 */
		template< class T, class TDeleter >
		class bc_ref_count_handle
		{
		public:
			using handle_t = typename TDeleter::handle_t;
			using element_t = T;

			template< class T, class TDeleter >
			friend class bc_ref_count_handle;

		public:
			bc_ref_count_handle() noexcept;

			bc_ref_count_handle(std::nullptr_t) noexcept;

			bc_ref_count_handle(handle_t p_pointer, TDeleter p_deleter) noexcept;

			bc_ref_count_handle(const bc_ref_count_handle& p_other) noexcept;

			bc_ref_count_handle(bc_ref_count_handle&& p_other) noexcept;

			template< class TOther >
			bc_ref_count_handle(const bc_ref_count_handle< TOther, TDeleter >& p_other) noexcept;

			template< class TOther >
			bc_ref_count_handle(bc_ref_count_handle< TOther, TDeleter >&& p_other) noexcept;

			~bc_ref_count_handle();

			bc_ref_count_handle& operator=(std::nullptr_t) noexcept;

			bc_ref_count_handle& operator=(const bc_ref_count_handle& p_other) noexcept;

			bc_ref_count_handle& operator=(bc_ref_count_handle&& p_other) noexcept;

			template< class TOther >
			bc_ref_count_handle& operator=(const bc_ref_count_handle< TOther, TDeleter >& p_other) noexcept;

			template< class TOther >
			bc_ref_count_handle& operator=(bc_ref_count_handle< TOther, TDeleter >&& p_other) noexcept;

			void reset() noexcept;

			void reset(handle_t p_handle) noexcept;

			void swap(bc_ref_count_handle& p_other) noexcept;

			element_t* get() const noexcept;

			handle_t get_handle() const noexcept;

			bcUINT32 ref_count() const noexcept;

			element_t& operator*() const;

			element_t* operator->() const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			operator bool() const noexcept;

		private:
			void _construct(handle_t p_handle, const TDeleter& p_deleter);

			void _destruct();

			template< typename T1 >
			void _assign(const bc_ref_count_handle< T1, TDeleter >& p_other);

			template< typename T1 >
			void _assign(bc_ref_count_handle< T1, TDeleter >&& p_other);

			handle_t m_handle;
			TDeleter m_deleter;
		};

		// -- bc_ref_count_ptr --------------------------------------------------------------------------------

		template< class T, class TDeleter >
		bc_ref_count_ptr< T, TDeleter >::bc_ref_count_ptr() noexcept
			: m_pointer(nullptr)
		{
		}

		template< class T, class TDeleter >
		bc_ref_count_ptr< T, TDeleter >::bc_ref_count_ptr(std::nullptr_t) noexcept
			: bc_ref_count_ptr()
		{
		}

		template< class T, class TDeleter >
		bc_ref_count_ptr< T, TDeleter >::bc_ref_count_ptr(element_ptr p_pointer, TDeleter p_deleter) noexcept
		{
			_construct(static_cast< element_ptr >(p_pointer), p_deleter);
		}

		template< class T, class TDeleter >
		template< class TOther >
		bc_ref_count_ptr< T, TDeleter >::bc_ref_count_ptr(TOther* p_pointer, TDeleter p_deleter) noexcept
		{
			_construct(static_cast< element_ptr >(p_pointer), p_deleter);
		}

		template< class T, class TDeleter >
		bc_ref_count_ptr< T, TDeleter >::bc_ref_count_ptr(const bc_ref_count_ptr& p_other) noexcept
		{
			_assign(p_other);
		}

		template< class T, class TDeleter >
		bc_ref_count_ptr< T, TDeleter >::bc_ref_count_ptr(bc_ref_count_ptr&& p_other) noexcept
		{
			_assign(std::move(p_other));
		}

		template< class T, class TDeleter >
		template< class TOther >
		bc_ref_count_ptr< T, TDeleter >::bc_ref_count_ptr(const bc_ref_count_ptr< TOther, TDeleter >& p_other) noexcept
		{
			_assign(p_other);
		}

		template< class T, class TDeleter >
		template< class TOther >
		bc_ref_count_ptr< T, TDeleter >::bc_ref_count_ptr(bc_ref_count_ptr< TOther, TDeleter >&& p_other) noexcept
		{
			_assign(std::move(p_other));
		}

		template< class T, class TDeleter >
		bc_ref_count_ptr< T, TDeleter >::~bc_ref_count_ptr()
		{
			_destruct();
		}

		template< class T, class TDeleter >
		bc_ref_count_ptr< T, TDeleter >& bc_ref_count_ptr< T, TDeleter >::operator=(std::nullptr_t) noexcept
		{
			reset(nullptr);

			return *this;
		}

		template< class T, class TDeleter >
		bc_ref_count_ptr< T, TDeleter >& bc_ref_count_ptr< T, TDeleter >::operator=(const bc_ref_count_ptr& p_other) noexcept
		{
			if (this != &p_other)
			{
				_assign(p_other);
			}

			return *this;
		}

		template< class T, class TDeleter >
		bc_ref_count_ptr< T, TDeleter >& bc_ref_count_ptr< T, TDeleter >::operator=(bc_ref_count_ptr&& p_other) noexcept
		{
			if (this != &p_other)
			{
				_assign(std::move(p_other));
			}

			return *this;
		}

		template< class T, class TDeleter >
		template< class TOther >
		bc_ref_count_ptr< T, TDeleter >& bc_ref_count_ptr< T, TDeleter >::operator=(const bc_ref_count_ptr< TOther, TDeleter >& p_other) noexcept
		{
			if (m_pointer != p_other.m_pointer)
			{
				_assign(p_other);
			}

			return *this;
		}

		template< class T, class TDeleter >
		template< class TOther >
		bc_ref_count_ptr< T, TDeleter >& bc_ref_count_ptr< T, TDeleter >::operator=(bc_ref_count_ptr< TOther, TDeleter >&& p_other) noexcept
		{
			if (m_pointer != p_other.m_pointer)
			{
				_assign(std::move(p_other));
			}

			return *this;
		}

		template< class T, class TDeleter >
		void bc_ref_count_ptr<T, TDeleter>::reset() noexcept
		{
			_destruct();
		}

		template< class T, class TDeleter >
		void bc_ref_count_ptr< T, TDeleter >::reset(element_ptr p_pointer) noexcept
		{
			_destruct();
			_construct(p_pointer, m_deleter);
		}

		template< class T, class TDeleter >
		void bc_ref_count_ptr< T, TDeleter >::reset(bc_ref_count_ptr& p_other) noexcept
		{
			_destruct();
			_construct(p_other.m_pointer, p_other.m_deleter);
		}

		template< class T, class TDeleter >
		void bc_ref_count_ptr< T, TDeleter >::swap(bc_ref_count_ptr& p_other) noexcept
		{
			using std::swap;
			swap(m_pointer, p_other.m_pointer);
		}

		template< class T, class TDeleter >
		typename bc_ref_count_ptr< T, TDeleter >::element_ptr bc_ref_count_ptr< T, TDeleter >::get() const noexcept
		{
			return m_pointer;
		}

		template< class T, class TDeleter >
		bcUINT32 bc_ref_count_ptr<T, TDeleter>::ref_count() const noexcept
		{
			if(m_pointer)
			{
				return static_cast<bc_ref_count*>(m_pointer)->get_ref_count();
			}

			return 0;
		}

		template< class T, class TDeleter >
		typename bc_ref_count_ptr< T, TDeleter >::element_type& bc_ref_count_ptr< T, TDeleter >::operator*() const
		{
			return *m_pointer;
		}

		template< class T, class TDeleter >
		typename bc_ref_count_ptr< T, TDeleter >::element_ptr bc_ref_count_ptr< T, TDeleter >::operator->() const noexcept
		{
			return m_pointer;
		}

		template< class T, class TDeleter >
		bool bc_ref_count_ptr< T, TDeleter >::operator==(std::nullptr_t) const noexcept
		{
			return m_pointer == nullptr;
		}

		template< class T, class TDeleter >
		bool bc_ref_count_ptr< T, TDeleter >::operator!=(std::nullptr_t) const noexcept
		{
			return !operator==(nullptr);
		}

		template< class T, class TDeleter >
		bc_ref_count_ptr< T, TDeleter >::operator bool() const noexcept
		{
			return m_pointer != nullptr;
		}

		template< class T, class TDeleter >
		void bc_ref_count_ptr< T, TDeleter >::_construct(element_ptr p_pointer, const TDeleter& p_deleter)
		{
			static_assert(std::is_base_of< bc_ref_count, T >::value, "Pointer class must inherite from bc_ref_count");

			m_pointer = p_pointer;
			m_deleter = p_deleter;

			if (m_pointer)
			{
				static_cast< bc_ref_count* >(m_pointer)->add_ref();
			}
		}

		template< class T, class TDeleter >
		void bc_ref_count_ptr< T, TDeleter >::_destruct()
		{
			if (m_pointer)
			{
				if (static_cast< bc_ref_count* >(m_pointer)->dec_ref())
				{
					m_deleter(m_pointer);
				}

				m_pointer = nullptr;
			}
		}

		template< class T, class TDeleter >
		template< typename T1 >
		void bc_ref_count_ptr< T, TDeleter >::_assign(const bc_ref_count_ptr< T1, TDeleter >& p_other)
		{
			_destruct();
			_construct(static_cast< element_ptr >(p_other.get()), p_other.m_deleter);
		}

		template< class T, class TDeleter >
		template< typename T1 >
		void bc_ref_count_ptr< T, TDeleter >::_assign(bc_ref_count_ptr< T1, TDeleter >&& p_other)
		{
			_destruct();

			m_pointer = p_other.m_pointer;
			m_deleter = std::move(p_other.m_deleter);

			if (p_other.m_pointer)
			{
				p_other.m_pointer = nullptr;
			}
		}

		// -- bc_ref_count_handle --------------------------------------------------------------------------------

		template< class T, class TDeleter >
		bc_ref_count_handle< T, TDeleter >::bc_ref_count_handle() noexcept
			: m_handle(nullptr)
		{
		}

		template< class T, class TDeleter >
		bc_ref_count_handle< T, TDeleter >::bc_ref_count_handle(std::nullptr_t) noexcept
			: bc_ref_count_handle()
		{
		}

		template< class T, class TDeleter >
		bc_ref_count_handle< T, TDeleter >::bc_ref_count_handle(handle_t p_handle, TDeleter p_deleter) noexcept
		{
			_construct(p_handle, p_deleter);
		}

		template< class T, class TDeleter >
		bc_ref_count_handle< T, TDeleter >::bc_ref_count_handle(const bc_ref_count_handle& p_other) noexcept
		{
			_assign(p_other);
		}

		template< class T, class TDeleter >
		bc_ref_count_handle< T, TDeleter >::bc_ref_count_handle(bc_ref_count_handle&& p_other) noexcept
		{
			_assign(std::move(p_other));
		}

		template< class T, class TDeleter >
		template< class TOther >
		bc_ref_count_handle< T, TDeleter >::bc_ref_count_handle(const bc_ref_count_handle< TOther, TDeleter >& p_other) noexcept
		{
			_assign(p_other);
		}

		template< class T, class TDeleter >
		template< class TOther >
		bc_ref_count_handle< T, TDeleter >::bc_ref_count_handle(bc_ref_count_handle< TOther, TDeleter >&& p_other) noexcept
		{
			_assign(std::move(p_other));
		}

		template< class T, class TDeleter >
		bc_ref_count_handle< T, TDeleter >::~bc_ref_count_handle()
		{
			_destruct();
		}

		template< class T, class TDeleter >
		bc_ref_count_handle< T, TDeleter >& bc_ref_count_handle< T, TDeleter >::operator=(std::nullptr_t) noexcept
		{
			reset(nullptr);

			return *this;
		}

		template< class T, class TDeleter >
		bc_ref_count_handle< T, TDeleter >& bc_ref_count_handle< T, TDeleter >::operator=(const bc_ref_count_handle& p_other) noexcept
		{
			if (this != &p_other)
			{
				_assign(p_other);
			}

			return *this;
		}

		template< class T, class TDeleter >
		bc_ref_count_handle< T, TDeleter >& bc_ref_count_handle< T, TDeleter >::operator=(bc_ref_count_handle&& p_other) noexcept
		{
			if (this != &p_other)
			{
				_assign(std::move(p_other));
			}

			return *this;
		}

		template< class T, class TDeleter >
		template< class TOther >
		bc_ref_count_handle< T, TDeleter >& bc_ref_count_handle< T, TDeleter >::operator=(const bc_ref_count_handle< TOther, TDeleter >& p_other) noexcept
		{
			if (this != &p_other)
			{
				_assign(p_other);
			}

			return *this;
		}

		template< class T, class TDeleter >
		template< class TOther >
		bc_ref_count_handle< T, TDeleter >& bc_ref_count_handle< T, TDeleter >::operator=(bc_ref_count_handle< TOther, TDeleter >&& p_other) noexcept
		{
			if (this != &p_other)
			{
				_assign(std::move(p_other));
			}

			return *this;
		}

		template< class T, class TDeleter >
		void bc_ref_count_handle<T, TDeleter>::reset() noexcept
		{
			_destruct();
		}

		template< class T, class TDeleter >
		void bc_ref_count_handle< T, TDeleter >::reset(handle_t p_handle) noexcept
		{
			_destruct();
			_construct(p_handle, m_deleter);
		}

		template< class T, class TDeleter >
		void bc_ref_count_handle< T, TDeleter >::swap(bc_ref_count_handle& p_other) noexcept
		{
			using std::swap;
			swap(m_handle, p_other.m_handle);
		}

		template< class T, class TDeleter >
		typename bc_ref_count_handle< T, TDeleter >::element_t* bc_ref_count_handle< T, TDeleter >::get() const noexcept
		{
			if(m_handle != nullptr)
			{
				return m_deleter(m_handle);
			}

			return nullptr;
		}

		template< class T, class TDeleter >
		typename bc_ref_count_handle<T, TDeleter>::handle_t bc_ref_count_handle<T, TDeleter>::get_handle() const noexcept
		{
			return m_handle;
		}

		template< class T, class TDeleter >
		bcUINT32 bc_ref_count_handle<T, TDeleter>::ref_count() const noexcept
		{
			if(m_handle)
			{
				return static_cast<bc_ref_count*>(get())->get_ref_count();
			}

			return 0;
		}

		template< class T, class TDeleter >
		typename bc_ref_count_handle< T, TDeleter >::element_t& bc_ref_count_handle< T, TDeleter >::operator*() const
		{
			return *get();
		}

		template< class T, class TDeleter >
		typename bc_ref_count_handle< T, TDeleter >::element_t* bc_ref_count_handle< T, TDeleter >::operator->() const noexcept
		{
			return get();
		}

		template< class T, class TDeleter >
		bool bc_ref_count_handle< T, TDeleter >::operator==(std::nullptr_t) const noexcept
		{
			return get() == nullptr;
		}

		template< class T, class TDeleter >
		bool bc_ref_count_handle< T, TDeleter >::operator!=(std::nullptr_t) const noexcept
		{
			return !operator==(nullptr);
		}

		template< class T, class TDeleter >
		bc_ref_count_handle<T, TDeleter>::operator bool() const noexcept
		{
			return m_handle != nullptr;
		}

		template< class T, class TDeleter >
		void bc_ref_count_handle< T, TDeleter >::_construct(handle_t p_handle, const TDeleter& p_deleter)
		{
			static_assert(std::is_base_of< bc_ref_count, T >::value, "Pointer class must inherit from bc_ref_count");

			m_handle = p_handle;
			m_deleter = p_deleter;

			if (m_handle != nullptr)
			{
				static_cast<bc_ref_count*>(get())->add_ref();
			}
		}

		template< class T, class TDeleter >
		void bc_ref_count_handle< T, TDeleter >::_destruct()
		{
			if (m_handle != nullptr)
			{
				element_t* l_ptr = get();
				
				if (static_cast< bc_ref_count* >(l_ptr)->dec_ref())
				{
					m_deleter(l_ptr);
				}

				m_handle = nullptr;
			}
		}

		template< class T, class TDeleter >
		template< typename T1 >
		void bc_ref_count_handle< T, TDeleter >::_assign(const bc_ref_count_handle< T1, TDeleter >& p_other)
		{
			static_assert(std::is_convertible< T1, T >::value, "T1 must be convertible to T");

			_destruct();
			_construct(p_other.m_handle, p_other.m_deleter);
		}

		template< class T, class TDeleter >
		template< typename T1 >
		void bc_ref_count_handle< T, TDeleter >::_assign(bc_ref_count_handle< T1, TDeleter >&& p_other)
		{
			static_assert(std::is_convertible< T1, T >::value, "T1 must be convertible to T");

			_destruct();

			m_handle = p_other.m_handle;
			m_deleter = std::move(p_other.m_deleter);

			if (p_other.m_handle != nullptr)
			{
				p_other.m_handle = nullptr;
			}
		}
	}
}