// [11/26/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemoryTracer.h"

namespace black_cat
{
	namespace core
	{
		template< typename TMemory >
		class bc_memory_proxy : platform::bc_no_copy
		{
		public:
			using this_type = bc_memory_proxy;
			using memory_type = TMemory;
			using is_movable_type = typename memory_type::is_movable; // must be std::true_type or std::false_type

		public:
			bc_memory_proxy() noexcept(true)
				: memory_type()
			{
			}

			bc_memory_proxy(this_type&& p_other) noexcept(true)
				: memory_type(std::move(p_other.m_memory))
			{
			}

			bc_memory_proxy(memory_type&& p_other) noexcept(true)
				: memory_type(std::move(p_other))
			{
			}

			~bc_memory_proxy() noexcept(true)
			{
				destroy();
			}

			this_type& operator =(this_type&& p_other) noexcept(true)
			{
				m_memory.operator =(std::move(p_other.m_memory));
				return *this;
			}

			this_type& operator =(memory_type&& p_other) noexcept(true)
			{
				m_memory.operator =(std::move(p_other));
				return *this;
			}

			void tag(const bcCHAR* p_tag)
			{
				m_memory.tag(p_tag);
			}

			const bcCHAR* tag() const
			{
				return m_memory.tag();
			}

			const bc_memory_tracer& tracer() const
			{
				return m_memory.tracer();
			}

			template< typename ...A >
			void initialize(A... p_args, const bcCHAR* p_tag)
			{
				m_memory.initialize(p_tag, p_args...);
			}

			void destroy()
			{
				m_memory.destroy();
			}

			void* alloc(bc_memblock* p_memblock) noexcept(true)
			{
				return m_memory.alloc(p_memblock);
			}

			void free(const void* p_pointer, bc_memblock* p_memblock) noexcept(true)
			{
				m_memory.free(p_pointer, p_memblock);
			}

			bool contain_pointer(const void* p_pointer) const noexcept(true)
			{
				return m_memory.contain_pointer(p_pointer);
			}

			void clear() const noexcept(true)
			{
				m_memory.clear();
			}

			void register_pointer(const void* p_pointer)
			{
				register_pointer< memory_type >(p_pointer);
			}

			void unregister_pointer(const void* p_pointer)
			{
				register_pointer< memory_type >(p_pointer);
			}

			void defragment(bcUINT32 p_num)
			{
				defragment< memory_type >(p_num);
			}

		private:
			template< typename T1 >
			typename std::enable_if< is_movable_type::value_type >::type register_pointer(const void* p_pointer)
			{
#ifdef BC_MEMORY_DEFRAG
				m_memory.register_pointer(p_pointer);
#endif
			}

			template< typename T1 >
			typename std::enable_if< !is_movable_type::value_type >::type register_pointer(const void* p_pointer)
			{
			}

			template< typename T1 >
			typename std::enable_if< is_movable_type::value_type >::type unregister_pointer(const void* p_pointer)
			{
#ifdef BC_MEMORY_DEFRAG
				m_memory.unregister_pointer(p_pointer);
#endif
			}
			
			template< typename T1 >
			typename std::enable_if< !is_movable_type::value_type >::type unregister_pointer(const void* p_pointer)
			{
			}

			template< typename T1 >
			typename std::enable_if< is_movable_type::value_type >::type defragment(bcUINT32 p_num)
			{
#ifdef BC_MEMORY_DEFRAG
				m_memory.defragment(p_num);
#endif
			}

			template< typename T1 >
			typename std::enable_if< !is_movable_type::value_type >::type defragment(bcUINT32 p_num)
			{
			}
						
			memory_type m_memory;
		};
	}
}