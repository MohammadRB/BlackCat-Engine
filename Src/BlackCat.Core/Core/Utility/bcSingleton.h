#pragma once

#include "Core/CorePCH.h"
#include "Core/Utility/bcInitializable.h"

namespace black_cat::core
{
	template<typename T>
	class bc_singleton;

	// Any singleton class is no copyable and movable
	template<typename C, typename ...A>
	class bc_singleton<C(A...)> : public bc_initializable<A...>
	{
	private:
		using this_type = bc_singleton<C(A...)>;
		using base_type = bc_initializable<A...>;

	public:
		bc_singleton() = default;

		bc_singleton(const this_type&) = delete;

		bc_singleton(this_type&&) = delete;

		virtual ~bc_singleton() = default;

		this_type& operator=(const this_type&) = delete;

		this_type& operator=(this_type&&) = delete;

		static void start_up(A... pArgs)
		{
			BC_ASSERT(m_instance == nullptr);

			m_instance = new C();
			m_instance->initialize(pArgs...);
		}

		static void close()
		{
			BC_ASSERT(m_instance != nullptr);

			m_instance->destroy();
			delete m_instance;
			m_instance = nullptr;
		}

		static C* get() noexcept
		{
			return m_instance;
		}

	protected:
		static C* m_instance;
	};

	template <typename C, typename ...A>
	C* bc_singleton<C(A...)>::m_instance = nullptr;
}
