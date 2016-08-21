// [10/08/2014 MRB]

#pragma once

#include <utility>
#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatform/Concurrency/bcConcurrencyDef.h"

namespace black_cat
{
	namespace core_platform
	{
		template< bc_platform TP >
		struct bc_platform_mutex_pack
		{
		};

		template< bc_platform TP >
		struct bc_platform_timed_mutex_pack
		{
		};

		template< bc_platform TP >
		struct bc_platform_recursive_mutex_pack
		{
		};

		template< bc_platform TP >
		struct bc_platform_recursive_timed_mutex_pack
		{
		};

		template< bc_platform TP >
		struct bc_platform_shared_mutex_pack
		{
		};

		template< bc_platform TP >
		struct bc_platform_hybrid_mutex_pack
		{
		};

		template< bc_platform TP >
		class bc_platform_mutex : private bc_no_copy
		{
		public:
			using platform_pack = bc_platform_mutex_pack<TP>;

		public:
			bc_platform_mutex();

			~bc_platform_mutex();

			bcInline void lock();

			bcInline void unlock() noexcept(true);

			bcInline bool try_lock() noexcept(true);

		protected:

		private:
			platform_pack m_pack;
		};

		template< bc_platform TP >
		class bc_platform_timed_mutex : private bc_no_copy
		{
		public:
			using platform_pack = bc_platform_timed_mutex_pack<TP>;

		public:
			bc_platform_timed_mutex();

			~bc_platform_timed_mutex();

			bcInline void lock();

			bcInline void unlock() noexcept(true);

			bcInline bool try_lock() noexcept(true);

			bcInline bool try_lock_for(const bcUINT64 p_nano);

		protected:

		private:
			platform_pack m_pack;
		};

		template< bc_platform TP >
		class bc_platform_recursive_mutex : private bc_no_copy
		{
		public:
			using platform_pack = bc_platform_recursive_mutex_pack<TP>;

		public:
			bc_platform_recursive_mutex();

			~bc_platform_recursive_mutex();

			bcInline void lock();

			bcInline void unlock() noexcept(true);

			bcInline bool try_lock() noexcept(true);

		protected:

		private:
			platform_pack m_pack;
		};

		template< bc_platform TP >
		class bc_platform_recursive_timed_mutex : private bc_no_copy
		{
		public:
			using platform_pack = bc_platform_recursive_timed_mutex_pack<TP>;

		public:
			bc_platform_recursive_timed_mutex();

			~bc_platform_recursive_timed_mutex();

			bcInline void lock();

			bcInline void unlock() noexcept(true);

			bcInline bool try_lock() noexcept(true);

			bcInline bool try_lock_for(const bcUINT64 p_nano);

		protected:

		private:
			platform_pack m_pack;
		};

		template< bc_platform TP >
		class bc_platform_shared_mutex : private bc_no_copy
		{
		public:
			using platform_pack = bc_platform_shared_mutex_pack<TP>;

		public:
			bc_platform_shared_mutex();

			~bc_platform_shared_mutex();

			bcInline void lock();

			bcInline void lock_shared();

			bcInline void unlock();

			bcInline void unlock_shared();

			bcInline bool try_lock();

			bcInline bool try_lock_shared();

			bcInline bool try_lock_for(const bcUINT64 p_nano);

			bcInline bool try_lock_shared_for(const bcUINT64 p_nano);

		protected:

		private:
			platform_pack m_pack;
		};

		template< bc_platform TP >
		class bc_platform_hybrid_mutex : private bc_no_copy
		{
		public:
			using platform_pack = bc_platform_hybrid_mutex_pack<TP>;

		public:
			bc_platform_hybrid_mutex();

			~bc_platform_hybrid_mutex();

			bcInline void lock();

			bcInline void lock(bc_lock_operation p_lock_operation);

			bcInline void unlock() noexcept(true);

			bcInline bool try_lock() noexcept(true);

			bcInline bool try_lock(bc_lock_operation p_lock_operation) noexcept(true);

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_mutex = bc_platform_mutex< g_current_platform >;
		using bc_timed_mutex = bc_platform_timed_mutex< g_current_platform >;
		using bc_recursive_mutex = bc_platform_recursive_mutex< g_current_platform >;
		using bc_recursive_timed_mutex = bc_platform_recursive_timed_mutex< g_current_platform >;
		using bc_shared_mutex = bc_platform_shared_mutex< g_current_platform >;
		using bc_hybrid_mutex = bc_platform_hybrid_mutex< g_current_platform >;

		template< typename T >
		class bc_lock_guard : private bc_no_copy
		{
		public:
			using mutex_type = T;

		public:
			bc_lock_guard(mutex_type& p_lockable)
				: m_lock(&p_lockable)
			{
				m_lock->lock();
			}

			bc_lock_guard(mutex_type& p_lockable, bc_lock_property p_lock_properties) noexcept(true)
				: m_lock(&p_lockable)
			{
				bcAssert(p_lock_properties == bc_lock_property::adapt);
			}

			bc_lock_guard(mutex_type& p_lockable, bc_lock_operation p_lock_operation)
				: m_lock(&p_lockable)
			{
				//bc_template_is< bc_Ihybrid_lockable, Type >::assert();

				m_lock->lock(p_lock_operation);
			}

			~bc_lock_guard()
			{
				m_lock->unlock();
			}

		protected:

		private:
			mutex_type* m_lock;

		};

		template< typename T >
		class bc_unique_lock : private bc_no_copy
		{
		public:
			using mutex_type = T;

		public:
			bc_unique_lock() noexcept(true)
				: m_lock(nullptr),
				m_owns(false)
			{
			}

			explicit bc_unique_lock(mutex_type& p_lockable)
				: m_lock(&p_lockable),
				m_owns(false)
			{
				m_lock->lock();
				m_owns = true;
			}

			bc_unique_lock(mutex_type& p_lockable, bc_lock_operation p_lock_operation)
			{
				//bc_template_is< bc_Ihybrid_lockable, Type >::assert();

				m_lock = &p_lockable;
				m_lock->lock(p_lock_operation);
				m_owns = true;
			}

			bc_unique_lock(mutex_type& p_lockable, bc_lock_property p_lock_properties) noexcept(true)
			{
				m_lock = &p_lockable;
				switch (p_lock_properties)
				{
				case core_platform::bc_lock_property::adapt:
					m_owns = true;
					break;
				case core_platform::bc_lock_property::defer:
					m_owns = false;
					break;
				case core_platform::bc_lock_property::Try:
					m_owns = m_lock->try_lock();
					break;
				default:
					break;
				}
			}

			bc_unique_lock(mutex_type& p_lockable, bc_lock_property p_lock_properties, bc_lock_operation p_lock_operation) noexcept(true)
			{
				m_lock = &p_lockable;
				switch (p_lock_properties)
				{
				case core_platform::bc_lock_property::adapt:
					m_owns = true;
					break;
				case core_platform::bc_lock_property::defer:
					m_owns = false;
					break;
				case core_platform::bc_lock_property::Try:
					m_owns = m_lock->try_lock(p_lock_operation);
					break;
				default:
					break;
				}
			}

			bc_unique_lock(mutex_type& p_lockable, const bcINT64 p_nano)
			{
				//bc_template_is< bc_Itimed_lockable, Type >::assert();

				m_lock = &p_lockable;
				m_owns = m_lock->try_lock_for(p_nano);
			}

			bc_unique_lock(bc_unique_lock&& p_other) noexcept(true)
			{
				m_lock = p_other.m_lock;
				m_owns = p_other.m_owns;
				p_other.m_lock = nullptr;
				p_other.m_owns = false;
			}

			~bc_unique_lock() noexcept(true)
			{
				if (m_owns)
					m_lock->unlock();
			}

			bc_unique_lock& operator=(bc_unique_lock&& p_other) noexcept(true)
			{
				if (this != &p_other)
				{
					if (m_owns)
						m_lock->unlock();
					m_lock = p_other.m_lock;
					m_owns = p_other.m_owns;
					p_other.m_lock = nullptr;
					p_other.m_owns = false;
				}
				return *this;
			}

			bcInline void swap(bc_unique_lock& p_other) noexcept(true)
			{
				std::swap(m_lock, p_other.m_lock);
				std::swap(m_owns, p_other.m_owns);
			}

			bcInline void lock()
			{
				m_lock->lock();
				m_owns = true;
			}

			bcInline void lock(bc_lock_operation p_lock_operation)
			{
				//bc_template_is< bc_Ihybrid_lockable, Type >::assert();

				m_lock->lock(p_lock_operation);
				m_owns = true;
			}

			bcInline bool try_lock()
			{
				m_owns = m_lock->try_lock();
				return m_owns;
			}

			bcInline bool try_lock(bc_lock_operation p_lock_operation)
			{
				m_owns = m_lock->try_lock(p_lock_operation);
				return m_owns;
			}

			bcInline bool try_lock_for(const bcINT64 p_nano)
			{
				//bc_template_is< bc_Itimed_lockable, Type >::assert();

				m_owns = m_lock->try_lock_for(p_nano);
				return m_owns;
			}

			bcInline void unlock()
			{
				m_lock->unlock();
				m_owns = false;
			}

			bcInline explicit operator bool() const noexcept(true)
			{
				return m_owns;
			}

			bcInline bool owns_lock() const noexcept(true)
			{
				return m_owns;
			}

			bcInline mutex_type* get() const noexcept(true)
			{
				return m_lock;
			}

			bcInline mutex_type* release() noexcept(true)
			{
				mutex_type* l_lock = m_lock;
				m_lock = nullptr;
				m_owns = false;
				return l_lock;
			}

		protected:

		private:
			mutex_type* m_lock;
			bool m_owns;

		};

		template< typename T >
		class bc_shared_lock : private bc_no_copy
		{
		public:
			using mutex_type = T;

		public:
			bc_shared_lock() noexcept(true)
				: m_mutex(nullptr),
				m_owns(false)
			{
			}

			bc_shared_lock(bc_shared_lock&& p_other) noexcept(true)
				: m_mutex(p_other.m_mutex),
				m_owns(p_other.m_owns)
			{
				p_other.m_mutex = nullptr;
				p_other.m_owns = nullptr;
			}

			explicit bc_shared_lock(mutex_type& p_mutex)
			{
				m_mutex = &p_mutex;
				m_mutex->lock_shared();
				m_owns = true;
			}

			bc_shared_lock(mutex_type& p_mutex, bc_lock_property p_lock_property)
			{
				m_mutex = &p_mutex;
				switch (p_lock_property)
				{
				case bc_lock_property::adapt:
					m_owns = true;
					break;
				case bc_lock_property::defer:
					m_owns = false;
					break;
				case bc_lock_property::Try:
					m_owns = m_mutex->try_lock_shared();
					break;
				default:
					break;
				}
			}

			bc_shared_lock(mutex_type& p_mutex, const bcUINT64 p_nano)
			{
				m_mutex = &p_mutex;
				m_owns = m_mutex->try_lock_shared_for(p_nano);
			}

			~bc_shared_lock()
			{
				if (m_mutex && m_owns)
					m_mutex->unlock_shared();
			}

			bc_shared_lock& operator =(bc_shared_lock&& p_other)
			{
				m_mutex = p_other.m_mutex;
				m_owns = p_other.m_owns;

				p_other.m_mutex = nullptr;
				p_other.m_owns = nullptr;

				return *this;
			}

			void lock()
			{
				m_mutex->lock_shared();
			}

			bool try_lock()
			{
				return m_mutex->try_lock_shared();
			}

			bool try_lock_for(bcUINT64 p_nano)
			{
				return m_mutex->try_lock_shared_for(p_nano);
			}

			void unlock()
			{
				m_mutex->unlock_shared();
			}

			void swap(bc_shared_lock& p_other) noexcept(true)
			{
				std::swap(m_mutex, p_other.m_mutex);
				std::swap(m_owns, p_other.m_owns);
			}

			mutex_type* release() noexcept(true)
			{
				mutex_type* l_result = m_mutex;

				m_mutex = nullptr;
				m_owns = false;

				return l_result;
			}

			mutex_type* mutex() const noexcept(true)
			{
				return m_mutex;
			}

			bool owns_lock() const noexcept(true)
			{
				return m_owns;
			}

			explicit operator bool() const noexcept(true)
			{
				return m_owns;
			}

		protected:

		private:
			mutex_type* m_mutex;
			bool m_owns;
		};

		template< class TMutex >
		void swap(bc_unique_lock<TMutex>& p_first, bc_unique_lock<TMutex>& p_second) noexcept(true)
		{
			p_first.swap(p_second);
		}

		template< class TMutex >
		void swap(bc_shared_lock<TMutex>& p_first, bc_shared_lock<TMutex>& p_second) noexcept(true)
		{
			p_first.swap(p_second);
		}

		template< typename ...T >
		bool bc_try_lock(T&... p_locks);

		template< typename ...T >
		bool bc_try_lock(T&... p_locks)
		{
			//bc_template_is< bc_Ilockable, T>::assert();

			const bcUINT32 l_count = sizeof...(p_locks);
			T& l_locks[l_count] = { p_locks... };

			bcINT32 l_break = -1;
			bcUINT32 l_i = 0;
			try
			{
				for (l_i = 0; l_i < l_count; ++l_i)
				{
					if (!l_locks[l_i].tryLock())
					{
						l_break = l_i;
						break;
					}
				}
			}
			catch (...)
			{
				l_break = l_i;
			}

			if (l_break != -1)
			{
				for (l_i = 0; l_i < l_break; ++l_i)
				{
					l_locks[l_i].unlock();
				}
				return false;
			}
			return true;
		};

		template< typename ...T >
		bool bc_try_lock(T&... p_locks, core_platform::bc_lock_operation p_lock_operation);

		template< typename ...T >
		bool bc_try_lock(T&... p_locks, core_platform::bc_lock_operation p_lock_operation)
		{
			//bc_template_is< bc_Ihybrid_lockable, T >::assert();

			const bcUINT32 l_count = sizeof...(p_locks);
			T& l_locks[l_count] = { p_locks... };

			bcINT32 l_break = -1;
			bcUINT32 l_i = 0;
			try
			{
				for (l_i = 0; l_i < l_count; ++l_i)
				{
					if (!l_locks[l_i].tryLock(p_lock_operation))
					{
						l_break = l_i;
						break;
					}
				}
			}
			catch (...)
			{
				l_break = l_i;
			}

			if (l_break != -1)
			{
				for (l_i = 0; l_i < l_break; ++l_i)
				{
					l_locks[l_i].unlock();
				}
				return false;
			}
			return true;
		};

		template< typename ...T >
		void bc_lock(T&... p_locks);

		template< typename ...T >
		void bc_lock(T&... p_locks)
		{
			while (!bc_try_lock(p_locks...))
			{

			}
		}

		template< typename ...T >
		void bc_lock(T&... p_locks, core_platform::bc_lock_operation p_lock_operation);

		template< typename ...T >
		void bc_lock(T&... p_locks, core_platform::bc_lock_operation p_lock_operation)
		{
			while (!bc_try_lock(p_locks..., p_lock_operation))
			{

			}
		}

		template< typename ...T >
		void bc_unlock(T&... p_locks);

		template< typename ...T >
		void bc_unlock(T&... p_locks)
		{
			//bc_template_is< bc_Ilockable, T>::assert();

			const bcUINT32 l_count = sizeof...(p_locks);
			T& l_locks[l_count] = { p_locks... };

			for (bcUINT32 i = 0; i < l_count; ++i)
			{
				l_locks[i].unlock();
			}
		};
	}
}