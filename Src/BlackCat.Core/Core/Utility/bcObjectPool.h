// [2/12/2015 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/bcExport.h"
#include "Core/bcException.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_concurrent_object_pool;

		class BC_CORE_DLL bc_concurrent_memory_pool : public bc_initializable<bcUINT32, bcUINT32, bc_alloc_type>
		{
		private:
			template< typename T >
			friend class bc_concurrent_object_pool;
			using bit_block_type = bcUINT32;

		public:
			bc_concurrent_memory_pool();

			bc_concurrent_memory_pool(bc_concurrent_memory_pool&& p_other) noexcept;

			~bc_concurrent_memory_pool();

			bc_concurrent_memory_pool& operator =(bc_concurrent_memory_pool&& p_other) noexcept;

			bcUINT32 capacity() const noexcept;
			
			bcUINT32 entry_size() const noexcept;

			/**
			 * \brief Return null pointer in case of allocation failure
			 * \return Allocated pointer
			 */
			void* alloc() noexcept;

			void free(void* p_pointer) noexcept;

			bool contain_pointer(const void* p_pointer) const noexcept;

			void clear() noexcept;

		private:
			void _initialize(bcUINT32 p_object_count, bcUINT32 p_object_size, bc_alloc_type p_alloc_type) override;

			void _destroy() override;

			bcINT32 _find_free_bit(bit_block_type p_bit_block) const noexcept;

			static const bit_block_type s_bit_block_mask = 0xffffffff;
			static const bcSIZE s_bit_block_size = sizeof(bit_block_type) * 8;

			bcUINT32 m_num_block;
			bcUINT32 m_block_size;
			bcUINT32 m_num_bit_blocks;
			core_platform::bc_atomic< bcUINT32 > m_allocated_block;	// An index that searching for free block will continue from this place
			bc_ptr<core_platform::bc_atomic< bit_block_type >> m_blocks;
			bc_ptr<bcUBYTE> m_heap;
		};

		template< typename T >
		class bc_concurrent_object_pool : public bc_initializable<bcUINT32, bc_alloc_type>
		{
		public:
			bc_concurrent_object_pool();

			bc_concurrent_object_pool(bc_concurrent_object_pool&& p_other) noexcept;

			~bc_concurrent_object_pool();

			bc_concurrent_object_pool& operator=(bc_concurrent_object_pool&& p_other) noexcept;

			bcUINT32 block_size() const;

			bcUINT32 num_block() const;

			/**
			 * \brief Try to allocate object from pool or in case of not enough memory use global memory
			 * \tparam TArgs 
			 * \param p_args 
			 * \return Allocated object
			 */
			template< typename ...TArgs >
			T* alloc(TArgs&&... p_args) noexcept(std::is_nothrow_constructible< T, TArgs... >::value);

			void free(T* p_pointer) noexcept;

			void clear() noexcept;

		private:
			void _initialize(bcUINT32 p_objects_count, bc_alloc_type p_alloc_type);

			void _destroy() override;

			void _destruct_objects() const;

			bc_alloc_type m_alloc_type;
			bc_concurrent_memory_pool m_memory_pool;
		};

		inline bcUINT32 bc_concurrent_memory_pool::capacity() const noexcept
		{
			return m_num_block;
		}
		
		inline bcUINT32 bc_concurrent_memory_pool::entry_size() const noexcept
		{
			return m_block_size;
		}

		template< typename T >
		bc_concurrent_object_pool< T >::bc_concurrent_object_pool() = default;

		template< typename T >
		bc_concurrent_object_pool< T >::bc_concurrent_object_pool(bc_concurrent_object_pool&& p_other) noexcept
			: bc_initializable(std::move(p_other)),
			m_alloc_type(p_other.m_alloc_type),
			m_memory_pool(std::move(p_other.m_memory_pool))
		{
		}

		template< typename T >
		bc_concurrent_object_pool< T >::~bc_concurrent_object_pool()
		{
			if (m_initialized)
			{
				destroy();
			}
		}

		template< typename T >
		bc_concurrent_object_pool< T >& bc_concurrent_object_pool< T >::operator=(bc_concurrent_object_pool&& p_other) noexcept
		{
			bc_initializable::operator=(std::move(p_other));
			m_alloc_type = p_other.m_alloc_type;
			m_memory_pool = std::move(p_other.m_memory_pool);

			return *this;
		}

		template< typename T >
		bcUINT32 bc_concurrent_object_pool< T >::block_size() const
		{
			return m_memory_pool.entry_size();
		}

		template< typename T >
		bcUINT32 bc_concurrent_object_pool< T >::num_block() const
		{
			return m_memory_pool.capacity();
		}

		template< typename T >
		template< typename ... TArgs >
		T* bc_concurrent_object_pool< T >::alloc(TArgs&&... p_args) noexcept(std::is_nothrow_constructible< T, TArgs... >::value)
		{
			static_assert(std::is_constructible< T, TArgs... >::value, "class T is not constructable with provided parameters");

			T* l_result = static_cast<T*>(m_memory_pool.alloc());

			if (!l_result)
			{
				l_result = static_cast<T*>(BC_ALLOC_THROW(sizeof(T), m_alloc_type));
			}

			new(l_result)T(std::forward< TArgs >(p_args)...);

			return l_result;
		}

		template< typename T >
		void bc_concurrent_object_pool< T >::free(T* p_pointer) noexcept
		{
			p_pointer->~T();

			if (m_memory_pool.contain_pointer(p_pointer))
			{
				m_memory_pool.free(p_pointer);
			}
			else
			{
				BC_DELETE(p_pointer);
			}
		}

		template< typename T >
		void bc_concurrent_object_pool< T >::clear() noexcept
		{
			_destruct_objects();
			m_memory_pool.clear();
		}

		template< typename T >
		void bc_concurrent_object_pool< T >::_initialize(bcUINT32 p_objects_count, bc_alloc_type p_alloc_type)
		{
			// Because this class has fallback allocation routines any movable allocation will lead to dangled pointer
			if(p_alloc_type == bc_alloc_type::unknown_movable)
			{
				throw bc_invalid_argument_exception("unknown_movable allocation is not supported in bc_concurrent_object_pool");
			}
			
			m_alloc_type = p_alloc_type;
			m_memory_pool.initialize(p_objects_count, sizeof(T), p_alloc_type);
		}

		template< typename T >
		void bc_concurrent_object_pool< T >::_destroy()
		{
			_destruct_objects();
			m_memory_pool.destroy();
		}

		template< typename T >
		void bc_concurrent_object_pool< T >::_destruct_objects() const
		{
			for (bcUINT32 l_i = 0; l_i < m_memory_pool.m_num_bit_blocks; ++l_i)
			{
				const auto l_current_block = m_memory_pool.m_blocks[l_i].load(core_platform::bc_memory_order::relaxed);
				for (bcUINT32 l_j = 0; l_j < bc_concurrent_memory_pool::s_bit_block_size; ++l_j)
				{
					const bool l_is_alive = l_current_block & (bc_concurrent_memory_pool::bit_block_type(1) << l_j);
					if (l_is_alive)
					{
						const bcSIZE l_object_pos = l_i * bc_concurrent_memory_pool::s_bit_block_size + l_j;
						reinterpret_cast< T* >(static_cast<bcUBYTE*>(m_memory_pool.m_heap) + l_object_pos)->~T();
					}
				}
			}
		}
	}
}