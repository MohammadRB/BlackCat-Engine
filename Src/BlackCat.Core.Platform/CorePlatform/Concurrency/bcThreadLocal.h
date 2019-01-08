// [11/7/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatform/Concurrency/bcThread.h"

namespace black_cat
{
	namespace core_platform
	{
		template< bc_platform TP >
		struct bc_platform_thread_local_pack
		{
		};

		template< bc_platform TPlatform, typename T >
		class bc_platform_thread_local : private bc_no_copy
		{
		public:
			using type = T;
			using this_type = bc_platform_thread_local;
			using platform_pack = bc_platform_thread_local_pack<TPlatform>;
			using cleanup_function = void(*)(type*);

		public:
			bc_platform_thread_local() noexcept(false)
				: m_cleanup_function(&_default_clean_up)
			{
				_platform_initialize();
			}

			explicit bc_platform_thread_local(cleanup_function p_cleanup_function) noexcept(false)
				: m_cleanup_function(p_cleanup_function)
			{
				_platform_initialize();
			}

			bc_platform_thread_local(this_type&& p_other) noexcept
			{
				_assign(std::move(p_other));
			}

			~bc_platform_thread_local() noexcept
			{
				_cleanup();
				_platform_cleanup();
			}

			this_type& operator =(this_type&& p_other) noexcept
			{
				_assign(std::move(p_other));

				return *this;
			}

			/**
			 * \brief Return thread local pointer, or in case of any error return null
			 * \return 
			 */
			type* get() const noexcept
			{
				return _platform_get();
			}

			/**
			 * \brief Set thread local pointer and cleanup previous pointer and return true in case of success otherwise false
			 * \param p_pointer 
			 * \return 
			 */
			bool set(type* p_pointer) noexcept
			{
				type* l_pointer = get();
				if (!l_pointer)
				{
					// Bug we need to unregister this function when this object get destroyed and resource get released
					// otherwise we will have a function that point to a destroyed object
					bc_thread::current_thread_on_exit(std::bind(&bc_platform_thread_local::_cleanup, this));
				}
				else
				{
					m_cleanup_function(l_pointer);
				}

				return _platform_set(p_pointer);
			}

			type* operator ->() const noexcept
			{
				return get();
			}

			type& operator *() const noexcept
			{
				return *get();
			}

			/**
			 * \brief set thread local pointer to null and return current value
			 * \return 
			 */
			type* release() noexcept
			{
				type* l_pointer = get();
				set(nullptr);

				return l_pointer;
			}

		protected:

		private:
			static void _default_clean_up(type* p_pointer)
			{
				delete p_pointer;
			}

			void _assign(this_type&& p_other)
			{
				_cleanup();

				m_cleanup_function = p_other.m_cleanup_function;
				p_other.m_cleanup_function = nullptr;
				_platform_assign(std::move(p_other));
			}

			void _cleanup()
			{
				type* l_pointer = get();
				if (l_pointer)
				{
					m_cleanup_function(l_pointer);
				}
			}

			void _platform_initialize();

			void _platform_cleanup();

			void _platform_assign(this_type&&);

			bool _platform_set(type*);

			type* _platform_get() const;

			platform_pack m_pack;
			cleanup_function m_cleanup_function;
		};

		template< typename T >
		using bc_thread_local = bc_platform_thread_local< g_current_platform, T >;
	}
}