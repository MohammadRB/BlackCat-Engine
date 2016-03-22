// [12/19/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Utility/bcTemplateMetaType.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Utility/bcDelegate.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		struct bc_allocator_traits
		{
		public:
			using this_type = bc_allocator_traits;
			using allocator_type = T;
			using value_type = typename allocator_type::value_type;
			using pointer = typename bc_get_pointer_type< allocator_type >::type;
			using const_pointer = typename bc_get_const_pointer_type< allocator_type >::type;
			using reference = typename bc_get_reference_type<allocator_type>::type;
			using const_reference = typename bc_get_const_reference_type<allocator_type>::type;
			using difference_type = typename bc_get_difference_type< allocator_type >::type;
			using size_type = typename bc_get_size_type< allocator_type >::type;
			using is_movable_type = typename allocator_type::is_movable_type;

			template< typename TOther >
			struct rebind_alloc
			{
				using other = typename allocator_type::template rebind<TOther>::other;
			};

			template< typename TOther >
			struct rebind
			{
				using other = bc_allocator_traits< typename rebind_alloc< TOther >::other >;
			};

		public:
			bcInline static pointer allocate(allocator_type& p_allocator, size_type p_count)
			{
				return p_allocator.allocate(p_count);
			}

			bcInline static void deallocate(allocator_type& p_allocator, pointer p_pointer)
			{
				p_allocator.deallocate(p_pointer);
			}

			template< typename ...TArgs >
			bcInline static void construct(allocator_type& p_allocator, value_type* p_pointer, TArgs&&... p_args)
				noexcept(std::is_nothrow_constructible<value_type, TArgs...>::value)
			{
				p_allocator.construct(p_pointer, std::forward< TArgs >(p_args)...);
			}

			bcInline static void destroy(allocator_type& p_allocator, value_type* p_pointer)
			{
				p_allocator.destroy(p_pointer);
			}

			bcInline static void register_pointer(allocator_type& p_allocator, pointer* p_pointer)
			{
				_register_pointer(p_allocator, p_pointer, is_movable_type());
			}

			bcInline static void unregister_pointer(allocator_type& p_allocator, pointer* p_pointer)
			{
				_unregister_pointer(p_allocator, p_pointer, is_movable_type());
			}

			template< typename TOther >
			bcInline static bool equal(const this_type& p_first, const typename rebind_alloc< TOther >::other& p_second)
			{
				return p_first == p_second;
			}

			template< typename TOther >
			bcInline static bool not_equal(const this_type& p_first, const typename rebind_alloc< TOther >::other& p_second)
			{
				return p_first != p_second;
			}

		protected:
			
		private:
			static void _register_pointer(allocator_type& p_allocator, pointer* p_pointer, std::true_type)
			{
				p_allocator.register_pointer(p_pointer);
			}

			static void _register_pointer(allocator_type& p_allocator, pointer* p_pointer, std::false_type)
			{
			}

			static void _unregister_pointer(allocator_type& p_allocator, pointer* p_pointer, std::true_type)
			{
				p_allocator.unregister_pointer(p_pointer);
			}

			static void _unregister_pointer(allocator_type& p_allocator, pointer* p_pointer, std::false_type)
			{
			}

		};

		template< typename T, bcUINT32 TAlignment, bc_alloc_type TAllocType >
		class bc_allocator_base
		{
		public:
			using this_type = bc_allocator_base< T, TAlignment, TAllocType >;
			using value_type = T;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using reference = value_type&;
			using const_reference = const value_type&;
			using difference_type = bcPTRDIFF;
			using size_type = bcSIZE;
			using propagate_on_container_copy_assignment = std::false_type;
			using propagate_on_container_move_assignment = std::false_type;
			using propagate_on_container_swap = std::false_type;
			using is_movable_type = typename std::conditional< TAllocType == bc_alloc_type::unknown_movale, std::true_type, std::false_type >::type;

			template< typename TOther >
			struct rebind
			{
				using other = bc_allocator_base< TOther, TAlignment, TAllocType >;
			};

		public:
			bc_allocator_base() noexcept(true) = default;

			bc_allocator_base(const this_type&) noexcept(true) = default;

			template< typename TOther >
			bc_allocator_base(const bc_allocator_base< TOther, TAlignment, TAllocType >&) noexcept(true)
			{
			}

			bc_allocator_base(this_type&&) noexcept(true) = default;

			~bc_allocator_base() = default;

			this_type& operator =(const this_type&) noexcept(true) = default;

			template< typename TOther >
			this_type& operator =(const bc_allocator_base< TOther, TAlignment, TAllocType >&) noexcept(true)
			{
				return *this;
			}

			this_type& operator =(this_type&&) noexcept(true) = default;

			pointer address(reference p_object) const noexcept(true)
			{
				return std::addressof(p_object);
			}

			const_pointer address(const_reference p_object) const noexcept(true)
			{
				return std::addressof(p_object);
			}

			pointer allocate(size_type p_count, std::allocator<void>::const_pointer p_hint = nullptr)
			{
				if (TAlignment <= BC_MEMORY_MIN_ALIGN)
					return static_cast<pointer>(bcAllocThrow(sizeof(value_type)* p_count, TAllocType));
				return static_cast<pointer>(bcAlignedAllocThrow(sizeof(value_type)* p_count, TAlignment, TAllocType));
			}

			// Parameter n is for compatibility with std allocators
			void deallocate(pointer p_pointer, size_type n = 0) noexcept(true)
			{
				if (TAlignment <= BC_MEMORY_MIN_ALIGN)
					return bcFree(p_pointer);
				return bcAlignedFree(p_pointer);
			}

			template< class TU, class... TArgs >
			void construct(TU* p_pointer, TArgs&&... p_args)
			{
				new(p_pointer)TU(std::forward< TArgs >(p_args)...);
			}

			template< class TU >
			void destroy(TU* p_pointer)
			{
				p_pointer->~TU();
			}

			size_type max_size() const noexcept(true)
			{
				return (std::numeric_limits<size_type>::max)();
			}

			void register_pointer(pointer* p_pointer) noexcept(true)
			{
				_register_pointer(p_pointer, is_movable_type());
			}

			void unregister_pointer(pointer* p_pointer) noexcept(true)
			{
				_unregister_pointer(p_pointer, is_movable_type());
			}

		protected:

		private:
			void _register_pointer(pointer* p_pointer, std::true_type)
			{
				register_movable_pointer(reinterpret_cast<void**>(p_pointer));
			}

			void _register_pointer(pointer* p_pointer, std::false_type)
			{
			}

			void _unregister_pointer(pointer* p_pointer, std::true_type)
			{
				unregister_movable_pointer(reinterpret_cast<void**>(p_pointer));
			}

			void _unregister_pointer(pointer* p_pointer, std::false_type)
			{
			}

		};

		template< typename T1,
			bcUINT32 TAlignment1,
			bc_alloc_type TAllocType1,
			typename T2,
			bcUINT32 TAlignment2,
			bc_alloc_type TAllocType2 >
		bool operator ==(const bc_allocator_base< T1, TAlignment1, TAllocType1 >&,
			const bc_allocator_base< T2, TAlignment2, TAllocType2 >&)
		{
			return true;
		}

		template< typename T1,
			bcUINT32 TAlignment1,
			bc_alloc_type TAllocType1,
			typename T2,
			bcUINT32 TAlignment2,
			bc_alloc_type TAllocType2 >
			bool operator !=(const bc_allocator_base< T1, TAlignment1, TAllocType1 >&,
			const bc_allocator_base< T2, TAlignment2, TAllocType2 >&)
		{
			return false;
		}

		template< typename T >
		using bc_allocator_program = bc_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::program >;

		template< typename T >
		using bc_allocator_level = bc_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::level >;

		template< typename T >
		using bc_allocator_frame = bc_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::frame >;

		template< typename T >
		using bc_allocator = bc_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::unknown >;

		template< typename T >
		using bc_allocator_movable = bc_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::unknown_movale >;

		template< typename T, bcUINT32 TAlign >
		using bc_aligned_allocator_program = bc_allocator_base< T, TAlign, bc_alloc_type::program >;

		template< typename T, bcUINT32 TAlign >
		using bc_aligned_allocator_level = bc_allocator_base< T, TAlign, bc_alloc_type::level >;

		template< typename T, bcUINT32 TAlign >
		using bc_aligned_allocator_frame = bc_allocator_base< T, TAlign, bc_alloc_type::frame >;

		template< typename T, bcUINT32 TAlign >
		using bc_aligned_allocator = bc_allocator_base< T, TAlign, bc_alloc_type::unknown >;

		template< typename T, bcUINT32 TAlign >
		using bc_aligned_allocator_movable = bc_allocator_base< T, TAlign, bc_alloc_type::unknown_movale >;

		template< typename T >
		class bc_runtime_allocator
		{
		private:
			using allocator_type = bc_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::unknown >;

		public:
			using value_type = typename allocator_type::value_type;
			using pointer = typename allocator_type::pointer;
			using const_pointer = typename allocator_type::const_pointer;
			using reference = typename allocator_type::reference;
			using const_reference = typename allocator_type::const_reference;
			using difference_type = typename allocator_type::difference_type;
			using size_type = typename allocator_type::size_type;
			using propagate_on_container_copy_assignment = std::false_type;
			using propagate_on_container_move_assignment = std::false_type;
			using propagate_on_container_swap = std::false_type;

			template< typename TOther >
			struct rebind
			{
				using other = bc_runtime_allocator< TOther >;
			};

		public:
			explicit bc_runtime_allocator(bc_alloc_type p_alloc_type, bcUINT p_alignment = BC_MEMORY_MIN_ALIGN)
				: m_alloc_type(p_alloc_type), 
				m_alignment(p_alignment)
			{
			}

			bc_runtime_allocator(const bc_runtime_allocator&) noexcept = default;

			template< typename TOther >
			bc_runtime_allocator(const bc_runtime_allocator< TOther >& p_other) noexcept
				: m_alloc_type(p_other.m_alloc_type),
				m_alignment(p_other.m_alignment)
			{
			}

			~bc_runtime_allocator() = default;

			bc_runtime_allocator& operator =(const bc_runtime_allocator&) noexcept = default;

			template< typename TOther >
			bc_runtime_allocator& operator =(const bc_runtime_allocator< TOther >& p_other) noexcept
			{
				m_alloc_type = p_other.m_alloc_type;
				m_alignment = p_other.m_alignment;

				return *this;
			}

			bc_alloc_type get_alloc_type()const
			{
				return m_alloc_type;
			}

			void set_alloc_type(bc_alloc_type p_alloc_type)
			{
				m_alloc_type = p_alloc_type;
			}

			bcUINT get_alignment() const
			{
				return m_alignment;
			}

			void set_alignment(bcUINT p_alignment)
			{
				m_alignment = p_alignment;
			}

			pointer address(reference p_object) const noexcept(true)
			{
				return std::addressof(p_object);
			}

			const_pointer address(const_reference p_object) const noexcept(true)
			{
				return std::addressof(p_object);
			}

			pointer allocate(size_type p_count, std::allocator<void>::const_pointer p_hint = nullptr)
			{
				if (m_alignment <= BC_MEMORY_MIN_ALIGN)
					return static_cast<pointer>(bcAllocThrow(sizeof(value_type)* p_count, m_alloc_type));
				return static_cast<pointer>(bcAlignedAllocThrow(sizeof(value_type)* p_count, m_alignment, m_alloc_type));
			}

			void deallocate(pointer p_pointer, size_type n = 0) noexcept(true)
			{
				if (m_alignment <= BC_MEMORY_MIN_ALIGN)
					return bcFree(p_pointer);
				return bcAlignedFree(p_pointer);
			}

			template< class TU, class... TArgs >
			void construct(TU* p_pointer, TArgs&&... p_args)
			{
				new(p_pointer)TU(std::forward< TArgs >(p_args)...);
			}

			template< class TU >
			void destroy(TU* p_pointer)
			{
				p_pointer->~TU();
			}

			size_type max_size() const noexcept(true)
			{
				return (std::numeric_limits<size_type>::max)();
			}

			void register_pointer(pointer* p_pointer) noexcept(true)
			{
				if(m_alloc_type == bc_alloc_type::unknown_movale)
					register_movable_pointer(reinterpret_cast<void**>(p_pointer));
			}

			void unregister_pointer(pointer* p_pointer) noexcept(true)
			{
				if (m_alloc_type == bc_alloc_type::unknown_movale)
					unregister_movable_pointer(reinterpret_cast< void** >(p_pointer));
			}

		protected:

		private:
			bc_alloc_type m_alloc_type;
			bcUINT m_alignment;
		};

		// Provide an interface for classes that use dynamic memory and their clients to change allocation properties
		class bc_object_allocator
		{
		public:
			template<typename T>
			using ptr = bc_unique_ptr<T, bc_delegate<void(T*)>>;

		public:
			explicit bc_object_allocator(bc_alloc_type p_alloc_type = bc_alloc_type::unknown, bcUINT p_alignment = BC_MEMORY_MIN_ALIGN)
				: m_alloc_type(p_alloc_type),
				m_alignment(p_alignment)
			{	
			}

			bc_alloc_type get_allocator_alloc_type()const
			{
				return m_alloc_type;
			}

			bc_alloc_type set_allocator_alloc_type(bc_alloc_type p_alloc_type)
			{
				bc_alloc_type l_temp = m_alloc_type;
				m_alloc_type = p_alloc_type;
				return l_temp;
			}

			bcUINT get_allocator_alignment() const
			{
				return m_alignment;
			}

			bcUINT set_allocator_alignment(bcUINT p_alignment)
			{
				bcUINT l_temp = m_alignment;
				m_alignment = p_alignment;
				return l_temp;
			}

		protected:
			// Allocate memory for an object and construct it. return type of this function is a unique pointer 
			// that will call deallocate function and there is no need to use deallocate manualy
			template<typename T, typename ...TArgs>
			ptr<T> allocate(TArgs&&... p_args)
			{
				bc_runtime_allocator<T> l_allocator(m_alloc_type, m_alignment);

				T* l_pointer = l_allocator.allocate(1);
				try
				{
					l_allocator.construct(l_pointer, std::forward<TArgs>(p_args)...);
				}
				catch(...)
				{
					l_allocator.deallocate(l_pointer);
					throw;
				}

				bc_delegate<void(T*)> l_deleter(this, &bc_object_allocator::deallocate<T>);
				return ptr<T>(l_pointer, std::move(l_deleter));
			}

			// Destruct object and deallocate it's memory
			template<typename T>
			void deallocate(T* p_pointer)
			{
				bc_runtime_allocator<T> l_allocator(m_alloc_type, m_alignment);

				try
				{
					l_allocator.destroy(p_pointer);
				}
				catch(...)
				{
					l_allocator.deallocate(p_pointer);
					throw;
				}

				l_allocator.deallocate(p_pointer);
			}

			template<typename T>
			void deallocate(ptr<T> p_pointer)
			{
				deallocate(p_pointer.get());
			}

		private:
			bc_alloc_type m_alloc_type;
			bcUINT m_alignment;
		};
	}
}
