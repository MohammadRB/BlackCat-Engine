//  [10/28/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcThreadLocal.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"

namespace black_cat
{
	namespace core
	{
		/**
		 * \brief Same as core_platform::bc_thread_local but provide auto initialization. if type is an integral it will initialize to zero
		 * \tparam T 
		 * \tparam TAllocType 
		 */
		template<typename T, bc_alloc_type TAllocType = bc_alloc_type::unknown>
		class bc_thread_local
		{
		public:
			using value_type = T;

		private:
			struct entry
			{
				value_type m_value;
				entry* m_next;
			};

		public:
			bc_thread_local()
				: m_local()
			{
				static_assert(TAllocType != bc_alloc_type::unknown_movable, "movable pointers are not allowed");
				static_assert(std::is_default_constructible_v<value_type>, "T must be default constructable");
			}

			bc_thread_local(bc_thread_local&& p_other) noexcept
				: m_local(std::move(p_other.m_local))
			{
			}

			~bc_thread_local()
			{
				entry* l_first_entry = m_first_entry.load(core_platform::bc_memory_order::relaxed);
				while (l_first_entry)
				{
					entry* l_next_entry = l_first_entry->m_next;
					_default_clean_up(l_first_entry);
					l_first_entry = l_next_entry;
				}
			}

			bc_thread_local& operator =(bc_thread_local&& p_other) noexcept
			{
				m_local = std::move(p_other.m_local);

				return *this;
			}

			value_type& get() noexcept
			{
				entry* l_entry = m_local.get();

				if (!l_entry)
				{
					l_entry = _create<T>();
					m_local.set(l_entry);
				}

				return l_entry->m_value;
			}

			const value_type& get() const noexcept
			{
				return const_cast<bc_thread_local*>(this)->get();
			}

			bool set(value_type&& p_value) noexcept
			{
				entry& l_entry = get();
				l_entry.m_value = std::move(p_value);
				
				return true;
			}

			value_type* operator ->() noexcept
			{
				return &get();
			}

			const value_type* operator ->() const noexcept
			{
				return &get();
			}

			value_type& operator *() noexcept
			{
				return get();
			}

			const value_type& operator *() const noexcept
			{
				return get();
			}

			value_type release() noexcept
			{
				// TODO no release is done. use actual release and delete entry
				entry* l_entry = m_local.release();
				//entry* l_entry = m_local.get();
				value_type l_temp = std::move(l_entry->m_value);

				return l_temp;
			}

		private:
			static void _default_clean_up(entry* p_pointer)
			{
				BC_DELETE(p_pointer);
			}

			template<typename T>
			typename std::enable_if<std::is_integral<T>::value, entry*>::type _create()
			{
				entry* l_entry = BC_NEW(entry, TAllocType);
				l_entry->m_value = value_type(0);
				l_entry->m_next = nullptr;

				_swap_first_entry(l_entry);
				
				return l_entry;
			}

			template<typename T>
			typename std::enable_if<!std::is_integral<T>::value, entry*>::type _create()
			{
				entry* l_entry = BC_NEW(entry, TAllocType);
				l_entry->m_next = nullptr;

				_swap_first_entry(l_entry);
				
				return l_entry;
			}

			void _swap_first_entry(entry* p_entry)
			{
				entry* l_first_entry = m_first_entry.load(core_platform::bc_memory_order::relaxed);
				while(true)
				{
					p_entry->m_next = l_first_entry;

					if(m_first_entry.compare_exchange_weak
					(
						&l_first_entry,
						p_entry,
						core_platform::bc_memory_order::relaxed,
						core_platform::bc_memory_order::relaxed
					))
					{
						break;
					}
				}
			}

			core_platform::bc_atomic<entry*> m_first_entry; // All created instances by different threads
			core_platform::bc_thread_local<entry> m_local;
		};
	}
}