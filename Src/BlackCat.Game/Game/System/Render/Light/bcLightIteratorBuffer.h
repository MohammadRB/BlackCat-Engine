// [11/03/2020 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"

namespace black_cat
{
	namespace game
	{
		template< class TContainer >
		class bc_light_iterator_buffer
		{
		public:
			using container_type = TContainer;
			using value_type = typename container_type::value_type;
			using pointer = typename container_type::pointer;
			using const_pointer = typename container_type::const_pointer;
			using reference = typename container_type::reference;
			using const_reference = typename container_type::const_reference;
			using difference_type = typename container_type::difference_type;
			using size_type = typename container_type::size_type;
			using iterator = typename container_type::iterator;
			using const_iterator = typename container_type::const_iterator;
			using reverse_iterator = typename container_type::reverse_iterator;
			using const_reverse_iterator = typename container_type::const_reverse_iterator;

		public:
			bc_light_iterator_buffer(core_platform::bc_shared_mutex& p_container_lock, const container_type& p_container);

			bc_light_iterator_buffer(bc_light_iterator_buffer&&) noexcept;

			~bc_light_iterator_buffer();

			bc_light_iterator_buffer& operator=(bc_light_iterator_buffer&&) noexcept;

			const_iterator begin() const noexcept;

			const_iterator cbegin() const noexcept;

			const_iterator end() const noexcept;

			const_iterator cend() const noexcept;

			const_reverse_iterator rbegin() const noexcept;

			const_reverse_iterator crbegin() const noexcept;

			const_reverse_iterator rend() const noexcept;

			const_reverse_iterator crend() const noexcept;

			size_type size() const noexcept;

			void lock();

			void unlock() noexcept;
			
		private:
			const container_type* m_container;
			core_platform::bc_shared_mutex* m_container_lock;
		};

		template< class TContainer >
		bc_light_iterator_buffer<TContainer>::bc_light_iterator_buffer(core_platform::bc_shared_mutex& p_container_lock, const container_type& p_container)
			: m_container(&p_container),
			m_container_lock(&p_container_lock)
		{
		}

		template< class TContainer >
		bc_light_iterator_buffer<TContainer>::bc_light_iterator_buffer(bc_light_iterator_buffer&& p_other) noexcept
		{
			m_container = p_other.m_container;
			m_container_lock = p_other.m_container_lock;
			p_other.m_container = nullptr;
			p_other.m_container_lock = nullptr;
		}

		template< class TContainer >
		bc_light_iterator_buffer<TContainer>::~bc_light_iterator_buffer() = default;

		template< class TContainer >
		bc_light_iterator_buffer<TContainer>& bc_light_iterator_buffer<TContainer>::operator=(bc_light_iterator_buffer&& p_other) noexcept
		{
			m_container = p_other.m_container;
			m_container_lock = p_other.m_container_lock;
			p_other.m_container = nullptr;
			p_other.m_container_lock = nullptr;
			
			return *this;
		}

		template< class TContainer >
		typename bc_light_iterator_buffer<TContainer>::const_iterator bc_light_iterator_buffer<TContainer>::begin() const noexcept
		{
			return m_container->begin();
		}

		template< class TContainer >
		typename bc_light_iterator_buffer<TContainer>::const_iterator bc_light_iterator_buffer<TContainer>::cbegin() const noexcept
		{
			return m_container->cbegin();
		}

		template< class TContainer >
		typename bc_light_iterator_buffer<TContainer>::const_iterator bc_light_iterator_buffer<TContainer>::end() const noexcept
		{
			return m_container->end();
		}

		template< class TContainer >
		typename bc_light_iterator_buffer<TContainer>::const_iterator bc_light_iterator_buffer<TContainer>::cend() const noexcept
		{
			return m_container->cend();
		}

		template< class TContainer >
		typename bc_light_iterator_buffer<TContainer>::const_reverse_iterator bc_light_iterator_buffer<TContainer>::rbegin() const noexcept
		{
			return m_container->rbegin();
		}

		template< class TContainer >
		typename bc_light_iterator_buffer<TContainer>::const_reverse_iterator bc_light_iterator_buffer<TContainer>::crbegin() const noexcept
		{
			return m_container->crbegin();
		}

		template< class TContainer >
		typename bc_light_iterator_buffer<TContainer>::const_reverse_iterator bc_light_iterator_buffer<TContainer>::rend() const noexcept
		{
			return m_container->rend();
		}

		template< class TContainer >
		typename bc_light_iterator_buffer<TContainer>::const_reverse_iterator bc_light_iterator_buffer<TContainer>::crend() const noexcept
		{
			return m_container->crend();
		}

		template< class TContainer >
		typename bc_light_iterator_buffer<TContainer>::size_type bc_light_iterator_buffer<TContainer>::size() const noexcept
		{
			return m_container->size();
		}

		template< class TContainer >
		void bc_light_iterator_buffer<TContainer>::lock()
		{
			m_container_lock->lock_shared();
		}

		template< class TContainer >
		void bc_light_iterator_buffer<TContainer>::unlock() noexcept
		{
			m_container_lock->unlock_shared();
		}
	}
}