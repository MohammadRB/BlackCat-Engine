// [12/19/2014 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/CorePCH.h"
#include "Core/Utility/bcTemplateMetaType.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Memory/bcPtr.h"

namespace black_cat
{
	namespace core
	{
		template<typename T>
		struct bc_allocator_traits
		{
		public:
			using this_type = bc_allocator_traits;
			using allocator_type = T;
			using value_type = typename allocator_type::value_type;
			using pointer = typename bc_get_pointer_type<allocator_type>::type;
			using const_pointer = typename bc_get_const_pointer_type<allocator_type>::type;
			using reference = typename bc_get_reference_type<allocator_type>::type;
			using const_reference = typename bc_get_const_reference_type<allocator_type>::type;
			using difference_type = typename bc_get_difference_type<allocator_type>::type;
			using size_type = typename bc_get_size_type<allocator_type>::type;
			using is_movable_type = typename allocator_type::is_movable_type;

			template<typename TOther>
			struct rebind_alloc
			{
				using other = typename allocator_type::template rebind<TOther>::other;
			};

			template<typename TOther>
			struct rebind
			{
				using other = bc_allocator_traits<typename rebind_alloc<TOther>::other>;
			};

		public:
			static pointer allocate(allocator_type& p_allocator, size_type p_count, const void* p_hint = nullptr)
			{
				return p_allocator.allocate(p_count, p_hint);
			}

			static void deallocate(allocator_type& p_allocator, pointer p_pointer)
			{
				p_allocator.deallocate(p_pointer);
			}

			template<typename ...TArgs>
			static void construct(allocator_type& p_allocator, value_type* p_pointer, TArgs&&... p_args) noexcept(std::is_nothrow_constructible_v<value_type, TArgs...>)
			{
				p_allocator.construct(p_pointer, std::forward<TArgs>(p_args)...);
			}

			static void destroy(allocator_type& p_allocator, value_type* p_pointer)
			{
				p_allocator.destroy(p_pointer);
			}

			static void register_pointer(allocator_type& p_allocator, pointer* p_pointer)
			{
				_register_pointer(p_allocator, p_pointer, is_movable_type());
			}

			static void unregister_pointer(allocator_type& p_allocator, pointer* p_pointer)
			{
				_unregister_pointer(p_allocator, p_pointer, is_movable_type());
			}

			template<typename TOther>
			static bool equal(const this_type& p_first, const typename rebind_alloc<TOther>::other& p_second)
			{
				return p_first == p_second;
			}

			template<typename TOther>
			static bool not_equal(const this_type& p_first, const typename rebind_alloc<TOther>::other& p_second)
			{
				return p_first != p_second;
			}

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

		template<typename T, bcUINT32 TAlignment, bc_alloc_type TAllocType>
		class bc_allocator_base
		{
		public:
			using this_type = bc_allocator_base<T, TAlignment, TAllocType>;
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
			using is_movable_type = std::conditional_t<TAllocType == bc_alloc_type::unknown_movable, std::true_type, std::false_type>;

			template<typename TOther>
			struct rebind
			{
				using other = bc_allocator_base<TOther, TAlignment, TAllocType>;
			};

		public:
			bc_allocator_base() noexcept = default;

			bc_allocator_base(const this_type&) noexcept = default;

			template<typename TOther>
			bc_allocator_base(const bc_allocator_base<TOther, TAlignment, TAllocType>&) noexcept
			{
			}

			bc_allocator_base(this_type&&) noexcept = default;

			~bc_allocator_base() = default;

			this_type& operator =(const this_type&) noexcept = default;

			template<typename TOther>
			this_type& operator =(const bc_allocator_base<TOther, TAlignment, TAllocType>&) noexcept
			{
				return *this;
			}

			this_type& operator =(this_type&&) noexcept = default;

			pointer address(reference p_object) const noexcept
			{
				return std::addressof(p_object);
			}

			const_pointer address(const_reference p_object) const noexcept
			{
				return std::addressof(p_object);
			}

			pointer allocate(size_type p_count, const void* p_hint = nullptr)
			{
				if (TAlignment <= BC_MEMORY_MIN_ALIGN)
				{
					return static_cast<pointer>(BC_ALLOC_THROW(sizeof(value_type)* p_count, TAllocType));
				}
				return static_cast<pointer>(BC_ALIGNED_ALLOC_THROW(sizeof(value_type)* p_count, TAlignment, TAllocType));
			}

			// Parameter n is for compatibility with std allocators
			void deallocate(pointer p_pointer, size_type n = 0) noexcept
			{
				if (TAlignment <= BC_MEMORY_MIN_ALIGN)
				{
					BC_FREE(p_pointer);
					return;
				}

				BC_ALIGNED_FREE(p_pointer);
			}

			template<class TU, class... TArgs>
			void construct(TU* p_pointer, TArgs&&... p_args)
			{
				new(p_pointer)TU(std::forward<TArgs>(p_args)...);
			}

			template<class TU>
			void destroy(TU* p_pointer)
			{
				p_pointer->~TU();
			}

			size_type max_size() const noexcept
			{
				return (std::numeric_limits<size_type>::max)();
			}

			void register_pointer(pointer* p_pointer) noexcept
			{
				_register_pointer(p_pointer, is_movable_type());
			}

			void unregister_pointer(pointer* p_pointer) noexcept
			{
				_unregister_pointer(p_pointer, is_movable_type());
			}

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

		template
		<
			typename T1,
			bcUINT32 TAlignment1,
			bc_alloc_type TAllocType1,
			typename T2,
			bcUINT32 TAlignment2,
			bc_alloc_type TAllocType2 
		>
		bool operator ==(const bc_allocator_base<T1, TAlignment1, TAllocType1>&, const bc_allocator_base<T2, TAlignment2, TAllocType2>&)
		{
			return true;
		}

		template
		<
			typename T1,
			bcUINT32 TAlignment1,
			bc_alloc_type TAllocType1,
			typename T2,
			bcUINT32 TAlignment2,
			bc_alloc_type TAllocType2 
		>
		bool operator !=(const bc_allocator_base<T1, TAlignment1, TAllocType1>&, const bc_allocator_base<T2, TAlignment2, TAllocType2>&)
		{
			return false;
		}

		template<typename T>
		using bc_allocator_program = bc_allocator_base<T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::program>;

		template<typename T>
		using bc_allocator_frame = bc_allocator_base<T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::frame>;

		template<typename T>
		using bc_allocator = bc_allocator_base<T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::unknown>;

		template<typename T>
		using bc_allocator_movable = bc_allocator_base<T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::unknown_movable>;

		template<typename T, bcUINT32 TAlign>
		using bc_aligned_allocator_program = bc_allocator_base<T, TAlign, bc_alloc_type::program>;

		template<typename T, bcUINT32 TAlign>
		using bc_aligned_allocator_frame = bc_allocator_base<T, TAlign, bc_alloc_type::frame>;

		template<typename T, bcUINT32 TAlign>
		using bc_aligned_allocator = bc_allocator_base<T, TAlign, bc_alloc_type::unknown>;

		template<typename T, bcUINT32 TAlign>
		using bc_aligned_allocator_movable = bc_allocator_base<T, TAlign, bc_alloc_type::unknown_movable>;

		template<typename T>
		class bc_runtime_allocator
		{
		private:
			using allocator_type = bc_allocator_base<T, BC_MEMORY_MIN_ALIGN, bc_alloc_type::unknown>;
			template<typename>
			friend class bc_runtime_allocator;

		public:
			using value_type = typename allocator_type::value_type;
			using pointer = typename allocator_type::pointer;
			using const_pointer = typename allocator_type::const_pointer;
			using reference = typename allocator_type::reference;
			using const_reference = typename allocator_type::const_reference;
			using difference_type = typename allocator_type::difference_type;
			using size_type = typename allocator_type::size_type;
			using propagate_on_container_copy_assignment = std::true_type;
			using propagate_on_container_move_assignment = std::true_type;
			using propagate_on_container_swap = std::true_type;
			using is_movable_type = typename allocator_type::is_movable_type;
			
			template<typename TOther>
			struct rebind
			{
				using other = bc_runtime_allocator<TOther>;
			};

		public:
			explicit bc_runtime_allocator(bc_alloc_type p_alloc_type, bcUINT p_alignment = BC_MEMORY_MIN_ALIGN)
				: m_alloc_type(p_alloc_type), 
				m_alignment(p_alignment)
			{
			}

			bc_runtime_allocator(const bc_runtime_allocator&) noexcept = default;

			template<typename TOther>
			bc_runtime_allocator(const bc_runtime_allocator<TOther>& p_other) noexcept
				: m_alloc_type(p_other.m_alloc_type),
				m_alignment(p_other.m_alignment)
			{
			}

			~bc_runtime_allocator() = default;

			bc_runtime_allocator& operator =(const bc_runtime_allocator&) noexcept = default;

			template<typename TOther>
			bc_runtime_allocator& operator =(const bc_runtime_allocator<TOther>& p_other) noexcept
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

			pointer address(reference p_object) const noexcept
			{
				return std::addressof(p_object);
			}

			const_pointer address(const_reference p_object) const noexcept
			{
				return std::addressof(p_object);
			}

			pointer allocate(size_type p_count, const void* p_hint = nullptr)
			{
				if (m_alignment <= BC_MEMORY_MIN_ALIGN)
				{
					return static_cast<pointer>(BC_ALLOC_THROW(sizeof(value_type)* p_count, m_alloc_type));
				}
				return static_cast<pointer>(BC_ALIGNED_ALLOC_THROW(sizeof(value_type)* p_count, m_alignment, m_alloc_type));
			}

			void deallocate(pointer p_pointer, size_type p_n = 0) noexcept
			{
				if (m_alignment <= BC_MEMORY_MIN_ALIGN)
				{
					return BC_FREE(p_pointer);
				}
				return BC_ALIGNED_FREE(p_pointer);
			}

			template<class TU, class... TArgs>
			void construct(TU* p_pointer, TArgs&&... p_args)
			{
				new(p_pointer)TU(std::forward<TArgs>(p_args)...);
			}

			template<class TU>
			void destroy(TU* p_pointer)
			{
				p_pointer->~TU();
			}

			size_type max_size() const noexcept
			{
				return (std::numeric_limits<size_type>::max)();
			}

			void register_pointer(pointer* p_pointer) noexcept
			{
				if(m_alloc_type == bc_alloc_type::unknown_movable)
				{
					register_movable_pointer(reinterpret_cast<void**>(p_pointer));
				}
			}

			void unregister_pointer(pointer* p_pointer) noexcept
			{
				if (m_alloc_type == bc_alloc_type::unknown_movable)
				{
					unregister_movable_pointer(reinterpret_cast<void**>(p_pointer));
				}
			}

		private:
			bc_alloc_type m_alloc_type;
			bcUINT m_alignment;
		};

		template<typename T1, typename T2>
		bool operator ==(const bc_runtime_allocator<T1>& p_1, const bc_runtime_allocator<T2>& p_2)
		{
			return p_1.get_alignment() == p_2.get_alignment() && p_1.get_alloc_type() == p_2.get_alloc_type();
		}

		template<typename T1, typename T2>
		bool operator !=(const bc_runtime_allocator<T1>& p_1, const bc_runtime_allocator<T2>& p_2)
		{
			return !(p_1 == p_2);
		}

		/**
		 * \brief Provide an interface for classes that use memory allocation and their clients need to change allocation properties.
		 * (Don't use movable memory to allocate objects of this type)
		 */
		class bc_object_allocator
		{
		public:
			explicit bc_object_allocator(bc_alloc_type p_alloc_type = bc_alloc_type::unknown, bcUINT16 p_alignment = BC_MEMORY_MIN_ALIGN)
				: m_flags(0)
			{
				set_allocator_alloc_type(p_alloc_type);
				set_allocator_alignment(p_alignment);
			}

			bc_object_allocator(const bc_object_allocator& p_other)
			{
				set_allocator_alloc_type(p_other.get_allocator_alloc_type());
				set_allocator_alignment(p_other.get_allocator_alignment());
			}

			bc_object_allocator(bc_object_allocator&& p_other) noexcept
			{
				set_allocator_alloc_type(p_other.get_allocator_alloc_type());
				set_allocator_alignment(p_other.get_allocator_alignment());
			}

			~bc_object_allocator() = default;

			bc_object_allocator& operator=(const bc_object_allocator& p_other)
			{
				set_allocator_alloc_type(p_other.get_allocator_alloc_type());
				set_allocator_alignment(p_other.get_allocator_alignment());

				return *this;
			}

			bc_object_allocator& operator=(bc_object_allocator&& p_other) noexcept
			{
				set_allocator_alloc_type(p_other.get_allocator_alloc_type());
				set_allocator_alignment(p_other.get_allocator_alignment());

				return *this;
			}

			bc_alloc_type get_allocator_alloc_type() const
			{
				return static_cast<bc_alloc_type>(m_flags.load(platform::bc_memory_order::relaxed) & s_alloc_type_mask);
			}

			bc_alloc_type set_allocator_alloc_type(bc_alloc_type p_alloc_type)
			{
				const bc_alloc_type l_alloc_type = get_allocator_alloc_type();
				const bcUINT16 l_alignment = get_allocator_alignment();
				const bcUINT32 l_value_to_store = static_cast<bcUINT32>((l_alignment <<8) | static_cast<bcUINT32>(p_alloc_type));

				m_flags.store(l_value_to_store, platform::bc_memory_order::relaxed);
				
				return l_alloc_type;
			}

			bcUINT16 get_allocator_alignment() const
			{
				return static_cast<bcUINT16>((m_flags.load(platform::bc_memory_order::relaxed) & s_alignment_mask)>> 8);
			}

			bcUINT16 set_allocator_alignment(bcUINT16 p_alignment)
			{
				const bc_alloc_type l_alloc_type = get_allocator_alloc_type();
				const bcUINT16 l_alignment = get_allocator_alignment();
				const bcUINT32 l_value_to_store = static_cast<bcUINT32>((p_alignment <<8) | static_cast<bcUINT32>(l_alloc_type));

				m_flags.store(l_value_to_store, platform::bc_memory_order::relaxed);

				return l_alignment;
			}

		protected:
			// Allocate memory for an object and construct it
			template<typename T, typename ...TArgs>
			bc_unique_ptr<T> allocate(TArgs&&... p_args) const
			{
				bc_alloc_type l_alloc_type = get_allocator_alloc_type();
				bcUINT16 l_alignment = get_allocator_alignment();

				return bc_make_unique<T>(l_alloc_type, l_alignment, std::forward<TArgs>(p_args)...);
			}

			// Used to allocate raw memory
			bc_unique_ptr<bcBYTE> allocate_raw(bcUINT p_num) const
			{
				bc_alloc_type l_alloc_type = get_allocator_alloc_type();
				bcUINT16 l_alignment = get_allocator_alignment();
				void* l_memory;

				if(get_allocator_alignment()> BC_MEMORY_MIN_ALIGN)
				{
					l_memory = BC_ALIGNED_ALLOC(p_num, l_alignment, l_alloc_type);
				}
				else
				{
					l_memory = BC_ALLOC(p_num, l_alloc_type);
				}

				return bc_unique_ptr<bcBYTE>(reinterpret_cast<bcBYTE*>(l_memory));
			}

			// Destruct object and deallocate it's memory
			template<typename T>
			static void deallocate(T* p_pointer)
			{
				BC_DELETE(p_pointer);
			}

			// Used to deallocate raw memory
			static void deallocate_raw(bcBYTE* p_pointer)
			{
				BC_FREE(p_pointer);
			}

		private:
			static constexpr bcUINT32 s_alloc_type_mask = 0x000000ff;
			static constexpr bcUINT32 s_alignment_mask = 0x00ffff00;

			platform::bc_atomic<bcUINT32> m_flags;
		};
	}
}