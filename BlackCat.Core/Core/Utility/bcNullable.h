// [2/5/2015 MRB]

#pragma once

#include "Core/CorePCH.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_nullable
		{
			using type = T;
			using this_type = bc_nullable< type >;

		public:
			bc_nullable() noexcept(std::is_nothrow_constructible<type>::value)
				: m_is_set(false)
			{
			}

			explicit bc_nullable(const type& p_value) noexcept(std::is_nothrow_assignable<type, type>::value)
			{
				_set(p_value);
			}

			explicit bc_nullable(type&& p_value) noexcept(std::is_nothrow_move_assignable<type, type>::value)
			{
				_set(std::forward<type>(p_value));
			}

			bc_nullable(std::nullptr_t) noexcept(std::is_nothrow_constructible<type>::value)
			{
				m_is_set = false;
			}

			~bc_nullable() noexcept(true)
			{
			}

			type& operator =(const type& p_value) noexcept(std::is_nothrow_assignable<type, type>::value)
			{ 
				_set(p_value); 
				
				return m_value; 
			}

			type& operator =(type&& p_value) noexcept(std::is_nothrow_move_assignable<type, type>::value)
			{ 
				_set(std::move(p_value)); 
				
				return m_value; 
			}

			void operator =(std::nullptr_t) noexcept(true)
			{
				m_is_set = false;
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
				return &m_value;
			}

			type& operator *() noexcept(true)
			{
				return m_value;
			}

			bcInline type& get() const noexcept(true)
			{
				bcAssert(m_is_set);

				return m_value;
			}

			bcInline bool is_set() const noexcept(true)
			{ 
				return m_is_set; 
			}

			void reset() noexcept(true)
			{
				m_is_set = false;
			}

		protected:

		private:
			bool m_is_set;
			type m_value;

			bcInline void _set(const type& p_value)
			{
				m_value = p_value; 
				m_is_set = true;
			}

			bcInline void _set(type&& p_value)
			{
				m_value = std::move(p_value); 
				m_is_set = true;
			}

			bcInline bool _equal(this_type& p_other) const
			{
				if (!m_is_set)
					return !p_other.m_is_set;

				return m_value == p_other.m_value;
			}

			bcInline bool _equal(std::nullptr_t) const
			{
				return !m_is_set;
			}
		};
	}
}