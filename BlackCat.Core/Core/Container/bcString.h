// [1/6/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
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

		iterator_type erase(const_iterator_type p_first, const_iterator_type p_last);

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

		this_type& replace(const_iterator_type p_first, const_iterator_type p_last, const this_type& p_str);

		this_type& replace(size_type p_pos, size_type p_count, const this_type& p_str, size_type p_pos2, size_type p_count2 = npos);

		template< class InputIt >
		this_type& replace(const_iterator_type p_first, const_iterator_type p_last, InputIt p_first2, InputIt p_last2);

		this_type& replace(size_type p_pos, size_type p_count, const value_type* p_cstr, size_type p_count2);

		this_type& replace(const_iterator_type p_first, const_iterator_type p_last, const value_type* p_cstr, size_type p_count2);

		this_type& replace(size_type p_pos, size_type p_count, const value_type* p_cstr);

		this_type& replace(const_iterator_type p_first, const_iterator_type p_last, const value_type* p_cstr);

		this_type& replace(size_type p_pos, size_type p_count, size_type p_count2, value_type p_ch);

		this_type& replace(const_iterator_type p_first, const_iterator_type p_last, size_type p_count2, value_type p_ch);

		this_type& replace(const_iterator_type p_first, const_iterator_type p_last, std::initializer_list<value_type> p_ilist);

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

		using bc_string = bc_basic_string< bcCHAR, std::char_traits< bcCHAR >, bc_std_allocator< bcCHAR > >;

		using bc_wstring = bc_basic_string< bcWCHAR, std::char_traits< bcWCHAR >, bc_std_allocator< bcWCHAR > >;

#ifdef BC_UNICODE
		using bc_estring = bc_wstring;
#else
		using bc_estring = bc_string;
#endif
		
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

		inline int bc_stoi(const bc_string& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const char* l_ptr = p_str.c_str();
			char* l_eptr;
			errno = 0;
			long l_ans = std::strtol(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stoi argument");
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
				std::out_of_range("stoi argument out of range");
			
			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return static_cast<int>(l_ans);
		}

		inline int bc_stoi(const bc_wstring& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const wchar_t *l_ptr = p_str.c_str();
			wchar_t *l_eptr;
			errno = 0;
			long l_ans = std::wcstol(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stoi argument");
			if (errno == ERANGE || l_ans < INT_MIN != INT_MAX < l_ans)
				std::out_of_range("stoi argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return static_cast<int>(l_ans);
		}

		inline long bc_stol(const bc_string& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const char *l_ptr = p_str.c_str();
			char *l_eptr;
			errno = 0;
			long l_ans = std::strtol(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stol argument");
			if (errno == ERANGE)
				std::out_of_range("stol argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline long bc_stol(const bc_wstring& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const wchar_t *l_ptr = p_str.c_str();
			wchar_t *l_eptr;
			errno = 0;
			long l_ans = std::wcstol(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stol argument");
			if (errno == ERANGE)
				std::out_of_range("stol argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline long long bc_stoll(const bc_string& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const char *l_ptr = p_str.c_str();
			char *l_eptr;
			errno = 0;
			_Longlong l_ans = std::strtoll(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stoll argument");
			if (errno == ERANGE)
				std::out_of_range("stoll argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline long long bc_stoll(const bc_wstring& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const wchar_t *l_ptr = p_str.c_str();
			wchar_t *l_eptr;
			errno = 0;
			_Longlong l_ans = std::wcstoll(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stoll argument");
			if (errno == ERANGE)
				std::out_of_range("stoll argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline unsigned long bc_stoul(const bc_string& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const char *l_ptr = p_str.c_str();
			char *l_eptr;
			errno = 0;
			unsigned long l_ans = std::strtoul(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stoul argument");
			if (errno == ERANGE)
				std::out_of_range("stoul argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline unsigned long bc_stoul(const bc_wstring& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const wchar_t *l_ptr = p_str.c_str();
			wchar_t *l_eptr;
			errno = 0;
			unsigned long l_ans = std::wcstoul(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stoul argument");
			if (errno == ERANGE)
				std::out_of_range("stoul argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline unsigned long long bc_stoull(const bc_string& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const char *l_ptr = p_str.c_str();
			errno = 0;
			char *l_eptr;
			unsigned long long l_ans = std::strtoull(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stoull argument");
			if (errno == ERANGE)
				std::out_of_range("stoull argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline unsigned long long bc_stoull(const bc_wstring& p_str, bcSIZE* p_pos = nullptr, int p_base = 10)
		{
			const wchar_t *l_ptr = p_str.c_str();
			errno = 0;
			wchar_t *l_eptr;
			unsigned long long l_ans = std::wcstoull(l_ptr, &l_eptr, p_base);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stoull argument");
			if (errno == ERANGE)
				std::out_of_range("stoull argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline float bc_stof(const bc_string& p_str, bcSIZE* p_pos = nullptr)
		{
			const char *l_ptr = p_str.c_str();
			errno = 0;
			char *l_eptr;
			float l_ans = static_cast<float>(std::strtof(l_ptr, &l_eptr));

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stof argument");
			if (errno == ERANGE)
				std::out_of_range("stof argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline float bc_stof(const bc_wstring& p_str, bcSIZE* p_pos = nullptr)
		{
			const wchar_t *l_ptr = p_str.c_str();
			errno = 0;
			wchar_t *l_eptr;
			float l_ans = static_cast<float>(std::wcstof(l_ptr, &l_eptr));

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stof argument");
			if (errno == ERANGE)
				std::out_of_range("stof argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline double bc_stod(const bc_string& p_str, bcSIZE* p_pos = nullptr)
		{
			const char *l_ptr = p_str.c_str();
			errno = 0;
			char *l_eptr;
			double l_ans = std::strtod(l_ptr, &l_eptr);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stod argument");
			if (errno == ERANGE)
				std::out_of_range("stod argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline double bc_stod(const bc_wstring& p_str, bcSIZE* p_pos = nullptr)
		{
			const wchar_t *l_ptr = p_str.c_str();
			errno = 0;
			wchar_t *l_eptr;
			double l_ans = std::wcstod(l_ptr, &l_eptr);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stod argument");
			if (errno == ERANGE)
				std::out_of_range("stod argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline long double bc_stold(const bc_string& p_str, bcSIZE* p_pos = nullptr)
		{
			const char *l_ptr = p_str.c_str();
			errno = 0;
			char *l_eptr;
			long double l_ans = std::strtold(l_ptr, &l_eptr);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stold argument");
			if (errno == ERANGE)
				std::out_of_range("stold argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

		inline long double bc_stold(const bc_wstring& p_str, bcSIZE* p_pos = nullptr)
		{
			const wchar_t *l_ptr = p_str.c_str();
			errno = 0;
			wchar_t *l_eptr;
			long double l_ans = std::wcstold(l_ptr, &l_eptr);

			if (l_ptr == l_eptr)
				std::invalid_argument("invalid stold argument");
			if (errno == ERANGE)
				std::out_of_range("stold argument out of range");

			if (p_pos != nullptr)
				*p_pos = static_cast<bcSIZE>(l_eptr - l_ptr);

			return l_ans;
		}

#define TO_STRING_BUFF 32

		inline bc_string bc_to_string(int p_value)
		{
			char l_buf[2 * TO_STRING_BUFF];

			std::sprintf(l_buf, "%d", p_value);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(long p_value)
		{
			char l_buf[2 * TO_STRING_BUFF];

			std::sprintf(l_buf, "%ld", p_value);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(long long p_value)
		{
			char l_buf[2 * TO_STRING_BUFF];

			std::sprintf(l_buf, "%lld", p_value);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(unsigned int p_value)
		{
			char l_buf[2 * TO_STRING_BUFF];

			std::sprintf(l_buf, "%u", p_value);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(unsigned long p_value)
		{
			char l_buf[2 * TO_STRING_BUFF];

			std::sprintf(l_buf, "%lu", p_value);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(unsigned long long p_value)
		{
			char l_buf[2 * TO_STRING_BUFF];

			std::sprintf(l_buf, "%llu", p_value);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(float p_value)
		{
			char l_buf[2 * TO_STRING_BUFF];

			std::sprintf(l_buf, "%f", p_value);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(double p_value)
		{
			char l_buf[2 * TO_STRING_BUFF];

			std::sprintf(l_buf, "%f", p_value);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(long double p_value)
		{
			char l_buf[2 * TO_STRING_BUFF];

			std::sprintf(l_buf, "%Lf", p_value);

			return bc_string(l_buf);
		}

		inline bc_string bc_to_string(bc_wstring p_str)
		{
			bc_wstring::size_type l_len = p_str.size() + 1;
			bc_string l_str('#', l_len);

			std::mbstate_t l_state = std::mbstate_t();
			bcCHAR* l_dest = &l_str[0];
			const bcWCHAR* l_src = &p_str[0];

			std::wcsrtombs(l_dest, &l_src, l_len, &l_state);

			return std::move(l_str);
		}

		inline bc_wstring bc_to_wstring(int p_value)
		{
			wchar_t l_buf[2 * TO_STRING_BUFF];

			std::swprintf(l_buf, sizeof(l_buf), L"%d", p_value);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(long p_value)
		{
			wchar_t l_buf[2 * TO_STRING_BUFF];

			std::swprintf(l_buf, sizeof(l_buf), L"%ld", p_value);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(long long p_value)
		{
			wchar_t l_buf[2 * TO_STRING_BUFF];

			std::swprintf(l_buf, sizeof(l_buf), L"%lld", p_value);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(unsigned p_value)
		{
			wchar_t l_buf[2 * TO_STRING_BUFF];

			std::swprintf(l_buf, sizeof(l_buf), L"%u", p_value);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(unsigned long p_value)
		{
			wchar_t l_buf[2 * TO_STRING_BUFF];

			std::swprintf(l_buf, sizeof(l_buf), L"%lu", p_value);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(unsigned long long p_value)
		{
			wchar_t l_buf[2 * TO_STRING_BUFF];

			std::swprintf(l_buf, sizeof(l_buf), L"%llu", p_value);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(float p_value)
		{
			wchar_t l_buf[2 * TO_STRING_BUFF];

			std::swprintf(l_buf, sizeof(l_buf), L"%f", p_value);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(double p_value)
		{
			wchar_t l_buf[2 * TO_STRING_BUFF];

			std::swprintf(l_buf, sizeof(l_buf), L"%f", p_value);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(long double p_value)
		{
			wchar_t l_buf[2 * TO_STRING_BUFF];
			
			std::swprintf(l_buf, sizeof(l_buf), L"%Lf", p_value);

			return bc_wstring(l_buf);
		}

		inline bc_wstring bc_to_wstring(bc_string p_str)
		{
			bc_string::size_type l_len = p_str.size() + 1;
			bc_wstring l_str('#', l_len);

			std::mbstate_t l_state = std::mbstate_t();
			bcWCHAR* l_dest = &l_str[0];
			const bcCHAR* l_src = &p_str[0];

			std::mbsrtowcs(l_dest, &l_src, l_len, &l_state);
			
			return std::move(l_str);
		}

#undef TO_STRING_BUFF

	}
}

namespace std
{
	template<>
	struct hash< black_cat::core::bc_string >
	{
	public:
		using argument_type = black_cat::core::bc_string;
		using result_type = std::size_t;

		result_type operator ()(argument_type p_arg)
		{
			uint32_t l_hash = 0;
			uint32_t l_length = p_arg.length();
			argument_type::value_type* l_raw = const_cast<argument_type::value_type*>(p_arg.c_str());

			for (uint32_t i = 0; i < l_length; i++)
			{
				l_hash += ((i + 1) * l_raw[i]);
			}

			return l_hash;
		}
	};

	template<>
	struct hash< black_cat::core::bc_wstring >
	{
		using argument_type = black_cat::core::bc_wstring;
		using result_type = std::size_t;

		result_type operator ()(argument_type p_arg)
		{
			uint32_t l_hash = 0;
			uint32_t l_length = p_arg.length();
			argument_type::value_type* l_raw = const_cast<argument_type::value_type*>(p_arg.c_str());

			for (uint32_t i = 0; i < l_length; i++)
			{
				l_hash += ((i + 1) * l_raw[i]);
			}

			return l_hash;
		}
	};
}