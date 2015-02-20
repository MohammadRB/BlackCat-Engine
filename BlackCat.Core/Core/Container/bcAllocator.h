// [12/19/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Utility/bcDefaultMemberType.h"
#include "Core/Memory/bcAlloc.h"

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

			bcInline static void destruct(allocator_type& p_allocator, value_type* p_pointer)
			{
				p_allocator.destruct(p_pointer);
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
		struct bc_allocator_base
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

			pointer allocate(size_type p_count)
			{
				if (TAlignment <= BC_MEMORY_MIN_ALIGN)
					return static_cast<pointer>(bcAllocThrow(sizeof(value_type)* p_count, TAllocType));
				return static_cast<pointer>(bcAlignedAllocThrow(sizeof(value_type)* p_count, TAlignment, TAllocType));
			}

			void deallocate(pointer p_pointer) noexcept(true)
			{
				if (TAlignment <= BC_MEMORY_MIN_ALIGN)
					return bcFree(p_pointer);
				return bcAlignedFree(p_pointer);
			}

			template< typename ...TArgs >
			void construct(value_type* p_pointer, TArgs&&... p_args)
				noexcept(std::is_nothrow_constructible<value_type, TArgs...>::value)
			{
				new(p_pointer)value_type(std::forward< TArgs >(p_args)...);
			}

			void destruct(value_type* p_pointer)
				noexcept(std::is_nothrow_destructible<value_type>::value)
			{
				p_pointer->~value_type();
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
		using b_allocator_level = bc_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::level >;

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

		template< typename T, bcUINT32 TAlignment, bc_alloc_type TAllocType >
		struct bc_std_allocator_base
		{
		public:
			using this_type = bc_std_allocator_base< T, TAlignment, TAllocType >;
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
				using other = bc_std_allocator_base< TOther, TAlignment, TAllocType >;
			};

		public:
			bc_std_allocator_base() noexcept(true) {}

			bc_std_allocator_base(const this_type&) noexcept(true) {}

			template< typename TOther >
			bc_std_allocator_base(const bc_std_allocator_base< TOther, TAlignment, TAllocType >&) noexcept(true) {}

			~bc_std_allocator_base() = default;

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

			void deallocate(pointer p_pointer, size_type n)
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

		protected:

		private:

		};

		template< typename T1,
			bcUINT32 TAlignment1,
			bc_alloc_type TAllocType1,
			typename T2,
			bcUINT32 TAlignment2,
			bc_alloc_type TAllocType2 >
		bool operator ==(const bc_std_allocator_base< T1, TAlignment1, TAllocType1 >&,
			const bc_std_allocator_base< T2, TAlignment2, TAllocType2 >&)
		{
			return true;
		}

		template< typename T1,
			bcUINT32 TAlignment1,
			bc_alloc_type TAllocType1,
			typename T2,
			bcUINT32 TAlignment2,
			bc_alloc_type TAllocType2 >
		bool operator !=(const bc_std_allocator_base< T1, TAlignment1, TAllocType1 >&,
			const bc_std_allocator_base< T2, TAlignment2, TAllocType2 >&)
		{
			return false;
		}

		template< typename T >
		using bc_std_allocator_program = bc_std_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::program >;

		template< typename T >
		using b_std_allocator_level = bc_std_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::level >;

		template< typename T >
		using bc_std_allocator_frame = bc_std_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::frame >;

		template< typename T >
		using bc_std_allocator = bc_std_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::unknown >;

		template< typename T >
		using bc_std_allocator_movable = bc_std_allocator_base< T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::unknown_movale >;

		template< typename T, bcUINT32 TAlign >
		using bc_std_aligned_allocator_program = bc_std_allocator_base< T, TAlign, bc_alloc_type::program >;

		template< typename T, bcUINT32 TAlign >
		using bc_std_aligned_allocator_level = bc_std_allocator_base< T, TAlign, bc_alloc_type::level >;

		template< typename T, bcUINT32 TAlign >
		using bc_std_aligned_allocator_frame = bc_std_allocator_base< T, TAlign, bc_alloc_type::frame >;

		template< typename T, bcUINT32 TAlign >
		using bc_std_aligned_allocator = bc_std_allocator_base< T, TAlign, bc_alloc_type::unknown >;

		template< typename T, bcUINT32 TAlign >
		using bc_std_aligned_allocator_movable = bc_std_allocator_base< T, TAlign, bc_alloc_type::unknown_movale >;
	}
}
