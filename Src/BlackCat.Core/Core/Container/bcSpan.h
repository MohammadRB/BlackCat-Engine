// [03/13/2021 MRB]

#pragma once

#include <type_traits>
#include <iterator>
#include "CorePlatform/bcType.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_const_span
		{
		public:
			using element_type = T;
			using value_type = std::remove_cv_t<T>;
			using size_type = bcSIZE;
			using difference_type = ptrdiff_t;
			using const_pointer = const T*;
			using const_reference = const T&;
			using const_iterator = const_pointer;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		public:
			constexpr bc_const_span() noexcept;

			constexpr bc_const_span(const value_type* p_data, bcUINT32 p_size) noexcept;

			constexpr bc_const_span(const bc_const_span& p_other) noexcept;

			~bc_const_span();

			constexpr bc_const_span& operator=(const bc_const_span& p_other) noexcept;

			constexpr const_reference operator [](size_type p_position) const;

			constexpr const_reference front() const;

			constexpr const_reference back() const;

			constexpr const_iterator begin() const noexcept;

			constexpr const_iterator cbegin() const noexcept;

			constexpr const_iterator end() const noexcept;

			constexpr const_iterator cend() const noexcept;

			constexpr const_reverse_iterator rbegin() const noexcept;

			constexpr const_reverse_iterator crbegin() const noexcept;

			constexpr const_reverse_iterator rend() const noexcept;

			constexpr const_reverse_iterator crend() const noexcept;

			constexpr const value_type* data() const;

			constexpr size_type size() const noexcept;

			constexpr bool empty() const noexcept;

		protected:
			value_type* m_data;
			bcUINT32 m_size;
		};
		
		template< typename T >
		class bc_span : public bc_const_span<T>
		{
		public:
			using base_type = bc_const_span<T>;
			using element_type = typename base_type::element_type;
			using value_type = typename base_type::value_type;
			using size_type = typename base_type::size_type;
			using difference_type = typename base_type::difference_type;
			using pointer = T*;
			using const_pointer = typename base_type::const_pointer;
			using reference = T&;
			using const_reference = typename base_type::const_reference;
			using iterator = pointer;
			using const_iterator = typename base_type::const_pointer;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = typename base_type::const_reverse_iterator;

			using base_type::front;
			using base_type::back;
			using base_type::begin;
			using base_type::end;
			using base_type::rbegin;
			using base_type::rend;
			using base_type::data;
			
		public:
			constexpr bc_span() noexcept;
			
			constexpr bc_span(value_type* p_data, bcUINT32 p_size) noexcept;

			constexpr bc_span(const bc_span& p_other) noexcept;
			
			~bc_span();

			constexpr bc_span& operator=(const bc_span& p_other) noexcept;

			constexpr reference operator [](size_type p_position);

			constexpr reference front();

			constexpr reference back();
			
			constexpr iterator begin() noexcept;

			constexpr iterator end() noexcept;

			constexpr reverse_iterator rbegin() noexcept;

			constexpr reverse_iterator rend() noexcept;
			
			constexpr value_type* data();
		};

		template< class TContainer >
		bc_span< typename TContainer::value_type > bc_make_span(TContainer& p_container)
		{
			return { p_container.data(), p_container.size() };
		}

		template< class TContainer >
		bc_const_span< typename TContainer::value_type > bc_make_span(const TContainer& p_container)
		{
			return { p_container.data(), p_container.size() };
		}

		template< class TContainer >
		bc_const_span< typename TContainer::value_type > bc_make_cspan(const TContainer& p_container)
		{
			return { p_container.data(), p_container.size() };
		}
		
		template< typename T >
		constexpr bc_const_span<T>::bc_const_span() noexcept
			: m_data(nullptr),
			m_size(0)
		{
		}

		template< typename T >
		constexpr bc_const_span<T>::bc_const_span(const value_type* p_data, bcUINT32 p_size) noexcept
			: m_data(const_cast<value_type*>(p_data)),
			m_size(p_size)
		{
		}

		template< typename T >
		constexpr bc_const_span<T>::bc_const_span(const bc_const_span& p_other) noexcept = default;

		template< typename T >
		bc_const_span<T>::~bc_const_span() = default;

		template< typename T >
		constexpr bc_const_span<T>& bc_const_span<T>::operator=(const bc_const_span& p_other) noexcept = default;

		template< typename T >
		constexpr typename bc_const_span<T>::const_reference bc_const_span<T>::operator[](size_type p_position) const
		{
			BC_ASSERT(p_position < m_size);
			return m_data[p_position];
		}

		template< typename T >
		constexpr typename bc_const_span<T>::const_reference bc_const_span<T>::front() const
		{
			return m_data[0];
		}

		template< typename T >
		constexpr typename bc_const_span<T>::const_reference bc_const_span<T>::back() const
		{
			return m_data[m_size - 1];
		}

		template< typename T >
		constexpr typename bc_const_span<T>::const_iterator bc_const_span<T>::begin() const noexcept
		{
			return &front();
		}

		template< typename T >
		constexpr typename bc_const_span<T>::const_iterator bc_const_span<T>::cbegin() const noexcept
		{
			return &front();
		}

		template< typename T >
		constexpr typename bc_const_span<T>::const_iterator bc_const_span<T>::end() const noexcept
		{
			return &back() + 1;
		}

		template< typename T >
		constexpr typename bc_const_span<T>::const_iterator bc_const_span<T>::cend() const noexcept
		{
			return &back() + 1;
		}

		template< typename T >
		constexpr typename bc_const_span<T>::const_reverse_iterator bc_const_span<T>::rbegin() const noexcept
		{
			return const_reverse_iterator(&back());
		}

		template< typename T >
		constexpr typename bc_const_span<T>::const_reverse_iterator bc_const_span<T>::crbegin() const noexcept
		{
			return const_reverse_iterator(&back());
		}

		template< typename T >
		constexpr typename bc_const_span<T>::const_reverse_iterator bc_const_span<T>::rend() const noexcept
		{
			return const_reverse_iterator(&front() - 1);
		}

		template< typename T >
		constexpr typename bc_const_span<T>::const_reverse_iterator bc_const_span<T>::crend() const noexcept
		{
			return const_reverse_iterator(&front() - 1);
		}

		template< typename T >
		constexpr const typename bc_const_span<T>::value_type* bc_const_span<T>::data() const
		{
			return m_data;
		}
		
		template< typename T >
		constexpr typename bc_const_span<T>::size_type bc_const_span<T>::size() const noexcept
		{
			return m_size;
		}

		template< typename T >
		constexpr bool bc_const_span<T>::empty() const noexcept
		{
			return m_size == 0;
		}
		
		template< typename T >
		constexpr bc_span<T>::bc_span() noexcept
			: bc_const_span<T>()
		{
		}

		template< typename T >
		constexpr bc_span<T>::bc_span(value_type* p_data, bcUINT32 p_size) noexcept
			: bc_const_span<T>(p_data, p_size)
		{
		}

		template< typename T >
		constexpr bc_span<T>::bc_span(const bc_span& p_other) noexcept = default;

		template< typename T >
		bc_span<T>::~bc_span() = default;

		template< typename T >
		constexpr bc_span<T>& bc_span<T>::operator=(const bc_span& p_other) noexcept = default;

		template< typename T >
		constexpr typename bc_span<T>::reference bc_span<T>::operator[](size_type p_position)
		{
			BC_ASSERT(p_position < base_type::m_size);
			return base_type::m_data[p_position];
		}

		template< typename T >
		constexpr typename bc_span<T>::reference bc_span<T>::front()
		{
			return base_type::m_data[0];
		}

		template< typename T >
		constexpr typename bc_span<T>::reference bc_span<T>::back()
		{
			return base_type::m_data[base_type::m_size - 1];
		}

		template< typename T >
		constexpr typename bc_span<T>::iterator bc_span<T>::begin() noexcept
		{
			return &front();
		}

		template< typename T >
		constexpr typename bc_span<T>::iterator bc_span<T>::end() noexcept
		{
			return &back() + 1;
		}

		template< typename T >
		constexpr typename bc_span<T>::reverse_iterator bc_span<T>::rbegin() noexcept
		{
			return reverse_iterator(&back());
		}

		template< typename T >
		constexpr typename bc_span<T>::reverse_iterator bc_span<T>::rend() noexcept
		{
			return reverse_iterator(&front() - 1);
		}

		template< typename T >
		constexpr typename bc_span<T>::value_type* bc_span<T>::data()
		{
			return base_type::m_data;
		}
	}
}