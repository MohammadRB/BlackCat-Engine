// [01/04/2020 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/bcExport.h"
#include "Core/Memory/bcMemoryStack.h"
#include "Core/Utility/bcInitializable.h"

namespace black_cat
{
	namespace core
	{
		class BC_CORE_DLL bc_concurrent_object_stack_pool : public bc_initializable<bcSIZE, bcSIZE>
		{
		public:
			bc_concurrent_object_stack_pool() noexcept;

			bc_concurrent_object_stack_pool(bc_concurrent_object_stack_pool&& p_other) noexcept;

			~bc_concurrent_object_stack_pool();

			bc_concurrent_object_stack_pool& operator=(bc_concurrent_object_stack_pool&& p_other) noexcept;

			bcSIZE capacity() const noexcept;

			bcSIZE size() const noexcept;

			template<typename T, typename ...TArgs>
			T* alloc(TArgs&&... p_parameters);

			template<typename T>
			void free(T* p_object) noexcept;

		private:
			void _initialize(bcSIZE p_max_num_thread, bcSIZE p_capacity) override final;

			void _destroy() override final;

			void* _alloc(bcSIZE p_size);

			void _free(void* p_pointer, bcSIZE p_size);

			bc_memory_stack m_stack_allocator;
			core_platform::bc_atomic<bcSIZE> m_size;
		};
		
		template<typename T, typename ...TArgs>
		T* bc_concurrent_object_stack_pool::alloc(TArgs&&... p_parameters)
		{
			// TODO default alignment is not preserved
			void* l_memory = _alloc(sizeof(bcSIZE) + sizeof(T));
			void* l_memory_object = reinterpret_cast<bcBYTE*>(l_memory) + sizeof(bcSIZE);
			bcSIZE* l_memory_size = reinterpret_cast<bcSIZE*>(l_memory);

			*l_memory_size = sizeof(T);
			new (l_memory_object) T(std::forward<T>(p_parameters)...);

			m_size.fetch_add(1, core_platform::bc_memory_order::relaxed);
			
			return reinterpret_cast<T*>(l_memory_object);
		}
		
		template<typename T>
		void bc_concurrent_object_stack_pool::free(T* p_object) noexcept
		{
			p_object->~T();

			void* l_memory = reinterpret_cast<bcBYTE*>(p_object) - sizeof(bcSIZE);
			const bcSIZE l_memory_size = *reinterpret_cast<bcSIZE*>(l_memory);

			_free(l_memory, l_memory_size);

			m_size.fetch_sub(1, core_platform::bc_memory_order::relaxed);
		}
	}
}