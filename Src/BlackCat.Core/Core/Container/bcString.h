// [1/6/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/bcException.h"
#include "Core/Container/bcContainer.h"
#include <cerrno>
#include <string>

namespace black_cat
{
	namespace core
	{
		/*
		template < typename TChar, class TTrait, class TAllocator >
		class bc_basic_string_base : public bc_container< TChar, TAllocator >
		{
		public:
		using this_type = bc_basic_string_base;
		using base_type = bc_container< TChar, TAllocator >;
		using traits_type = TTrait;
		using value_type = typename traits_type::char_type;
		using allocator_type = TAllocator;
		using pointer_type = typename base_type::pointer_type;
		using const_pointer_type = typename base_type::const_pointer_type;
		using reference_type = typename base_type::reference_type;
		using const_reference_type = typename base_type::const_reference_type;
		using size_type = typename base_type::size_type;
		using difference_type = typename base_type::difference_type;

		protected:
		using node_type = value_type;
		using node_pointer_type = typename std::pointer_traits< pointer_type >::template rebind< node_type >::other;
		using internal_allocator_type = typename bc_allocator_traits< allocator_type >::template rebind_alloc< node_type >::other;

		public:
		bc_basic_string_base(const allocator_type& p_allocator = allocator_type())
		: m_capacity(0),
		m_first(nullptr),
		m_allocator(p_allocator)
		{
		}

		~bc_basic_string_base() {}

		protected:
		bcInline void _change_capacity(size_type p_new_capacity)
		{
		node_type* l_heap = static_cast< node_type* >(bc_allocator_traits< internal_allocator_type >::allocate(m_allocator, p_new_capacity));

		if (base_type::m_first)
		{
		traits_type::copy(l_heap, base_type::m_first, sizeof(node_type)* std::min< size_type >(base_type::m_size, p_new_capacity));

		bc_allocator_traits< internal_allocator_type >::deallocate(base_type::m_allocator, base_type::m_first);
		}

		base_type::m_first = l_heap;
		bc_allocator_traits< internal_allocator_type >::register_pointer(base_type::m_allocator, &base_type::m_first);
		base_type::m_capacity = p_new_capacity;
		}

		// Zero for auto increasing or other values for manual increasing
		bcInline void _increase_capacity(size_type p_count_to_add = 0)
		{
		size_type l_reserved_count = base_type::m_capacity - base_type::m_size;
		size_type l_new_capacity = base_type::m_capacity == 0 ? 1 : base_type::m_capacity;

		if (l_reserved_count == 0)
		{
		l_new_capacity *= 2;
		l_reserved_count += (l_new_capacity - base_type::m_capacity);
		}

		while (p_count_to_add > l_reserved_count)
		{
		l_new_capacity *= 2;
		l_reserved_count += (l_new_capacity - base_type::m_capacity);
		}

		if (l_new_capacity <= base_type::m_capacity)
		return;

		_change_capacity(l_new_capacity);
		};

		bcInline void _decrease_capacity()
		{
		if (base_type::m_capacity > base_type::m_size)
		{
		_change_capacity(base_type::m_size);
		}
		}

		bcInline void _set_eof()
		{
		traits_type::assign(*(m_first + base_type::m_size), traits_type::to_char_type(traits_type::eof()));
		}

		bcInline void _assign(const value_type* p_string)
		{
		_clear();

		size_type l_str_size = traits_type::length(p_string);

		_increase_capacity(l_str_size + 1);

		traits_type::copy(m_first, p_string, base_type::m_size);

		base_type::base_type::m_size = l_str_size;
		_set_eof();
		}

		bcInline void _append(const value_type* p_string)
		{
		size_type l_str_size = traits_type::length(p_string);

		_increase_capacity(l_str_size);

		traits_type::copy(m_first + base_type::m_size, p_string, l_str_size);

		base_type::m_size = base_type::m_size + l_str_size;
		_set_eof();
		}

		bcInline void _insert(const node_type* p_position, const value_type* p_string)
		{
		bcAssert(p_position >= m_first && p_position < m_first + base_type::m_size);

		difference_type l_offset = p_position - m_first;
		size_type l_str_size = traits_type::length(p_string);

		_increase_capacity(l_str_size);

		traits_type::move(m_first + l_offset + l_str_size, m_first + l_offset, base_type::m_size - l_offset);
		traits_type::copy(m_first + l_offset, p_string, l_str_size);

		base_type::m_size = base_type::m_size + l_str_size;
		_set_eof();
		}

		bcInline void _erase(const node_type* p_position, bcUINT32 p_count)
		{
		bcAssert(base_type::m_size - p_count >= 0);

		difference_type l_offset = p_position - m_first;
		size_type l_str_size = p_count;

		traits_type::move(m_first + l_offset, m_first + l_offset + l_str_size, base_type::m_size - (l_offset + l_str_size));

		base_type::m_size = base_type::m_size - l_str_size;
		_set_eof();
		}

		bcInline bcINT32 _compare(const value_type* p_string) const
		{
		size_type l_str_size = traits_type::length(p_string);

		if (base_type::m_size > l_str_size)
		return 1;
		else if (base_type::m_size < l_str_size)
		return -1;
		else
		{
		bcINT32 l_result = traits_type::compare(m_first, p_string, base_type::m_size);
		return l_result;
		}
		};

		bcInline void _clear()
		{
		if (m_first)
		bc_allocator_traits< internal_allocator_type >::deallocate(m_allocator, m_first);

		m_first = nullptr;
		base_type::base_type::m_size = 0;
		}

		size_type m_capacity;
		node_pointer_type m_first;
		internal_allocator_type m_allocator;
		};

		template < typename TChar, class TTrait = std::char_traits< TChar >, class TAllocator = bc_allocator_movable< TChar > >
		class bc_basic_string : public bc_basic_string_base< TChar, TTrait, TAllocator >
		{
		public:
		using this_type = bc_basic_string;
		using base_type = bc_basic_string_base< TChar, TTrait, TAllocator >;
		using traits_type = typename base_type::traits_type;
		using value_type = typename traits_type::char_type;
		using allocator_type = TAllocator;
		using pointer_type = typename base_type::pointer_type;
		using const_pointer_type = typename base_type::const_pointer_type;
		using reference_type = typename base_type::reference_type;
		using const_reference_type = typename base_type::const_reference_type;
		using size_type = typename base_type::size_type;
		using difference_type = typename base_type::difference_type;
		using iterator_type = bc_random_access_iterator< this_type >;
		using const_iterator_type = bc_const_random_access_iterator< this_type >;

		static const size_type npos = -1;

		private:

		protected:

		public:
		explicit bc_basic_string(const allocator_type& p_allo = allocator_type());

		bc_basic_string(size_type p_count, value_type p_ch, const allocator_type& p_alloc = allocator_type());

		bc_basic_string(const this_type& p_other, size_type p_position, size_type p_count = npos, const allocator_type& p_alloc = allocator_type());

		bc_basic_string(const value_type* p_str, size_type p_count, const allocator_type& p_alloc = allocator_type());

		bc_basic_string(const value_type* p_str, const allocator_type& p_alloc = allocator_type());

		template< class InputIterator >
		bc_basic_string(InputIterator p_first, InputIterator p_last, const allocator_type& p_alloc = allocator_type());

		bc_basic_string(std::initializer_list<value_type> p_init, const allocator_type& p_alloc = allocator_type());

		bc_basic_string(const this_type& p_other);

		bc_basic_string(const this_type& p_other, const allocator_type& p_alloc);

		bc_basic_string(this_type&& p_other);

		bc_basic_string(this_type&& p_other, const allocator_type& p_alloc);

		this_type& operator=(const this_type& p_str);

		this_type& operator=(this_type&& p_str);

		this_type& operator=(const value_type* p_str);

		this_type& operator=(value_type p_ch);

		this_type& operator=(std::initializer_list<value_type> p_ilist);

		this_type& assign(size_type p_count, value_type p_ch);

		this_type& assign(const this_type& p_str);

		this_type& assign(const this_type& p_str, size_type p_pos, size_type p_count = npos);

		this_type& assign(this_type&& p_str);

		this_type& assign(const value_type* p_str, size_type p_count);

		this_type& assign(const value_type* p_str);

		template< class InputIterator >
		this_type& assign(InputIterator p_first, InputIterator p_last);

		this_type& assign(std::initializer_list<value_type> p_ilist);

		allocator_type get_allocator() const;

		reference_type at(size_type p_pos);

		const_reference_type at(size_type p_pos) const;

		reference_type operator[](size_type p_pos);

		const_reference_type operator[](size_type p_pos) const;

		value_type& front();

		const value_type& front() const;

		value_type& back();

		const value_type& back() const;

		const value_type* data() const;

		const value_type* c_str() const;

		iterator_type begin();

		const_iterator_type begin() const;

		const_iterator_type cbegin() const;

		iterator_type end();

		const_iterator_type end() const;

		const_iterator_type cend() const;

		size_type length() const;

		void reserve(size_type p_new_cap = 0);

		size_type capacity() const;

		void shrink_to_fit();

		void clear();

		this_type& insert(size_type p_index, size_type p_count, value_type p_ch);

		this_type& insert(size_type p_index, const value_type* p_str);

		this_type& insert(size_type p_index, const value_type* p_str, size_type p_count);

		this_type& insert(size_type p_index, const this_type& p_str);

		this_type& insert(size_type p_index, const this_type& p_str, size_type p_index_str, size_type p_count = npos);

		iterator_type insert(const_iterator_type p_pos, value_type p_ch);

		iterator_type insert(const_iterator_type p_pos, size_type count, value_type p_ch);

		template< class InputIterator >
		iterator_type insert(const_iterator_type p_pos, InputIterator p_first, InputIterator p_last);

		iterator_type insert(const_iterator_type p_pos, std::initializer_list<value_type> p_ilist);

		this_type& erase(size_type p_index = 0, size_type p_count = npos);

		iterator_type erase(const_iterator_type p_position);

		iterator_type erase(const_iterator_type p_first, const_iterator p_last);

		void push_back(value_type p_ch);

		void pop_back();

		this_type& append(size_type p_count, value_type p_ch);

		this_type& append(const this_type& p_str);

		this_type& append(const this_type& p_str, size_type p_pos, size_type p_count = npos);

		this_type& append(const value_type* p_str, size_type p_count);

		this_type& append(const value_type* p_str);

		template< class InputIt >
		this_type& append(InputIt p_first, InputIt p_last);

		this_type& append(std::initializer_list<value_type> p_ilist);

		this_type& operator+=(const this_type& p_str);

		this_type& operator+=(value_type p_ch);

		this_type& operator+=(const value_type* p_str);

		this_type& operator+=(std::initializer_list<value_type> p_ilist);

		int compare(const this_type& p_str) const;

		int compare(size_type p_pos1, size_type p_count1, const this_type& p_str) const;

		int compare(size_type p_pos1, size_type p_count1, const this_type& p_str, size_type p_pos2, size_type p_count2 = npos) const;

		int compare(const value_type* p_str) const;

		int compare(size_type p_pos1, size_type p_count1, const value_type* p_str) const;

		int compare(size_type p_pos1, size_type p_count1, const value_type* p_str, size_type p_count2) const;

		this_type& replace(size_type p_pos, size_type p_count, const this_type& p_str);

		this_type& replace(const_iterator_type p_first, const_iterator p_last, const this_type& p_str);

		this_type& replace(size_type p_pos, size_type p_count, const this_type& p_str, size_type p_pos2, size_type p_count2 = npos);

		template< class InputIt >
		this_type& replace(const_iterator_type p_first, const_iterator p_last, InputIt p_first2, InputIt p_last2);

		this_type& replace(size_type p_pos, size_type p_count, const value_type* p_cstr, size_type p_count2);

		this_type& replace(const_iterator_type p_first, const_iterator p_last, const value_type* p_cstr, size_type p_count2);

		this_type& replace(size_type p_pos, size_type p_count, const value_type* p_cstr);

		this_type& replace(const_iterator_type p_first, const_iterator p_last, const value_type* p_cstr);

		this_type& replace(size_type p_pos, size_type p_count, size_type p_count2, value_type p_ch);

		this_type& replace(const_iterator_type p_first, const_iterator p_last, size_type p_count2, value_type p_ch);

		this_type& replace(const_iterator_type p_first, const_iterator p_last, std::initializer_list<value_type> p_ilist);

		this_type substr(size_type p_pos = 0, size_type p_count = npos) const;

		size_type copy(value_type* p_dest, size_type p_count, size_type p_pos = 0) const;

		void resize(size_type p_count);

		void resize(size_type p_count, value_type p_ch);

		void swap(this_type& p_other);

		size_type find(const this_type& p_str, size_type p_pos = 0) const;

		size_type find(const value_type* p_str, size_type p_pos, size_type p_count) const;

		size_type find(const value_type* p_str, size_type p_pos = 0) const;

		size_type find(value_type p_ch, size_type p_pos = 0) const;

		size_type rfind(const this_type& p_str, size_type p_pos = npos) const;

		size_type rfind(const value_type* p_s, size_type p_pos, size_type p_count) const;

		size_type rfind(const value_type* p_s, size_type p_pos = npos) const;

		size_type rfind(value_type p_ch, size_type p_pos = npos) const;

		size_type find_first_of(const this_type& p_str, size_type p_pos = 0) const;

		size_type find_first_of(const value_type* p_s, size_type p_pos, size_type p_count) const;

		size_type find_first_of(const value_type* p_s, size_type p_pos = 0) const;

		size_type find_first_of(value_type p_ch, size_type p_pos = 0) const;

		size_type find_first_not_of(const this_type& p_str, size_type p_pos = 0) const;

		size_type find_first_not_of(const value_type* p_s, size_type p_pos, size_type p_count) const;

		size_type find_first_not_of(const value_type* p_s, size_type p_pos = 0) const;

		size_type find_first_not_of(value_type p_ch, size_type p_pos = 0) const;

		size_type find_last_of(const this_type& p_str, size_type p_pos = npos) const;

		size_type find_last_of(const value_type* p_s, size_type p_pos, size_type p_count) const;

		size_type find_last_of(const value_type* p_s, size_type p_pos = npos) const;

		size_type find_last_of(value_type p_ch, size_type p_pos = npos) const;

		size_type find_last_not_of(const this_type& p_str, size_type p_pos = npos) const;

		size_type find_last_not_of(const value_type* p_s, size_type p_pos, size_type p_count) const;

		size_type find_last_not_of(const value_type* p_s, size_type p_pos = npos) const;

		size_type find_last_not_of(value_type p_ch, size_type p_pos = npos) const;
		};
		*/

		template< typename TChar, typename TTraits, typename TAllocator >
		using bc_basic_string = std::basic_string<TChar, TTraits, TAllocator>;

		using bc_string = bc_basic_string< bcCHAR, std::char_traits< bcCHAR >, bc_allocator< bcCHAR > >;

		template<template<typename> typename TAllocator >
		using bc_string_a = bc_basic_string< bcCHAR, std::char_traits< bcCHAR >, TAllocator< bcCHAR > >;

		using bc_string_program = bc_string_a< bc_allocator_program >;

		using bc_string_frame = bc_string_a< bc_allocator_frame >;

		/*template<template<typename> typename TAllocator >
		using bc_string_movable = bc_string_a< bc_allocator_movable >;*/

		using bc_wstring = bc_basic_string< bcWCHAR, std::char_traits< bcWCHAR >, bc_allocator< bcECHAR > >;

		template<template<typename> typename TAllocator >
		using bc_wstring_a = bc_basic_string< bcWCHAR, std::char_traits< bcWCHAR >, TAllocator< bcECHAR > >;

		using bc_wstring_program = bc_wstring_a< bc_allocator_program >;

		using bc_wstring_frame = bc_wstring_a< bc_allocator_frame >;

		/*template<template<typename> typename TAllocator >
		using bc_wstring_movable = bc_wstring_a< bc_allocator_movable >;*/

#ifdef BC_UNICODE
		using bc_estring = bc_wstring;

		template<template<typename> typename TAllocator >
		using bc_estring_a = bc_wstring_a< TAllocator >;
#else
		using bc_estring = bc_string;

		template<template<typename> typename TAllocator >
		using bc_estring_a = bc_string_a<TAllocator>;
#endif

		using bc_estring_program = bc_estring_a< bc_allocator_program >;

		using bc_estring_frame = bc_estring_a< bc_allocator_frame >;
		
		/*
		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(const bc_basic_string< TChar, TTraits, TAllocator >& lhs, const bc_basic_string< TChar, TTraits, TAllocator >& rhs)
		{
			return std::operator+(lhs, rhs);
		}

		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(const TChar* lhs, const bc_basic_string< TChar, TTraits, TAllocator >& rhs)
		{
			return std::operator+(lhs, rhs);
		}

		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(TChar lhs, const bc_basic_string< TChar, TTraits, TAllocator >& rhs)
		{
			return std::operator+(lhs, rhs);
		}

		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(const bc_basic_string< TChar, TTraits, TAllocator >& lhs, const TChar* rhs)
		{
			return std::operator+(lhs, rhs);
		}

		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(const bc_basic_string< TChar, TTraits, TAllocator >& lhs, TChar rhs)
		{
			return std::operator+(lhs, rhs);
		}

		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(bc_basic_string< TChar, TTraits, TAllocator >&& lhs, const bc_basic_string< TChar, TTraits, TAllocator >& rhs)
		{
			return std::operator+(std::move(lhs), rhs);
		}

		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(const bc_basic_string< TChar, TTraits, TAllocator >& lhs, bc_basic_string< TChar, TTraits, TAllocator >&& rhs)
		{
			return std::operator+(lhs, std::move(rhs));
		}

		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(bc_basic_string< TChar, TTraits, TAllocator >&& lhs, bc_basic_string< TChar, TTraits, TAllocator >&& rhs)
		{
			return std::operator+(std::move(lhs), std::move(rhs));
		}

		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(const TChar* lhs, bc_basic_string< TChar, TTraits, TAllocator >&& rhs)
		{
			return std::operator+(lhs, std::move(rhs));
		}

		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(TChar lhs, bc_basic_string< TChar, TTraits, TAllocator >&& rhs)
		{
			return std::operator+(lhs, std::move(rhs));
		}

		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(bc_basic_string< TChar, TTraits, TAllocator >&& lhs, const TChar* rhs)
		{
			return std::operator+(std::move(lhs), rhs);
		}

		template< class TChar, class TTraits, class TAllocator >
		bc_basic_string< TChar, TTraits, TAllocator > operator+(bc_basic_string< TChar, TTraits, TAllocator >&& lhs, TChar rhs)
		{
			return std::operator+(std::move(lhs), rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator==(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator==(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator!=(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator!=(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator<(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator<(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator<=(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator<=(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator>(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator>(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator>=(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator>=(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator==(const TChar* lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator==(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator==(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const TChar* rhs)
		{
			return std::operator==(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator!=(const TChar* lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator!=(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator!=(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const TChar* rhs)
		{
			return std::operator!=(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator<(const TChar* lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator<(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator<(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const TChar* rhs)
		{
			return std::operator<(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator<=(const TChar* lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator<=(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator<=(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const TChar* rhs)
		{
			return std::operator<=(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator>(const TChar* lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator>(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator>(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const TChar* rhs)
		{
			return std::operator>(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator>=(const TChar* lhs, const bc_basic_string< TChar, TTrait, TAllocator >& rhs)
		{
			return std::operator>=(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bool operator>=(const bc_basic_string< TChar, TTrait, TAllocator >& lhs, const TChar* rhs)
		{
			return std::operator>=(lhs, rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		void swap(bc_basic_string<TChar, TTrait, TAllocator> &lhs, bc_basic_string<TChar, TTrait, TAllocator> &rhs)
		{
			return lhs.swap(rhs);
		}

		template< class TChar, class TTrait, class TAllocator >
		bc_basic_string< TChar, TTrait, TAllocator >& operator<<(std::basic_ostream< TChar, TTrait >& os, const bc_basic_string< TChar, TTrait, TAllocator >& str)
		{
			return std::operator<<(os, str);
		}

		template< class TChar, class TTrait, class TAllocator >
		bc_basic_string< TChar, TTrait, TAllocator >& operator>>(std::basic_istream< TChar, TTrait >& is, bc_basic_string< TChar, TTrait, TAllocator >& str)
		{
			return std::operator>>(is, str);
		}
		*/

		inline bcINT bc_stoi(const bc_string& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const bcCHAR* l_ptr = p_str.c_str();
			bcCHAR* l_eptr;
			errno = 0;
			long l_ans = std::strtol(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid stoi argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}
			
			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return static_cast<bcINT>(l_ans);
		}

		inline bcINT bc_stoi(const bc_wstring& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const bcWCHAR* l_ptr = p_str.c_str();
			bcWCHAR* l_eptr;
			errno = 0;
			long l_ans = std::wcstol(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid stoi argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return static_cast<bcINT>(l_ans);
		}

		inline long bc_stol(const bc_string& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const bcCHAR* l_ptr = p_str.c_str();
			bcCHAR* l_eptr;
			errno = 0;
			long l_ans = std::strtol(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid stol argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline long bc_stol(const bc_wstring& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const bcWCHAR* l_ptr = p_str.c_str();
			bcWCHAR* l_eptr;
			errno = 0;
			long l_ans = std::wcstol(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid stol argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline long long bc_stoll(const bc_string& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const bcCHAR* l_ptr = p_str.c_str();
			bcCHAR* l_eptr;
			errno = 0;
			long long l_ans = std::strtoll(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid stoll argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline long long bc_stoll(const bc_wstring& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const bcWCHAR* l_ptr = p_str.c_str();
			bcWCHAR* l_eptr;
			errno = 0;
			long long l_ans = std::wcstoll(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid stoll argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline unsigned long bc_stoul(const bc_string& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const bcCHAR* l_ptr = p_str.c_str();
			bcCHAR* l_eptr;
			errno = 0;
			unsigned long l_ans = std::strtoul(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid strtoul argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline unsigned long bc_stoul(const bc_wstring& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const bcWCHAR* l_ptr = p_str.c_str();
			bcWCHAR* l_eptr;
			errno = 0;
			unsigned long l_ans = std::wcstoul(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid wcstoul argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline unsigned long long bc_stoull(const bc_string& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const bcCHAR* l_ptr = p_str.c_str();
			bcCHAR* l_eptr;
			errno = 0;
			unsigned long long l_ans = std::strtoull(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid strtoull argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline unsigned long long bc_stoull(const bc_wstring& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const bcWCHAR* l_ptr = p_str.c_str();
			bcWCHAR* l_eptr;
			errno = 0;
			unsigned long long l_ans = std::wcstoull(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid wcstoull argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline bcFLOAT bc_stof(const bc_string& p_str, bcSIZE* p_pos = nullptr)
		{
			const bcCHAR* l_ptr = p_str.c_str();
			bcCHAR* l_eptr;
			errno = 0;
			bcFLOAT l_ans = std::strtof(l_ptr, &l_eptr);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid strtof argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline bcFLOAT bc_stof(const bc_wstring& p_str, bcSIZE* p_pos = nullptr)
		{
			const bcWCHAR* l_ptr = p_str.c_str();
			bcWCHAR* l_eptr;
			errno = 0;
			bcFLOAT l_ans = std::wcstof(l_ptr, &l_eptr);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid wcstof argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline bcDOUBLE bc_stod(const bc_string& p_str, bcSIZE* p_pos = nullptr)
		{
			const bcCHAR* l_ptr = p_str.c_str();
			bcCHAR* l_eptr;
			errno = 0;
			bcDOUBLE l_ans = std::strtod(l_ptr, &l_eptr);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid strtod argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline bcDOUBLE bc_stod(const bc_wstring& p_str, bcSIZE* p_pos = nullptr)
		{
			const bcWCHAR* l_ptr = p_str.c_str();
			bcWCHAR* l_eptr;
			errno = 0;
			bcDOUBLE l_ans = std::wcstod(l_ptr, &l_eptr);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid wcstod argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline long double bc_stold(const bc_string& p_str, bcSIZE* p_pos = nullptr)
		{
			const bcCHAR* l_ptr = p_str.c_str();
			bcCHAR* l_eptr;
			errno = 0;
			long double l_ans = std::strtold(l_ptr, &l_eptr);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid strtold argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

		inline long double bc_stold(const bc_wstring& p_str, bcSIZE* p_pos = nullptr)
		{
			const bcWCHAR* l_ptr = p_str.c_str();
			bcWCHAR* l_eptr;
			errno = 0;
			long double l_ans = std::wcstold(l_ptr, &l_eptr);

			if (l_ptr == l_eptr)
			{
				throw std::invalid_argument("invalid wcstold argument");
			}
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
			{
				throw std::out_of_range("stoi argument out of range");
			}

			if (p_pos != nullptr)
			{
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);
			}

			return l_ans;
		}

#define TO_STRING_BUFF (2 * 32)
		
		inline void _bc_to_string(bcINT p_value, bcCHAR* p_buf)
		{
			std::snprintf(p_buf, TO_STRING_BUFF, "%d", p_value);
		}

		inline void _bc_to_string(long p_value, bcCHAR* p_buf)
		{
			std::snprintf(p_buf, TO_STRING_BUFF, "%ld", p_value);
		}

		inline void _bc_to_string(long long p_value, bcCHAR* p_buf)
		{
			std::snprintf(p_buf, TO_STRING_BUFF, "%lld", p_value);
		}

		inline void _bc_to_string(bcUINT p_value, bcCHAR* p_buf)
		{
			std::snprintf(p_buf, TO_STRING_BUFF, "%u", p_value);
		}

		inline void _bc_to_string(unsigned long p_value, bcCHAR* p_buf)
		{
			std::snprintf(p_buf, TO_STRING_BUFF, "%lu", p_value);
		}

		inline void _bc_to_string(unsigned long long p_value, bcCHAR* p_buf)
		{
			std::snprintf(p_buf, TO_STRING_BUFF, "%llu", p_value);
		}

		inline void _bc_to_string(bcFLOAT p_value, bcCHAR* p_buf)
		{
			std::snprintf(p_buf, TO_STRING_BUFF, "%f", p_value);
		}

		inline void _bc_to_string(bcDOUBLE p_value, bcCHAR* p_buf)
		{
			std::snprintf(p_buf, TO_STRING_BUFF, "%f", p_value);
		}

		inline void _bc_to_string(long double p_value, bcCHAR* p_buf)
		{
			std::snprintf(p_buf, TO_STRING_BUFF, "%Lf", p_value);
		}

		inline void _bc_to_string(const bcWCHAR* p_src, bcCHAR* p_dest, bcSIZE p_len)
		{
			std::mbstate_t l_state = std::mbstate_t();
			std::wcsrtombs(p_dest, &p_src, p_len, &l_state);
		};

		template< template< typename > typename TInputAllocator, template< typename > typename TOutputAllocator >
		inline bc_string_a<TOutputAllocator> _bc_to_string(const bc_wstring_a<TInputAllocator>& p_str)
		{
			typename bc_string_a<TOutputAllocator>::size_type l_len = p_str.size();
			bc_string_a<TOutputAllocator> l_str(l_len, '#');

			bcCHAR* l_dest = &l_str[0];
			const bcWCHAR* l_src = &p_str[0];

			_bc_to_string(l_src, l_dest, l_len);

			return std::move(l_str);
		};

		inline bc_string bc_to_string(bcINT p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(long long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(bcUINT p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(unsigned long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(unsigned long long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(bcFLOAT p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(bcDOUBLE p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(long double p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(const bcWCHAR* p_str)
		{
			auto l_str_len = std::wcslen(p_str);
			bc_string l_str(l_str_len, bcL('#'));

			_bc_to_string(p_str, &l_str.at(0), l_str_len);

			return l_str;
		}

		inline bc_string bc_to_string(const bc_wstring& p_str)
		{
			return _bc_to_string<bc_allocator, bc_allocator>(p_str);
		}

		inline bc_string bc_to_string(const bc_wstring_program& p_str)
		{
			return _bc_to_string<bc_allocator_program, bc_allocator>(p_str);
		}

		inline bc_string bc_to_string(const bc_wstring_frame& p_str)
		{
			return _bc_to_string<bc_allocator_frame, bc_allocator>(p_str);
		}

		inline bc_string bc_to_string(const bcCHAR* p_str)
		{
			return bc_string(p_str);
		}

		inline bc_string bc_to_string(const bc_string& p_str)
		{
			return p_str;
		}

		inline bc_string bc_to_string(const bc_string_program& p_str)
		{
			return bc_string(p_str.c_str());
		}

		inline bc_string bc_to_string(const bc_string_frame& p_str)
		{
			return bc_string(p_str.c_str());
		}

		inline bc_string_program bc_to_string_program(bcINT p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_program(l_buf);
		}

		inline bc_string_program bc_to_string_program(long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_program(l_buf);
		}

		inline bc_string_program bc_to_string_program(long long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_program(l_buf);
		}

		inline bc_string_program bc_to_string_program(bcUINT p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_program(l_buf);
		}

		inline bc_string_program bc_to_string_program(unsigned long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_program(l_buf);
		}

		inline bc_string_program bc_to_string_program(unsigned long long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_program(l_buf);
		}

		inline bc_string_program bc_to_string_program(bcFLOAT p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_program(l_buf);
		}

		inline bc_string_program bc_to_string_program(bcDOUBLE p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_program(l_buf);
		}

		inline bc_string_program bc_to_string_program(const bcWCHAR* p_str)
		{
			auto l_str_len = std::wcslen(p_str);
			bc_string_program l_str(l_str_len, bcL('#'));

			_bc_to_string(p_str, &l_str.at(0), l_str_len);

			return l_str;
		}

		inline bc_string_program bc_to_string_program(const bc_wstring& p_str)
		{
			return _bc_to_string<bc_allocator, bc_allocator_program>(p_str);
		}

		inline bc_string_program bc_to_string_program(const bc_wstring_program& p_str)
		{
			return _bc_to_string<bc_allocator_program, bc_allocator_program>(p_str);
		}

		inline bc_string_program bc_to_string_program(const bc_wstring_frame& p_str)
		{
			return _bc_to_string<bc_allocator_frame, bc_allocator_program>(p_str);
		}

		inline bc_string_program bc_to_string_program(const bcCHAR* p_str)
		{
			return bc_string_program(p_str);
		}

		inline bc_string_program bc_to_string_program(const bc_string& p_str)
		{
			return bc_string_program(p_str.c_str());
		}

		inline bc_string_program bc_to_string_program(const bc_string_program& p_str)
		{
			return p_str;
		}

		inline bc_string_program bc_to_string_program(const bc_string_frame& p_str)
		{
			return bc_string_program(p_str.c_str());
		}

		inline bc_string_program bc_to_string_program(long double p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_program(l_buf);
		}

		inline bc_string_frame bc_to_string_frame(bcINT p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_frame(l_buf);
		}

		inline bc_string_frame bc_to_string_frame(long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_frame(l_buf);
		}

		inline bc_string_frame bc_to_string_frame(long long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_frame(l_buf);
		}

		inline bc_string_frame bc_to_string_frame(bcUINT p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_frame(l_buf);
		}

		inline bc_string_frame bc_to_string_frame(unsigned long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_frame(l_buf);
		}

		inline bc_string_frame bc_to_string_frame(unsigned long long p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_frame(l_buf);
		}

		inline bc_string_frame bc_to_string_frame(bcFLOAT p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_frame(l_buf);
		}

		inline bc_string_frame bc_to_string_frame(bcDOUBLE p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_frame(l_buf);
		}

		inline bc_string_frame bc_to_string_frame(long double p_value)
		{
			bcCHAR l_buf[TO_STRING_BUFF];

			_bc_to_string(p_value, l_buf);

			return bc_string_frame(l_buf);
		}

		inline bc_string_frame bc_to_string_frame(const bcWCHAR* p_str)
		{
			auto l_str_len = std::wcslen(p_str);
			bc_string_frame l_str(l_str_len, bcL('#'));

			_bc_to_string(p_str, &l_str.at(0), l_str_len);

			return l_str;
		}

		inline bc_string_frame bc_to_string_frame(const bc_wstring& p_str)
		{
			return _bc_to_string<bc_allocator, bc_allocator_frame>(p_str);
		}

		inline bc_string_frame bc_to_string_frame(const bc_wstring_program& p_str)
		{
			return _bc_to_string<bc_allocator_program, bc_allocator_frame>(p_str);
		}

		inline bc_string_frame bc_to_string_frame(const bc_wstring_frame& p_str)
		{
			return _bc_to_string<bc_allocator_frame, bc_allocator_frame>(p_str);
		}

		inline bc_string_frame bc_to_string_frame(const bcCHAR* p_str)
		{
			return bc_string_frame(p_str);
		}

		inline bc_string_frame bc_to_string_frame(const bc_string& p_str)
		{
			return bc_string_frame(p_str.c_str());
		}

		inline bc_string_frame bc_to_string_frame(const bc_string_program& p_str)
		{
			return bc_string_frame(p_str.c_str());
		}

		inline bc_string_frame bc_to_string_frame(const bc_string_frame& p_str)
		{
			return p_str;
		}

		inline void _bc_to_wstring(int p_value, bcWCHAR* p_buf)
		{
			std::swprintf(p_buf, TO_STRING_BUFF, L"%d", p_value);
		}

		inline void _bc_to_wstring(long p_value, bcWCHAR* p_buf)
		{
			std::swprintf(p_buf, TO_STRING_BUFF, L"%ld", p_value);
		}

		inline void _bc_to_wstring(long long p_value, bcWCHAR* p_buf)
		{
			std::swprintf(p_buf, TO_STRING_BUFF, L"%lld", p_value);
		}

		inline void _bc_to_wstring(unsigned p_value, bcWCHAR* p_buf)
		{
			std::swprintf(p_buf, TO_STRING_BUFF, L"%u", p_value);
		}

		inline void _bc_to_wstring(unsigned long p_value, bcWCHAR* p_buf)
		{
			std::swprintf(p_buf, TO_STRING_BUFF, L"%lu", p_value);
		}

		inline void _bc_to_wstring(unsigned long long p_value, bcWCHAR* p_buf)
		{
			std::swprintf(p_buf, TO_STRING_BUFF, L"%llu", p_value);
		}

		inline void _bc_to_wstring(float p_value, bcWCHAR* p_buf)
		{
			std::swprintf(p_buf, TO_STRING_BUFF, L"%f", p_value);
		}

		inline void _bc_to_wstring(double p_value, bcWCHAR* p_buf)
		{
			std::swprintf(p_buf, TO_STRING_BUFF, L"%f", p_value);
		}

		inline void _bc_to_wstring(long double p_value, bcWCHAR* p_buf)
		{
			std::swprintf(p_buf, TO_STRING_BUFF, L"%Lf", p_value);
		}

		inline void _bc_to_wstring(const bcCHAR* p_src, bcWCHAR* p_dest, bcSIZE p_len)
		{
			std::mbstate_t l_state = std::mbstate_t();
			std::mbsrtowcs(p_dest, &p_src, p_len, &l_state);
		};

		template< template<typename> typename TInputAllocator, template<typename> typename TOutputAllocator >
		inline bc_wstring_a<TOutputAllocator> _bc_to_wstring(const bc_string_a<TInputAllocator>& p_str)
		{
			typename bc_wstring_a<TOutputAllocator>::size_type l_len = p_str.size();
			bc_wstring_a<TOutputAllocator> l_str(l_len, '#');

			bcWCHAR* l_dest = &l_str[0];
			const bcCHAR* l_src = &p_str[0];

			_bc_to_wstring(l_src, l_dest, l_len);

			return l_str;
		};

		inline bc_wstring bc_to_wstring(bcINT p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(long long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(bcUINT p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(unsigned long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(unsigned long long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(bcFLOAT p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(bcDOUBLE p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(long double p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(const bcCHAR* p_str)
		{
			auto l_str_len = std::strlen(p_str);
			bc_wstring l_str(l_str_len, bcL('#'));

			_bc_to_wstring(p_str, &l_str.at(0), l_str_len);

			return l_str;
		}

		inline bc_wstring bc_to_wstring(const bc_string& p_str)
		{
			return _bc_to_wstring<bc_allocator, bc_allocator>(p_str);
		}

		inline bc_wstring bc_to_wstring(const bc_string_program& p_str)
		{
			return _bc_to_wstring<bc_allocator_program, bc_allocator>(p_str);
		}

		inline bc_wstring bc_to_wstring(const bc_string_frame& p_str)
		{
			return _bc_to_wstring<bc_allocator_frame, bc_allocator>(p_str);
		}

		inline bc_wstring bc_to_wstring(const bcWCHAR* p_str)
		{
			return bc_wstring(p_str);
		}

		inline bc_wstring bc_to_wstring(const bc_wstring& p_str)
		{
			return p_str;
		}

		inline bc_wstring bc_to_wstring(const bc_wstring_program& p_str)
		{
			return bc_wstring(p_str.c_str());
		}

		inline bc_wstring bc_to_wstring(const bc_wstring_frame& p_str)
		{
			return bc_wstring(p_str.c_str());
		}

		inline bc_wstring_program bc_to_wstring_program(bcINT p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_program(l_buf);
		}

		inline bc_wstring_program bc_to_wstring_program(long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_program(l_buf);
		}

		inline bc_wstring_program bc_to_wstring_program(long long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_program(l_buf);
		}

		inline bc_wstring_program bc_to_wstring_program(bcUINT p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_program(l_buf);
		}

		inline bc_wstring_program bc_to_wstring_program(unsigned long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_program(l_buf);
		}

		inline bc_wstring_program bc_to_wstring_program(unsigned long long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_program(l_buf);
		}

		inline bc_wstring_program bc_to_wstring_program(bcFLOAT p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_program(l_buf);
		}

		inline bc_wstring_program bc_to_wstring_program(bcDOUBLE p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_program(l_buf);
		}

		inline bc_wstring_program bc_to_wstring_program(long double p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_program(l_buf);
		}

		inline bc_wstring_program bc_to_wstring_program(const bcCHAR* p_str)
		{
			auto l_str_len = std::strlen(p_str);
			bc_wstring_program l_str(l_str_len, bcL('#'));

			_bc_to_wstring(p_str, &l_str.at(0), l_str_len);

			return l_str;
		}

		inline bc_wstring_program bc_to_wstring_program(const bc_string& p_str)
		{
			return _bc_to_wstring<bc_allocator, bc_allocator_program>(p_str);
		}

		inline bc_wstring_program bc_to_wstring_program(const bc_string_program& p_str)
		{
			return _bc_to_wstring<bc_allocator_program, bc_allocator_program>(p_str);
		}

		inline bc_wstring_program bc_to_wstring_program(const bc_string_frame& p_str)
		{
			return _bc_to_wstring<bc_allocator_frame, bc_allocator_program>(p_str);
		}

		inline bc_wstring_program bc_to_wstring_program(const bcWCHAR* p_str)
		{
			return bc_wstring_program(p_str);
		}

		inline bc_wstring_program bc_to_wstring_program(const bc_wstring& p_str)
		{
			return bc_wstring_program(p_str.c_str());
		}

		inline bc_wstring_program bc_to_wstring_program(const bc_wstring_program& p_str)
		{
			return p_str;
		}

		inline bc_wstring_program bc_to_wstring_program(const bc_wstring_frame& p_str)
		{
			return bc_wstring_program(p_str.c_str());
		}

		inline bc_wstring_frame bc_to_wstring_frame(bcINT p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_frame(l_buf);
		}

		inline bc_wstring_frame bc_to_wstring_frame(long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_frame(l_buf);
		}

		inline bc_wstring_frame bc_to_wstring_frame(long long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_frame(l_buf);
		}

		inline bc_wstring_frame bc_to_wstring_frame(bcUINT p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_frame(l_buf);
		}

		inline bc_wstring_frame bc_to_wstring_frame(unsigned long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_frame(l_buf);
		}

		inline bc_wstring_frame bc_to_wstring_frame(unsigned long long p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_frame(l_buf);
		}

		inline bc_wstring_frame bc_to_wstring_frame(bcFLOAT p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_frame(l_buf);
		}

		inline bc_wstring_frame bc_to_wstring_frame(bcDOUBLE p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_frame(l_buf);
		}

		inline bc_wstring_frame bc_to_wstring_frame(long double p_value)
		{
			bcWCHAR l_buf[TO_STRING_BUFF];

			_bc_to_wstring(p_value, l_buf);

			return bc_wstring_frame(l_buf);
		}

		inline bc_wstring_frame bc_to_wstring_frame(const bcCHAR* p_str)
		{
			auto l_str_len = std::strlen(p_str);
			bc_wstring_frame l_str(l_str_len, bcL('#'));

			_bc_to_wstring(p_str, &l_str.at(0), l_str_len);

			return l_str;
		}

		inline bc_wstring_frame bc_to_wstring_frame(const bc_string& p_str)
		{
			return _bc_to_wstring<bc_allocator, bc_allocator_frame>(p_str);
		}

		inline bc_wstring_frame bc_to_wstring_frame(const bc_string_program& p_str)
		{
			return _bc_to_wstring<bc_allocator_program, bc_allocator_frame>(p_str);
		}

		inline bc_wstring_frame bc_to_wstring_frame(const bc_string_frame& p_str)
		{
			return _bc_to_wstring<bc_allocator_frame, bc_allocator_frame>(p_str);
		}

		inline bc_wstring_frame bc_to_wstring_frame(const bcWCHAR* p_str)
		{
			return bc_wstring_frame(p_str);
		}

		inline bc_wstring_frame bc_to_wstring_frame(const bc_wstring& p_str)
		{
			return bc_wstring_frame(p_str.c_str());
		}

		inline bc_wstring_frame bc_to_wstring_frame(const bc_wstring_program& p_str)
		{
			return bc_wstring_frame(p_str.c_str());
		}

		inline bc_wstring_frame bc_to_wstring_frame(const bc_wstring_frame& p_str)
		{
			return p_str;
		}

		template< typename T >
		inline bc_estring bc_to_estring(T&& p_value)
		{
#ifdef BC_UNICODE
			return bc_to_wstring(std::forward<T>(p_value));
#else
			return bc_to_string(std::forward<T>(p_value));
#endif
		}

		template< typename T >
		inline bc_estring_program bc_to_estring_program(T&& p_value)
		{
#ifdef BC_UNICODE
			return bc_to_wstring_program(std::forward<T>(p_value));
#else
			return bc_to_string_program(std::forward<T>(p_value));
#endif
		}

		template< typename T >
		inline bc_estring_frame bc_to_estring_frame(T&& p_value)
		{
#ifdef BC_UNICODE
			return bc_to_wstring_frame(std::forward<T>(p_value));
#else
			return bc_to_string_frame(std::forward<T>(p_value));
#endif
		}

		inline bc_string bc_to_exclusive_string(const bcCHAR* p_value)
		{
			return bc_string(p_value);
		}

		inline bc_string bc_to_exclusive_string(const bcWCHAR* p_value)
		{
			auto l_str_len = std::wcslen(p_value);
			bc_string l_str(l_str_len, '#');

			_bc_to_string(p_value, &l_str.at(0), l_str_len);

			return l_str;
		}

		inline bc_string& bc_to_exclusive_string(const bc_string& p_value)
		{
			return const_cast< bc_string& >(p_value);
		}

		inline bc_string bc_to_exclusive_string(const bc_wstring& p_value)
		{
			return bc_to_string(p_value);
		}

		inline bc_wstring bc_to_exclusive_wstring(const bcCHAR* p_value)
		{
			auto l_str_len = std::strlen(p_value);
			bc_wstring l_str(l_str_len, bcL('#'));

			_bc_to_wstring(p_value, &l_str.at(0), l_str_len);

			return l_str;
		}

		inline bc_wstring bc_to_exclusive_wstring(const bcWCHAR* p_value)
		{
			return bc_wstring(p_value);
		}

		inline bc_wstring bc_to_exclusive_wstring(const bc_string& p_value)
		{
			return bc_to_wstring(p_value);
		}

		inline bc_wstring& bc_to_exclusive_wstring(const bc_wstring& p_value)
		{
			return const_cast< bc_wstring& >(p_value);
		}

#undef TO_STRING_BUFF

		inline bcSIZE bc_string_hash(const bcCHAR* p_string)
		{
			bcUINT32 l_hash = 0;
			bcUINT32 l_length = std::strlen(p_string);

			for (bcUINT32 i = 0; i < l_length; i++)
			{
				l_hash += ((i + 1) * static_cast< bcUINT32 >(p_string[i]));
			}

			return l_hash;
		}

		inline bcSIZE bc_wstring_hash(const bcWCHAR* p_string)
		{
			bcUINT32 l_hash = 0;
			bcUINT32 l_length = std::wcslen(p_string);

			for (bcUINT32 i = 0; i < l_length; i++)
			{
				l_hash += ((i + 1) * static_cast< bcUINT32 >(p_string[i]));
			}

			return l_hash;
		}
		
		// CRC32 Table (zlib polynomial)
		static constexpr bcUINT32 _crc_table[256] = {
			0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
			0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
			0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
			0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
			0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
			0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
			0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
			0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
			0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
			0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
			0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
			0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
			0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
			0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
			0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
			0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,

			0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
			0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
			0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
			0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
			0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
			0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
			0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
			0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
			0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
			0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
			0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
			0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
			0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
			0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
			0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
			0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,

			0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
			0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
			0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
			0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
			0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
			0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
			0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
			0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
			0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
			0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
			0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
			0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
			0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
			0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
			0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
			0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,

			0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
			0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
			0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
			0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
			0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
			0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
			0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
			0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
			0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
			0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
			0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
			0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
			0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
			0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
			0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
			0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
		};

		// http://stackoverflow.com/questions/2111667/compile-time-string-hashing
		template< bcSIZE TIdx >
		constexpr bcUINT32 _str_crc32(const bcCHAR* p_str)
		{
			return (_str_crc32< TIdx - 1 >(p_str) >> 8) ^ _crc_table[(_str_crc32< TIdx - 1 >(p_str) ^ p_str[TIdx]) & 0x000000FF];
		}

		template<>
		constexpr bcUINT32 _str_crc32< bcSIZE(-1) >(const bcCHAR* p_str)
		{
			return 0xFFFFFFFF;
		}

		inline bcUINT32 __str_crc32(const bcCHAR* p_str, bcSIZE p_idx)
		{
			if(p_idx == -1)
			{
				return 0xFFFFFFFF;
			}

			return (__str_crc32(p_str, p_idx - 1) >> 8) ^ _crc_table[(__str_crc32(p_str, p_idx - 1) ^ p_str[p_idx]) & 0x000000FF];
		}

		// This doesn't take into account the nul char
#define bc_compile_time_string_hash(str) (black_cat::core::_str_crc32<sizeof(str) - 2>(str) ^ 0xFFFFFFFF)
#define bc_run_time_string_hash(str, length) (black_cat::core::__str_crc32(str, length - 1) ^ 0xFFFFFFFF)
	}
}

namespace std
{
	template<>
	struct hash< const black_cat::bcCHAR* >
	{
	public:
		using argument_type = const black_cat::bcCHAR*;
		using result_type = std::size_t;

		result_type operator ()(const argument_type& p_arg) const
		{
			return black_cat::core::bc_string_hash(p_arg);
		}
	};

	template<>
	struct hash< const black_cat::bcWCHAR* >
	{
	public:
		using argument_type = const black_cat::bcWCHAR*;
		using result_type = std::size_t;

		result_type operator ()(const argument_type& p_arg) const
		{
			return black_cat::core::bc_wstring_hash(p_arg);
		}
	};

	template<>
	struct hash< black_cat::core::bc_string >
	{
	public:
		using argument_type = black_cat::core::bc_string;
		using result_type = std::size_t;

		result_type operator ()(const argument_type& p_arg) const
		{
			return black_cat::core::bc_string_hash(p_arg.c_str());
		}
	};

	template<>
	struct hash< black_cat::core::bc_string_program >
	{
	public:
		using argument_type = black_cat::core::bc_string_program;
		using result_type = std::size_t;

		result_type operator ()(const argument_type& p_arg) const
		{
			return black_cat::core::bc_string_hash(p_arg.c_str());
		}
	};

	template<>
	struct hash< black_cat::core::bc_string_frame >
	{
	public:
		using argument_type = black_cat::core::bc_string_frame;
		using result_type = std::size_t;

		result_type operator ()(const argument_type& p_arg) const
		{
			return black_cat::core::bc_string_hash(p_arg.c_str());
		}
	};

	template<>
	struct hash< black_cat::core::bc_wstring >
	{
		using argument_type = black_cat::core::bc_wstring;
		using result_type = std::size_t;

		result_type operator ()(const argument_type& p_arg) const
		{
			return black_cat::core::bc_wstring_hash(p_arg.c_str());
		}
	};

	template<>
	struct hash< black_cat::core::bc_wstring_program >
	{
	public:
		using argument_type = black_cat::core::bc_wstring_program;
		using result_type = std::size_t;

		result_type operator ()(const argument_type& p_arg) const
		{
			return black_cat::core::bc_wstring_hash(p_arg.c_str());
		}
	};

	template<>
	struct hash< black_cat::core::bc_wstring_frame >
	{
	public:
		using argument_type = black_cat::core::bc_wstring_frame;
		using result_type = std::size_t;

		result_type operator ()(const argument_type& p_arg) const
		{
			return black_cat::core::bc_wstring_hash(p_arg.c_str());
		}
	};
}