// [12/04/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Container/bcIteratorAdapter.h"

namespace black_cat::game
{
	template<class TContainer>
	class bc_decal_iterator_buffer : public core::bc_const_iterator_adapter<TContainer>
	{
	public:
		using container_type = typename core::bc_const_iterator_adapter<TContainer>::container_type;

	public:
		bc_decal_iterator_buffer(platform::bc_mutex& p_container_lock, const container_type& p_container);

		bc_decal_iterator_buffer(bc_decal_iterator_buffer&&) noexcept;

		~bc_decal_iterator_buffer();

		bc_decal_iterator_buffer& operator=(bc_decal_iterator_buffer&&) noexcept;

		void lock();

		void unlock() noexcept;

	private:
		platform::bc_mutex* m_container_lock;
		const container_type* m_container;
	};

	template<class TContainer>
	bc_decal_iterator_buffer<TContainer>::bc_decal_iterator_buffer(platform::bc_mutex& p_container_lock, const container_type& p_container)
		: core::bc_const_iterator_adapter<TContainer>(p_container),
		  m_container_lock(&p_container_lock),
		  m_container(&p_container)
	{
	}

	template<class TContainer>
	bc_decal_iterator_buffer<TContainer>::bc_decal_iterator_buffer(bc_decal_iterator_buffer&& p_other) noexcept
		: core::bc_const_iterator_adapter<TContainer>(*p_other.m_container),
		  m_container_lock(p_other.m_container_lock),
		  m_container(p_other.m_container)
	{
		p_other.m_container = nullptr;
		p_other.m_container_lock = nullptr;
	}

	template<class TContainer>
	bc_decal_iterator_buffer<TContainer>::~bc_decal_iterator_buffer() = default;

	template<class TContainer>
	bc_decal_iterator_buffer<TContainer>& bc_decal_iterator_buffer<TContainer>::operator=(bc_decal_iterator_buffer&& p_other) noexcept
	{
		m_container = p_other.m_container;
		m_container_lock = p_other.m_container_lock;
		p_other.m_container = nullptr;
		p_other.m_container_lock = nullptr;
		return *this;
	}

	template<class TContainer>
	void bc_decal_iterator_buffer<TContainer>::lock()
	{
		m_container_lock->lock();
	}

	template<class TContainer>
	void bc_decal_iterator_buffer<TContainer>::unlock() noexcept
	{
		m_container_lock->unlock();
	}
}
