// [5/2/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Utility/bcTemplateMetaType.h"

namespace black_cat::core
{
	template<typename T>
	class bc_optional
	{
	public:
		using type = T;

	public:
		bc_optional() noexcept
			: m_pointer(nullptr),
			  m_buffer()
		{
		}

		bc_optional(std::nullptr_t) noexcept
			: bc_optional()
		{
		}

		template<typename = std::enable_if_t<bc_type_traits<type>::is_copyable>>
		explicit bc_optional(const type& p_value) noexcept(bc_type_traits<type>::is_no_throw_copy)
			: bc_optional()
		{
			_set(p_value);
		}

		explicit bc_optional(type p_value) noexcept(bc_type_traits<type>::is_no_throw_move)
			: bc_optional()
		{
			_set(std::move(p_value));
		}

		bc_optional(const bc_optional& p_other) noexcept(bc_type_traits<type>::is_no_throw_copy)
			: bc_optional()
		{
			static_assert(bc_type_traits<type>::is_copyable, "T is not copyable");
				
			if(p_other.has_value())
			{
				_set(*p_other.get());
			}
		}

		bc_optional(bc_optional&& p_other) noexcept(bc_type_traits<type>::is_no_throw_move)
			: bc_optional()
		{
			if(p_other.has_value())
			{
				_set(std::move(*p_other.get()));
				p_other._unset();
			}
		}

		~bc_optional() noexcept
		{
			_unset();
		}

		bc_optional& operator=(const bc_optional& p_other) noexcept(bc_type_traits<type>::is_no_throw_copy)
		{
			static_assert(bc_type_traits<type>::is_copyable, "T is not copyable");
				
			if(p_other.has_value())
			{
				_set(*p_other.get());
			}
			else
			{
				_unset();
			}

			return *this;
		}

		bc_optional& operator=(bc_optional&& p_other) noexcept(bc_type_traits<type>::is_no_throw_move)
		{
			if(p_other.has_value())
			{
				_set(std::move(*p_other.get()));
				p_other._unset();
			}
			else
			{
				_unset();
			}

			return *this;
		}

		template<typename = std::enable_if_t<bc_type_traits<type>::is_copyable>>
		bc_optional& operator =(const type& p_value) noexcept(bc_type_traits<type>::is_no_throw_copy)
		{ 
			_set(p_value); 
			return *this;
		}

		bc_optional& operator =(type p_value) noexcept(bc_type_traits<type>::is_no_throw_move)
		{ 
			_set(std::move(p_value)); 
			return *this;
		}

		bc_optional& operator =(std::nullptr_t) noexcept
		{
			_unset();
			return *this;
		}

		bool operator ==(bc_optional& p_other) const noexcept
		{
			return _equal(p_other);
		}

		bool operator ==(std::nullptr_t) const noexcept
		{ 
			return _equal(std::nullptr_t());
		}

		bool operator !=(bc_optional& p_other) const noexcept
		{
			return !_equal(p_other);
		}

		bool operator !=(std::nullptr_t) const noexcept
		{ 
			return !_equal(std::nullptr_t()); 
		}

		type* operator ->()
		{
			return get();
		}

		const type* operator ->() const
		{
			return get();
		}

		type& operator *() noexcept
		{
			return *get();
		}

		const type& operator *() const noexcept
		{
			return *get();
		}

		type* get() noexcept
		{
			return m_pointer;
		}

		const type* get() const noexcept
		{
			return const_cast<bc_optional*>(this)->get();
		}

		bool has_value() const noexcept
		{
			return m_pointer != nullptr;
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

	private:
		type* m_pointer;
		bcBYTE m_buffer[sizeof(type)];

		void _set(const type& p_value)
		{
			if (!has_value())
			{
				m_pointer = reinterpret_cast<type*>(&m_buffer[0]);
				new(m_pointer) type(p_value);
			}
			else
			{
				*get() = p_value;
			}
		}

		void _set(type&& p_value)
		{
			if (!has_value())
			{
				m_pointer = reinterpret_cast<type*>(&m_buffer[0]);
				new (m_pointer) type(std::move(p_value));
			}
			else
			{
				*get() = std::move(p_value);
			}
		}

		void _unset()
		{
			if(has_value())
			{
				m_pointer->~type();
				m_pointer = nullptr;
			}
		}

		bool _equal(bc_optional& p_other) const
		{
			if (!has_value())
			{
				return !p_other.has_value();
			}

			return *get() == *p_other.get();
		}

		bool _equal(std::nullptr_t) const
		{
			return !has_value();
		}
	};

	template<typename T>
	bc_optional<T> bc_make_optional(T&& p_value)
	{
		return bc_optional<T>(std::forward<T>(p_value));
	}
}
