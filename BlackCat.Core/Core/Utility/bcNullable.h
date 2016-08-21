// [2/5/2015 MRB]

#pragma once

#include "Core/CorePCH.h"

namespace black_cat
{
	namespace core
	{
		// Wrap a non-nullable object (a class that doesn't support uninitialized state)
		// also this class doesn't use dynamic memory
		template< typename T >
		class bc_nullable
		{
			using type = T;
			using this_type = bc_nullable< type >;

		public:
			bc_nullable() noexcept
				: m_value(nullptr)
			{
			}

			bc_nullable(std::nullptr_t) noexcept
				: bc_nullable()
			{
			}

			template< typename = typename std::enable_if< std::is_copy_constructible< typename std::decay< type >::type >::type >::type >
			explicit bc_nullable(const type& p_value) noexcept(std::is_nothrow_copy_constructible<type>::value)
				: bc_nullable()
			{
				_set(p_value);
			}

			explicit bc_nullable(type&& p_value) noexcept(std::is_nothrow_move_constructible<type>::value)
				: bc_nullable()
			{
				_set(std::move(p_value));
			}

			template< typename = typename std::enable_if< std::is_copy_constructible< typename std::decay< type >::type >::type >::type >
			bc_nullable(const bc_nullable& p_other) noexcept(std::is_nothrow_copy_constructible<type>::value)
				: bc_nullable()
			{
				if(p_other.is_set())
				{
					_set(p_other.get());
				}
			}

			bc_nullable(bc_nullable&& p_other) noexcept(std::is_nothrow_move_constructible<type>::value)
				: bc_nullable()
			{
				if(p_other.is_set())
				{
					_set(std::move(p_other.get()));
					p_other._unset();
				}
			}

			~bc_nullable() noexcept(true)
			{
				_unset();
			}

			template< typename = typename std::enable_if< std::is_copy_constructible< typename std::decay< type >::type >::type >::type >
			bc_nullable& operator=(const bc_nullable& p_other) noexcept(std::is_nothrow_copy_constructible<type>::value)
			{
				if(p_other.is_set())
				{
					_set(p_other.get());
				}
				else
				{
					_unset();
				}

				return *this;
			}

			bc_nullable& operator=(bc_nullable&& p_other) noexcept(std::is_nothrow_move_constructible<type>::value)
			{
				if(p_other.is_set())
				{
					_set(std::move(p_other.get()));
					p_other._unset();
				}
				else
				{
					_unset();
				}

				return *this;
			}

			template< typename = typename std::enable_if< std::is_copy_constructible< typename std::decay< type >::type >::type >::type >
			type& operator =(const type& p_value) noexcept(std::is_nothrow_copy_assignable<type>::value)
			{ 
				_set(p_value); 
				
				return m_value; 
			}

			type& operator =(type&& p_value) noexcept(std::is_nothrow_move_assignable<type>::value)
			{ 
				_set(std::move(p_value)); 
				
				return *m_value; 
			}

			void operator =(std::nullptr_t) noexcept(true)
			{
				_unset();
			}

			bool operator ==(this_type& p_other) const noexcept(true)
			{
				return _equal(p_other);
			}

			bool operator ==(std::nullptr_t) const noexcept(true)
			{ 
				return _equal(std::nullptr_t());
			}

			bool operator !=(this_type& p_other) const noexcept(true)
			{
				return !_equal(p_other);
			}

			bool operator !=(std::nullptr_t) const noexcept(true)
			{ 
				return !_equal(std::nullptr_t()); 
			}

			type* operator ->()
			{
				return m_value;
			}

			type& operator *() noexcept
			{
				return *m_value;
			}

			type& get() const noexcept
			{
				bcAssert(is_set());

				return *m_value;
			}

			bool is_set() const noexcept
			{ 
				return m_value != nullptr;
			}

			void reset() noexcept
			{
				_unset();
			}

			void reset(std::nullptr_t) noexcept
			{
				_unset();
			}

			void reset(const type& p_value) noexcept
			{
				_set(p_value);
			}

			void reset(type&& p_value) noexcept
			{
				_set(std::move(p_value));
			}

		protected:

		private:
			type* m_value;
			bcBYTE m_buffer[sizeof(type)];

			void _set(const type& p_value)
			{
				if (!is_set())
				{
					new(m_buffer) type(p_value);
					m_value = reinterpret_cast< type* >(&m_buffer[0]);
				}
				else
				{
					*m_value = p_value;
				}
			}

			void _set(type&& p_value)
			{
				if (!is_set())
				{
					new (m_buffer) type(std::move(p_value));
					m_value = reinterpret_cast< type* >(&m_buffer[0]);
				}
				else
				{
					*m_value = std::move(p_value);
				}
			}

			void _unset()
			{
				if(is_set())
				{
					m_value->~type();
					m_value = nullptr;
				}
			}

			bool _equal(this_type& p_other) const
			{
				if (!is_set())
					return !p_other.is_set();

				return *m_value == *p_other.m_value;
			}

			bool _equal(std::nullptr_t) const
			{
				return !is_set();
			}
		};
	}
}