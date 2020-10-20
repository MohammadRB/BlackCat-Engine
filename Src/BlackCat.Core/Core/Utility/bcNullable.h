// [2/5/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Utility/bcTemplateMetaType.h"

namespace black_cat
{
	namespace core
	{
		
		/**
		 * \brief 
		 * Wrap a non-nullable object (a class that does not support uninitialized state).
		 * This class does not use dynamic memory
		 * \tparam T 
		 */
		template< typename T >
		class bc_nullable
		{
		public:
			using type = T;

		public:
			bc_nullable() noexcept
				: m_set(false),
				m_buffer()
			{
			}

			bc_nullable(std::nullptr_t) noexcept
				: bc_nullable()
			{
			}

			template< typename = std::enable_if_t< bc_type_traits< type >::is_copyable > >
			explicit bc_nullable(const type& p_value) noexcept(bc_type_traits<type>::is_no_throw_copy)
				: bc_nullable()
			{
				_set(p_value);
			}

			explicit bc_nullable(type&& p_value) noexcept(bc_type_traits<type>::is_no_throw_move)
				: bc_nullable()
			{
				_set(std::move(p_value));
			}

			template< typename = std::enable_if_t< bc_type_traits< type >::is_copyable > >
			bc_nullable(const bc_nullable& p_other) noexcept(bc_type_traits<type>::is_no_throw_copy)
				: bc_nullable()
			{
				if(p_other.is_set())
				{
					_set(p_other.get());
				}
			}

			bc_nullable(bc_nullable&& p_other) noexcept(bc_type_traits<type>::is_no_throw_move)
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

			template< typename = std::enable_if_t< bc_type_traits<type>::is_copyable > >
			bc_nullable& operator=(const bc_nullable& p_other) noexcept(bc_type_traits<type>::is_no_throw_copy)
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

			bc_nullable& operator=(bc_nullable&& p_other) noexcept(bc_type_traits<type>::is_no_throw_move)
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

			template< typename = std::enable_if_t< bc_type_traits< type >::is_copyable > >
			type& operator =(const type& p_value) noexcept(bc_type_traits<type>::is_no_throw_copy)
			{ 
				_set(p_value); 
				
				return get(); 
			}

			type& operator =(type&& p_value) noexcept(bc_type_traits<type>::is_no_throw_move)
			{ 
				_set(std::move(p_value)); 
				
				return get(); 
			}

			void operator =(std::nullptr_t) noexcept
			{
				_unset();
			}

			bool operator ==(bc_nullable& p_other) const noexcept
			{
				return _equal(p_other);
			}

			bool operator ==(std::nullptr_t) const noexcept
			{ 
				return _equal(std::nullptr_t());
			}

			bool operator !=(bc_nullable& p_other) const noexcept
			{
				return !_equal(p_other);
			}

			bool operator !=(std::nullptr_t) const noexcept
			{ 
				return !_equal(std::nullptr_t()); 
			}

			type* operator ->()
			{
				return &get();
			}

			const type* operator ->() const
			{
				return &get();
			}

			type& operator *() noexcept
			{
				return get();
			}

			const type& operator *() const noexcept
			{
				return get();
			}

			type& get() noexcept
			{
				bcAssert(is_set());

				return *reinterpret_cast< type* >(&m_buffer);
			}

			const type& get() const noexcept
			{
				return const_cast<bc_nullable*>(this)->get();
			}

			bool is_set() const noexcept
			{ 
				return m_set;
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
			bool m_set;
			bcBYTE m_buffer[sizeof(type)];

			void _set(const type& p_value)
			{
				if (!is_set())
				{
					new(m_buffer) type(p_value);
					m_set = true;
				}
				else
				{
					get() = p_value;
				}
			}

			void _set(type&& p_value)
			{
				if (!is_set())
				{
					new (m_buffer) type(std::move(p_value));
					m_set = true;
				}
				else
				{
					get() = std::move(p_value);
				}
			}

			void _unset()
			{
				if(is_set())
				{
					get().~type();
					m_set = false;
				}
			}

			bool _equal(bc_nullable& p_other) const
			{
				if (!is_set())
				{
					return !p_other.is_set();
				}

				return get() == p_other.get();
			}

			bool _equal(std::nullptr_t) const
			{
				return !is_set();
			}
		};
	}
}