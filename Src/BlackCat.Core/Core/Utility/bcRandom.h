// [18/12/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include <random>

namespace black_cat::core
{
	class bc_random
	{
	public:
		bc_random(bcUINT32 p_seed = std::random_device()(),
		          bcUINT32 p_min = std::numeric_limits<bcUINT32>::min(),
		          bcUINT32 p_max = std::numeric_limits<bcUINT32>::max());

		bc_random(const bc_random& p_other) = default;

		~bc_random() = default;

		bc_random& operator=(const bc_random& p_other) = default;

		bcUINT32 min() const noexcept;

		bcUINT32 max() const noexcept;
			
		void seed(bcUINT32 p_seed) noexcept;

		void min_max(bcUINT32 p_min, bcUINT32 p_max) noexcept;

		bcUINT32 generate() noexcept;

	private:
		std::mt19937 m_generator;
		std::uniform_int_distribution<bcUINT32> m_distributor;
	};

	inline bc_random::bc_random(bcUINT32 p_seed, bcUINT32 p_min, bcUINT32 p_max)
		: m_generator(p_seed),
		  m_distributor(p_min, p_max)
	{
	}

	inline bcUINT32 bc_random::min() const noexcept
	{
		return m_distributor.min();
	}

	inline bcUINT32 bc_random::max() const noexcept
	{
		return m_distributor.max();
	}

	inline void bc_random::seed(bcUINT32 p_seed) noexcept
	{
		m_generator.seed(p_seed);
	}

	inline void bc_random::min_max(bcUINT32 p_min, bcUINT32 p_max) noexcept
	{
		m_distributor = std::uniform_int_distribution<bcUINT32>(p_min, p_max);
	}

	inline bcUINT32 bc_random::generate() noexcept
	{
		return m_distributor(m_generator);
	}
}
