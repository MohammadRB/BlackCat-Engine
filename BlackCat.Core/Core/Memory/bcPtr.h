//  [8/9/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Utility/bcDelegate.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_default_deleter
		{
		public:
			constexpr bc_default_deleter() = default;

			template< typename T1 >
			constexpr bc_default_deleter(bc_default_deleter<T1>) {}

			~bc_default_deleter() = default;

			template< typename T1 >
			bc_default_deleter& operator =(bc_default_deleter<T1>) { return *this; }

			void operator()(T* p_pointer)
			{
				bcDelete(p_pointer);
			}
		};

		inline void _register_pointer(void** p_pointer)
		{
			if (is_movale_pointer(*p_pointer))
			{
				register_movable_pointer(p_pointer);
			}	
		}

		inline void _unregister_pointer(void** p_pointer)
		{
			if (is_movale_pointer(*p_pointer))
			{
				unregister_movable_pointer(p_pointer);
			}	
		}

#pragma region bcUniquePtr

		template <typename T, typename TDeleter = bc_default_deleter<T>>
		class bc_unique_ptr
		{
			template< typename T1, typename TDeleter1 >
			friend class bc_unique_ptr;

		public:
			using this_type = bc_unique_ptr;
			using pointer = T*;
			using element_type = T;
			using deleter_type = TDeleter;
			template< typename TOther >
			struct rebind
			{
				using other = bc_unique_ptr< TOther, deleter_type >;
			};

		public:
			constexpr bc_unique_ptr() noexcept(true)
				: m_pointer(nullptr),
				m_deleter()
			{
			}
			
			constexpr bc_unique_ptr(std::nullptr_t) noexcept(true)
				/*: bc_unique_ptr() TODO visual studio 2015 has a bug in constexpr delegating construction */
				: m_pointer(nullptr),
				m_deleter()
			{
			}
			
			explicit bc_unique_ptr(pointer p_pointer) noexcept(true)
				: m_pointer(nullptr),
				m_deleter()
			{
 				_construct(p_pointer);
			}

			bc_unique_ptr(pointer p_pointer, deleter_type p_deleter) noexcept(true)
				: m_pointer(nullptr),
				m_deleter(p_deleter)
			{
				_construct(p_pointer);
			}

			bc_unique_ptr(const this_type& p_other) = delete;

			bc_unique_ptr(this_type&& p_other) noexcept(true)
				: m_pointer(nullptr),
				m_deleter()
			{
				_assign(std::move(p_other));
			}

			template< typename T1, typename TDeleter1 >
			bc_unique_ptr(bc_unique_ptr<T1, TDeleter1>&& p_other) noexcept(true)
				: m_pointer(nullptr),
				m_deleter()
			{
				_assign(std::move(p_other));
			}

			~bc_unique_ptr()
			{
				_destruct();
			}

			this_type& operator =(const this_type& p_other) = delete;

			this_type& operator =(this_type&& p_other) noexcept(true)
			{
				_assign(std::move(p_other));

				return *this;
			}

			template< typename T1, typename TDeleter1 >
			this_type& operator =(bc_unique_ptr<T1, TDeleter1>&& p_other) noexcept(true)
			{
				_assign(std::move(p_other));

				return *this;
			}

			this_type& operator =(std::nullptr_t) noexcept(true)
			{
				_destruct();
				m_pointer = nullptr;

				return *this;
			}

			T* release() noexcept(true)
			{
				T* lPointer = get();
				_unregister_pointer(reinterpret_cast< void** >(&m_pointer));
				m_pointer = nullptr;

				return lPointer;
			}

			void reset() noexcept(true)
			{
				_destruct();
			}

			void reset(T* p_pointer) noexcept(true)
			{
				_destruct();
				_construct(p_pointer);
			}

			void swap(this_type& p_other) noexcept(true)
			{
				using std::swap;

				_unregister_pointer(reinterpret_cast< void** >(&m_pointer));
				_unregister_pointer(reinterpret_cast< void** >(&p_other.m_pointer));

				std::swap(m_pointer, p_other.m_pointer);
				swap(m_deleter, p_other.m_deleter);

				_register_pointer(reinterpret_cast< void** >(&m_pointer));
				_register_pointer(reinterpret_cast< void** >(&p_other.m_pointer));
			}

			T* get() const noexcept(true)
			{
				return m_pointer;
			}

			deleter_type get_deleter() const noexcept(true)
			{
				return m_deleter;
			}

			explicit operator bool() const noexcept(true)
			{
				return m_pointer != nullptr;
			}

			T& operator *() const
			{
				return *m_pointer;
			}

			T* operator ->() const noexcept(true)
			{
				return m_pointer;
			}

		protected:

		private:
			void _construct(T* p_pointer)
			{
				m_pointer = p_pointer;

				if (m_pointer)
				{
					_register_pointer(reinterpret_cast< void** >(&m_pointer));
				}
			}

			void _destruct()
			{
				if (m_pointer != nullptr)
				{
					_unregister_pointer(reinterpret_cast< void** >(&m_pointer));

					m_deleter(m_pointer);
					m_pointer = nullptr;
				}
			}

			template<typename T1, typename TDeleter1>
			void _assign(bc_unique_ptr<T1, TDeleter1>&& p_other)
			{
				if (this != reinterpret_cast<bc_unique_ptr<T, TDeleter>*>(&p_other)) // avoid self assignment
				{
					reset(static_cast<T*>(p_other.release()));
					m_deleter = static_cast< deleter_type >(p_other.get_deleter());
				}
			}

			bcBYTE _compare(const void* p_pointer) const
			{
				return (m_pointer == p_pointer) ? 0 : ((m_pointer > p_pointer) ? 1 : ((m_pointer < p_pointer) ? -1 : 0));
			}

			pointer m_pointer;
			deleter_type m_deleter;
		};

		template<class T1, class D1, class T2, class D2>
		bool operator ==(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
		{
			return p_first.get() == p_second.get();
		};

		template<class T1, class D1, class T2, class D2>
		bool operator !=(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
		{
			return p_first.get() != p_second.get();
		};

		template<class T1, class D1, class T2, class D2>
		bool operator <(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
		{
			return p_first.get() < p_second.get();
		};

		template<class T1, class D1, class T2, class D2>
		bool operator <=(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
		{
			return p_first.get() <= p_second.get();
		};
		
		template<class T1, class D1, class T2, class D2>
		bool operator >(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
		{
			return p_first.get() > p_second.get();
		};
		
		template<class T1, class D1, class T2, class D2>
		bool operator >=(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
		{
			return p_first.get() >= p_second.get();
		};

		template <class T, class D>
		bool operator ==(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
		{
			return p_first.get() == nullptr;
		};

		template <class T, class D>
		bool operator ==(std::nullptr_t, const bc_unique_ptr<T, D>& p_first)
		{
			return p_first.get() == nullptr;
		};

		template <class T, class D>
		bool operator !=(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
		{
			return p_first.get() != nullptr;
		};

		template <class T, class D>
		bool operator !=(std::nullptr_t, const bc_unique_ptr<T, D>& p_first)
		{
			return p_first.get() != nullptr;
		};

		template <class T, class D>
		bool operator <(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
		{
			return p_first.get() < nullptr;
		};

		template <class T, class D>
		bool operator <(std::nullptr_t, const bc_unique_ptr<T, D>& p_first)
		{
			return p_first.get() < nullptr;
		};

		template <class T, class D>
		bool operator <=(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
		{
			return p_first.get() <= nullptr;
		};

		template <class T, class D>
		bool operator <=(nullptr_t, const bc_unique_ptr<T, D>& p_first)
		{
			return p_first.get() <= nullptr;
		};

		template <class T, class D>
		bool operator >(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
		{
			return p_first.get() > nullptr;
		};

		template <class T, class D>
		bool operator >(std::nullptr_t, const bc_unique_ptr<T, D>& p_first)
		{
			return p_first.get() > nullptr;
		};

		template <class T, class D>
		bool operator >=(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
		{
			return p_first.get() >= nullptr;
		};

		template <class T, class D>
		bool operator >=(std::nullptr_t, const bc_unique_ptr<T, D>& p_first)
		{
			return p_first.get() >= nullptr;
		};
		
#pragma endregion

#pragma region bcSharedPtr
		
		template <typename T>
		class bc_shared_ptr
		{
			template< typename T1 >
			friend class bc_shared_ptr;

		public:
			using this_type = bc_shared_ptr;
			using pointer = T*;
			using element_type = T;
			template< typename TOther >
			struct rebind
			{
				using other = bc_shared_ptr< TOther >;
			};

			class meta_data
			{
			public:
				template <typename T1>
				friend class bc_shared_ptr;

			public:
				template< typename TDeleter >
				meta_data(TDeleter p_deleter, bool p_is_shared)
					: m_deleter(p_deleter),
					m_ref_count(0),
					m_is_shared(p_is_shared)
				{
				}

				bcSIZE inc_ref_count() noexcept(true)
				{
					bcSIZE l_count = m_ref_count.fetch_add(1, core_platform::bc_memory_order::relaxed);
					return l_count + 1;
				}

				bcSIZE dec_ref_count() noexcept(true)
				{
					bcSIZE l_count = m_ref_count.fetch_sub(1, core_platform::bc_memory_order::relaxed);
					return l_count - 1;
				}

				bcSIZE ref_count() const noexcept(true)
				{
					return m_ref_count.load(core_platform::bc_memory_order::relaxed);
				}

				void call_deleter(pointer p_pointer) const
				{
					m_deleter(p_pointer);
				}

				bool is_shared() const noexcept(true)
				{
					return m_is_shared;
				}
				
			protected:

			private:
				bc_delegate<void(pointer)> m_deleter;
				core_platform::bc_atomic<bcSIZE> m_ref_count;
				bool m_is_shared;
			};

		public:
			constexpr bc_shared_ptr() noexcept(true)
				: m_pointer(nullptr),
				m_meta(nullptr)
			{
			}

			constexpr bc_shared_ptr(std::nullptr_t) noexcept(true)
				/*: bc_shared_ptr() TODO visual studio 2015 has a bug in constexpr delegating construction */
				: m_pointer(nullptr),
				m_meta(nullptr)
			{
			}

			template< typename TDeleter >
			bc_shared_ptr(std::nullptr_t, TDeleter p_deleter)
			{
				_construct(nullptr, p_deleter);
			}

			explicit bc_shared_ptr(pointer p_pointer)
				: m_pointer(nullptr)
			{
				_construct(p_pointer, bc_default_deleter<T>());
			}

			template< typename TDeleter >
			explicit bc_shared_ptr(pointer p_pointer, TDeleter p_deleter)
				: m_pointer(nullptr)
			{
				_construct(p_pointer, p_deleter);
			}

			template< typename T1 >
			explicit bc_shared_ptr(T1* p_pointer)
				: m_pointer(nullptr)
			{
				_construct(static_cast<T*>(p_pointer), bc_default_deleter<T>());
			}

			template< typename T1, typename TDeleter >
			explicit bc_shared_ptr(T1* p_pointer, TDeleter p_deleter)
				: m_pointer(nullptr)
			{
				_construct(static_cast<T*>(p_pointer), p_deleter);
			}

			bc_shared_ptr(const this_type& p_other) noexcept(true)
				: m_pointer(nullptr)
			{
				_assign(p_other);
			}

			template<typename T1>
			bc_shared_ptr(const bc_shared_ptr<T1>& p_other) noexcept(true)
				: m_pointer(nullptr)
			{
				_assign(p_other);
			}

			bc_shared_ptr(this_type&& p_other) noexcept(true)
				: m_pointer(nullptr)
			{
				_assign(std::move(p_other));
			}
			
			template<typename T1>
			bc_shared_ptr(bc_shared_ptr<T1>&& p_other) noexcept(true)
				: m_pointer(nullptr)
			{
				_assign(std::move(p_other));
			}

			template<typename T1>
			bc_shared_ptr(bc_unique_ptr<T1>&& p_other)
				: m_pointer(nullptr)
			{
				_construct(static_cast<T*>(p_other.release()), p_other.get_deleter());
			}

			~bc_shared_ptr()
			{
				_destruct();
			}

			this_type& operator =(const this_type& p_other) noexcept(true)
			{
				_assign(p_other);

				return *this;
			}

			template<typename T1>
			this_type& operator =(const bc_shared_ptr<T1>& p_other) noexcept(true)
			{
				_assign(p_other);

				return *this;
			}

			this_type& operator =(this_type&& p_other) noexcept(true)
			{
				_assign(std::move(p_other));

				return *this;
			}

			template<typename T1>
			this_type& operator =(bc_shared_ptr<T1>&& p_other) noexcept(true)
			{
				_assign(std::move(p_other));

				return *this;
			}

			template<typename T1>
			this_type& operator =(bc_unique_ptr<T1>&& p_other)
			{
				_destruct();

				_construct(static_cast<T*>(p_other.release()), p_other.get_deleter());

				return *this;
			}

			void reset() noexcept(true)
			{
				_destruct();
			}

			template< typename T1 >
			void reset(T1* p_pointer)
			{
				_destruct();
				_construct(p_pointer, bc_default_deleter<T>());
			}

			template< typename T1, typename TDeleter >
			void reset(T1* p_pointer, TDeleter p_deleter)
			{
				_destruct();
				_construct(p_pointer, p_deleter);
			}

			void swap(this_type& p_other) noexcept(true)
			{
				_unregister_pointer(reinterpret_cast< void** >(&m_pointer));
				_unregister_pointer(reinterpret_cast< void** >(&p_other.m_pointer));

				std::swap(m_pointer, p_other.m_pointer);
				std::swap(m_meta, p_other.m_meta);

				_register_pointer(reinterpret_cast< void** >(&m_pointer));
				_register_pointer(reinterpret_cast< void** >(&p_other.m_pointer));
			}

			pointer get() const noexcept(true)
			{
				return m_pointer;
			}

			T& operator *() const
			{
				return *m_pointer;
			}

			pointer operator ->() const noexcept(true)
			{
				return m_pointer;
			}

			bcUINT32 use_count() const noexcept(true)
			{
				if (m_meta)
					return m_meta->ref_count();

				return 0;
			}

			bool unique() const noexcept(true)
			{
				return use_count() == 1;
			}

			explicit operator bool() const noexcept(true)
			{
				return m_pointer != nullptr;
			}
			
			void _make(pointer p_pointer, meta_data* p_meta)
			{
				_construct(p_pointer, p_meta);
			}

		protected:

		private:
			template< typename TDeleter >
			meta_data* _allocate_meta(TDeleter p_deleter)
			{
				return bcNew(meta_data(p_deleter, false), bc_alloc_type::unknown);
			}

			void _deallocate_meta(meta_data* p_meta)
			{
				bcDelete(p_meta);
			}

			void _inc_reference_count()
			{
				m_meta->inc_ref_count();
			}
			
			void _dec_reference_count()
			{
				if (m_meta->dec_ref_count() == 0)
				{
					// If meta structure is shared with data call it's destructor else free it's memory
					if (m_meta->is_shared())
					{
						auto l_deleter = std::move(m_meta->m_deleter);
						m_meta->~meta_data();
						l_deleter(m_pointer);
					}
					else
					{
						m_meta->call_deleter(m_pointer);
						_deallocate_meta(m_meta);
					}
				}
			}

			template< typename TDeleter >
			void _construct(pointer p_pointer, TDeleter p_deleter)
			{
				try
				{
					m_pointer = p_pointer;
					m_meta = _allocate_meta(p_deleter);
				}
				catch (...)
				{
					p_deleter(p_pointer);
					throw;
				}

				if (m_pointer)
				{
					_register_pointer(reinterpret_cast< void** >(&m_pointer));

					_inc_reference_count();
				}
			}

			void _construct(pointer p_pointer, meta_data* p_meta)
			{
				m_pointer = p_pointer;
				m_meta = p_meta;

				if (m_pointer)
				{
					_register_pointer(reinterpret_cast< void** >(&m_pointer));

					_inc_reference_count();
				}
			}

			void _destruct()
			{
				if (m_pointer)
				{
					_unregister_pointer(reinterpret_cast< void** >(&m_pointer));

					_dec_reference_count();
				}
			}

			template<typename T1>
			void _assign(const bc_shared_ptr<T1>& p_other)
			{
				if (this != &p_other) // avoid self assignment
				{
					_destruct();
					_construct(static_cast<T*>(p_other.m_pointer), p_other.m_meta);
				}
			}

			template<typename T1>
			void _assign(bc_shared_ptr<T1>&& p_other)
			{
				if (this != &p_other) // avoid self assignment
				{
					_destruct();
					
					m_pointer = static_cast<T*>(p_other.m_pointer);
					m_meta = p_other.m_meta;

					if (p_other.m_pointer)
					{
						_register_pointer(reinterpret_cast< void** >(&m_pointer));
						_unregister_pointer(reinterpret_cast< void** >(&p_other.m_pointer));
						p_other.m_pointer = nullptr;
					}
				}
			}

			bcBYTE _compare(const void* p_pointer) const
			{
				return (m_pointer == p_pointer) ? 0 : ((m_pointer > p_pointer) ? 1 : ((m_pointer < p_pointer) ? -1 : 0));
			}

			pointer m_pointer;
			meta_data* m_meta;
		};

		template<class T1, class T2>
		bool operator ==(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
		{
			return p_first.get() == p_second.get();
		};

		template<class T1, class T2>
		bool operator !=(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
		{
			return p_first.get() != p_second.get();
		};

		template<class T1, class T2>
		bool operator <(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
		{
			return p_first.get() < p_second.get();
		};

		template<class T1, class T2>
		bool operator <=(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
		{
			return p_first.get() <= p_second.get();
		};

		template<class T1, class T2>
		bool operator >(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
		{
			return p_first.get() > p_second.get();
		};

		template<class T1, class T2>
		bool operator >=(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
		{
			return p_first.get() >= p_second.get();
		};

		template <class T>
		bool operator ==(const bc_shared_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() == nullptr;
		};

		template <class T>
		bool operator ==(std::nullptr_t, const bc_shared_ptr<T>& p_first)
		{
			return p_first.get() == nullptr;
		};

		template <class T>
		bool operator !=(const bc_shared_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() != nullptr;
		};

		template <class T>
		bool operator !=(std::nullptr_t, const bc_shared_ptr<T>& p_first)
		{
			return p_first.get() != nullptr;
		};

		template <class T>
		bool operator <(const bc_shared_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() < nullptr;
		};

		template <class T>
		bool operator <(std::nullptr_t, const bc_shared_ptr<T>& p_first)
		{
			return p_first.get() < nullptr;
		};

		template <class T>
		bool operator <=(const bc_shared_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() <= nullptr;
		};

		template <class T>
		bool operator <=(nullptr_t, const bc_shared_ptr<T>& p_first)
		{
			return p_first.get() <= nullptr;
		};

		template <class T>
		bool operator >(const bc_shared_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() > nullptr;
		};

		template <class T>
		bool operator >(std::nullptr_t, const bc_shared_ptr<T>& p_first)
		{
			return p_first.get() > nullptr;
		};

		template <class T>
		bool operator >=(const bc_shared_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() >= nullptr;
		};

		template <class T>
		bool operator >=(std::nullptr_t, const bc_shared_ptr<T>& p_first)
		{
			return p_first.get() >= nullptr;
		};

#pragma endregion

#pragma region Ptr

		// A movale aware smart pointer that doesn't provide auto clean-up after destruction
		template < typename T >
		class bc_handle_ptr
		{
			template< typename T1 >
			friend class bc_handle_ptr;

		public:
			template< typename TOther >
			struct rebind
			{
				using other = bc_handle_ptr< TOther >;
			};

		public:
			constexpr bc_handle_ptr() noexcept(true)
				: m_pointer(nullptr)
			{
			}

			constexpr bc_handle_ptr(std::nullptr_t) noexcept(true)
				/*: bc_handle_ptr() TODO visual studio 2015 has a bug in constexpr delegating construction */
				: m_pointer(nullptr)
			{
			}

			bc_handle_ptr(T* p_pointer) noexcept(true)
				: m_pointer(nullptr)
			{
				_construct(p_pointer);
			}

			template< typename T1 >
			bc_handle_ptr(T1* p_pointer) noexcept(true)
				: bc_handle_ptr(static_cast<T*>(p_pointer))
			{
			}

			bc_handle_ptr(const bc_handle_ptr<T>& p_other) noexcept(true)
				: m_pointer(nullptr)
			{
				_construct(p_other.get());
			}

			bc_handle_ptr(bc_handle_ptr<T>&& p_other) noexcept(true)
				: m_pointer(nullptr)
			{
				_assign(std::move(p_other));
			}

			template<typename T1>
			bc_handle_ptr(const bc_handle_ptr<T1>& p_other) noexcept(true)
				: m_pointer(nullptr)
			{
				_construct(static_cast<T*>(p_other.get()));
			}

			template<typename T1>
			bc_handle_ptr(bc_handle_ptr<T1>&& p_other) noexcept(true)
				: m_pointer(nullptr)
			{
				_assign(std::move(p_other));
			}

			~bc_handle_ptr()
			{
				_destruct();
			}

			bc_handle_ptr<T>& operator =(std::nullptr_t) noexcept(true)
			{
				_destruct();

				return *this;
			}

			bc_handle_ptr<T>& operator =(const bc_handle_ptr<T>& p_other) noexcept(true)
			{
				_assign(p_other);

				return *this;
			}

			bc_handle_ptr<T>& operator =(bc_handle_ptr<T>&& p_other) noexcept(true)
			{
				_assign(std::move(p_other));

				return *this;
			}

			template<typename T1>
			bc_handle_ptr<T>& operator =(const bc_handle_ptr<T1>& p_other) noexcept(true)
			{
				_assign(p_other);

				return *this;
			}

			template<typename T1>
			bc_handle_ptr<T>& operator =(bc_handle_ptr<T1>&& p_other) noexcept(true)
			{
				_assign(std::move(p_other));

				return *this;
			}

			T* release() noexcept(true)
			{
				T* l_pointer = get();

				_destruct();

				return l_pointer;
			}

			template< typename T1 >
			void reset(T1* p_pointer) noexcept(true)
			{
				_destruct();
				_construct(static_cast<T*>(p_pointer));
			}

			void swap(bc_handle_ptr<T>& p_other) noexcept(true)
			{
				_unregister_pointer(reinterpret_cast< void** >(&m_pointer));
				_unregister_pointer(reinterpret_cast< void** >(&p_other.m_pointer));

				std::swap(m_pointer, p_other.m_pointer);

				_register_pointer(reinterpret_cast< void** >(&m_pointer));
				_register_pointer(reinterpret_cast< void** >(&p_other.m_pointer));
			}

			T* get() const noexcept(true)
			{
				return m_pointer;
			}

			T& operator *() const
			{
				return *m_pointer;
			}

			T* operator ->() const noexcept(true)
			{
				return m_pointer;
			}
				
			operator T*() const noexcept(true)
			{
				return get();
			}

			explicit operator bool() const noexcept(true)
			{
				return m_pointer != nullptr;
			}

		protected:

		private:
			void _construct(T* p_pointer)
			{
				m_pointer = p_pointer;

				if (m_pointer)
				{
					_register_pointer(reinterpret_cast< void** >(&m_pointer));
				}
			}

			void _destruct()
			{
				if (m_pointer)
				{
					_unregister_pointer(reinterpret_cast< void** >(&m_pointer));
					m_pointer = nullptr;
				}
			}

			template<typename T1>
			void _assign(const bc_handle_ptr<T1>& p_other)
			{
				if (static_cast< const void* >(this) != static_cast< const void* >(&p_other)) // avoid self assignment
				{
					reset(p_other.get());
				}
			}

			template<typename T1>
			void _assign(bc_handle_ptr<T1>&& p_other)
			{
				if (static_cast< const void* >(this) != static_cast< const void* >(&p_other)) // avoid self assignment
				{
					reset(p_other.release());
				}
			}

			bcBYTE _compare(const void* pPointer) const
			{
				return (m_pointer == pPointer) ? 0 : ((m_pointer > pPointer) ? 1 : ((m_pointer < pPointer) ? -1 : 0));
			}

			T* m_pointer;
		};

		template<class T1, class T2>
		bool operator ==(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
		{
			return p_first.get() == p_second.get();
		};

		template<class T1, class T2>
		bool operator !=(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
		{
			return p_first.get() != p_second.get();
		};

		template<class T1, class T2>
		bool operator <(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
		{
			return p_first.get() < p_second.get();
		};

		template<class T1, class T2>
		bool operator <=(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
		{
			return p_first.get() <= p_second.get();
		};

		template<class T1, class T2>
		bool operator >(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
		{
			return p_first.get() > p_second.get();
		};

		template<class T1, class T2>
		bool operator >=(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
		{
			return p_first.get() >= p_second.get();
		};

		template <class T>
		bool operator ==(const bc_handle_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() == nullptr;
		};

		template <class T>
		bool operator ==(std::nullptr_t, const bc_handle_ptr<T>& p_first)
		{
			return p_first.get() == nullptr;
		};

		template <class T>
		bool operator !=(const bc_handle_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() != nullptr;
		};

		template <class T>
		bool operator !=(std::nullptr_t, const bc_handle_ptr<T>& p_first)
		{
			return p_first.get() != nullptr;
		};

		template <class T>
		bool operator <(const bc_handle_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() < nullptr;
		};

		template <class T>
		bool operator <(std::nullptr_t, const bc_handle_ptr<T>& p_first)
		{
			return p_first.get() < nullptr;
		};

		template <class T>
		bool operator <=(const bc_handle_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() <= nullptr;
		};

		template <class T>
		bool operator <=(nullptr_t, const bc_handle_ptr<T>& p_first)
		{
			return p_first.get() <= nullptr;
		};

		template <class T>
		bool operator >(const bc_handle_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() > nullptr;
		};

		template <class T>
		bool operator >(std::nullptr_t, const bc_handle_ptr<T>& p_first)
		{
			return p_first.get() > nullptr;
		};

		template <class T>
		bool operator >=(const bc_handle_ptr<T>& p_first, std::nullptr_t)
		{
			return p_first.get() >= nullptr;
		};

		template <class T>
		bool operator >=(std::nullptr_t, const bc_handle_ptr<T>& p_first)
		{
			return p_first.get() >= nullptr;
		};

#pragma endregion

#ifdef BC_MEMORY_DEFRAG

		template< typename T >
		using bc_ptr = bc_handle_ptr< T >;

#else

		template< typename T >
		using bc_ptr = T*;

#endif

		template< typename T, typename ...TArgs >
		bc_unique_ptr< T > bc_make_unique(TArgs&&... p_args)
		{
			return bc_make_unique< T >(bc_alloc_type::unknown, std::forward< TArgs >(p_args)...);
		}

		template< typename T, typename ...TArgs >
		bc_unique_ptr< T > bc_make_unique(bc_alloc_type p_alloc_type, TArgs&&... p_args)
		{
			return bc_unique_ptr< T >(bcNew(T(std::forward< TArgs >(p_args)...), p_alloc_type));
		}

		template< typename T, typename ...TArgs >
		bc_shared_ptr< T > bc_make_shared(TArgs&&... p_args)
		{
			return bc_make_shared< T >(bc_alloc_type::unknown, std::forward< TArgs >(p_args)...);
		}

		template< typename T, typename ...TArgs >
		bc_shared_ptr< T > bc_make_shared(bc_alloc_type p_alloc_type, TArgs&&... p_args)
		{
			using meta_type = typename bc_shared_ptr< T >::meta_data;

			bcSIZE l_required_size = sizeof(T) + sizeof(meta_type);
			void* l_alloc = bcAlloc(l_required_size, p_alloc_type);
			if (!l_alloc)
				throw std::bad_alloc();

			T* l_pointer = l_alloc;
			meta_type* l_meta_pointer = l_alloc + sizeof(T);

			new(l_pointer)T(std::forward< TArgs >(p_args)...);
			new(l_meta_pointer)meta_type(bc_default_deleter< T >(), true);

			bc_shared_ptr< T > l_result;
			l_result._make(l_pointer, l_meta_pointer);

			return l_result;
		}

		template< typename T, typename TDeleter >
		void swap(bc_unique_ptr< T, TDeleter >& p_first, bc_unique_ptr< T, TDeleter >& p_second) noexcept
		{
			p_first.swap(p_second);
		}

		template< typename T >
		void swap(bc_shared_ptr< T >& p_first, bc_shared_ptr< T >& p_second) noexcept
		{
			p_first.swap(p_second);
		}

		template< typename T >
		void swap(bc_handle_ptr< T >& p_first, bc_handle_ptr< T >& p_second) noexcept
		{
			p_first.swap(p_second);
		}

		template< class T, class U >
		bc_shared_ptr<T> static_pointer_cast(const bc_shared_ptr<U>& r)
		{
			return bc_shared_ptr<T>(static_cast<T*>(r.get()));
		};

		template< class T, class U >
		bc_shared_ptr<T> dynamic_pointer_cast(const bc_shared_ptr<U>& r)
		{
			return bc_shared_ptr<T>(dynamic_cast<T*>(r.get()));
		};

		template< class T, class U >
		bc_shared_ptr<T> const_pointer_cast(const bc_shared_ptr<U>& r)
		{
			return bc_shared_ptr<T>(const_cast<T*>(r.get()));
		};

	}
}

namespace std
{
	template<class T>
	struct std::hash< black_cat::core::bc_unique_ptr<T> >
	{
		std::size_t operator ()(const black_cat::core::bc_unique_ptr<T>& p_pointer)
		{
			return std::hash<T*>()(p_pointer.get());
		}
	};

	template<class T>
	struct std::hash< black_cat::core::bc_shared_ptr<T> >
	{
		std::size_t operator ()(const black_cat::core::bc_shared_ptr<T>& p_pointer)
		{
			return std::hash<T*>()(p_pointer.get());
		}
	};

	template<class T>
	struct std::hash< black_cat::core::bc_handle_ptr<T> >
	{
		std::size_t operator ()(const black_cat::core::bc_handle_ptr<T>& p_pointer)
		{
			return std::hash<T*>()(p_pointer.get());
		}
	};
}