// [7/11/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/Utility/bcNoCopy.h"

namespace black_cat::platform
{
	template<bc_platform TP>
	struct bc_platform_thread_local_pack
	{
	};

	template<bc_platform TPlatform, typename T>
	class bc_platform_thread_local : private bc_no_copy
	{
	public:
		using value_type = T;
		using platform_pack = bc_platform_thread_local_pack<TPlatform>;

	public:
		bc_platform_thread_local() noexcept(false)
		{
			_platform_initialize();
		}

		bc_platform_thread_local(bc_platform_thread_local&& p_other) noexcept
		{
			_assign(std::move(p_other));
		}

		~bc_platform_thread_local() noexcept
		{
			_platform_cleanup();
		}

		bc_platform_thread_local& operator =(bc_platform_thread_local&& p_other) noexcept
		{
			_assign(std::move(p_other));

			return *this;
		}

		/**
			 * \brief Return thread local pointer, or in case of any error return null
			 * \return
			 */
		value_type* get() noexcept
		{
			return _platform_get();
		}
			
		/**
			 * \brief Return thread local pointer, or in case of any error return null
			 * \return 
			 */
		const value_type* get() const noexcept
		{
			return _platform_get();
		}

		/**
			 * \brief Set thread local pointer and return true in case of success otherwise false
			 * \param p_pointer
			 * \return 
			 */
		bool set(value_type* p_pointer) noexcept
		{
			return _platform_set(p_pointer);
		}

		value_type* operator ->() noexcept
		{
			return get();
		}

		const value_type* operator ->() const noexcept
		{
			return get();
		}

		value_type& operator *() noexcept
		{
			return *get();
		}

		const value_type& operator *() const noexcept
		{
			return *get();
		}

		/**
			 * \brief set thread local pointer to null and return current value
			 * \return 
			 */
		value_type* release() noexcept
		{
			value_type* l_pointer = get();
			set(nullptr);

			return l_pointer;
		}

	private:
		void _assign(bc_platform_thread_local&& p_other)
		{
			_platform_assign(std::move(p_other));
		}

		void _platform_initialize();

		void _platform_cleanup();

		void _platform_assign(bc_platform_thread_local&&);

		bool _platform_set(value_type*);

		value_type* _platform_get() const;

		platform_pack m_pack;
	};

	template<typename T>
	using bc_thread_local = bc_platform_thread_local<g_current_platform, T>;
}
