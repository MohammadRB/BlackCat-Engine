// [23/03/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/Utility/bcInitializable.h"

namespace black_cat
{
	namespace core
	{
		void bc_initializable<>::initialize()
		{
			_initialize();
			m_initialized = true;
		}

		void bc_initializable<>::destroy()
		{
			_destroy();
			m_initialized = false;
		}

		bool bc_initializable<>::is_initialized() const
		{
			return m_initialized;
		}

		bc_initializable<>::bc_initializable() = default;

		bc_initializable<>::bc_initializable(bc_initializable&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_initializable<>::~bc_initializable() = default;

		bc_initializable<>& bc_initializable<>::operator=(bc_initializable&& p_other) noexcept
		{
			m_initialized = p_other.m_initialized;
			p_other.m_initialized = false;

			return *this;
		}
	}
}