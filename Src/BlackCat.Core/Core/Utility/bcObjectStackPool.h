// [01/04/2020 MRB]

#pragma once

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

			template<typename T, typename ...TArgs>
			T* alloc(TArgs&&... p_parameters);

			template<typename T>
			void free(T* p_object) noexcept;

		private:
			virtual void _initialize(bcSIZE p_max_num_thread, bcSIZE p_capacity) override final;

			virtual void _destroy() override final;

			void* _alloc(bcSIZE p_size);

			void _free(void* p_pointer, bcSIZE p_size);

			bc_memory_stack m_stack_allocator;
		};
		
		template<typename T, typename ...TArgs>
		inline T* bc_concurrent_object_stack_pool::alloc(TArgs&&... p_parameters)
		{
			void* l_memory = _alloc(sizeof(T));
			new (l_memory) T(std::forward<T>(p_parameters)...);

			return reinterpret_cast<T*>(l_memory);
		}
		
		template<typename T>
		inline void bc_concurrent_object_stack_pool::free(T* p_object) noexcept
		{
			p_object->~T();
			_free(p_object, sizeof(T));
		}
	}
}