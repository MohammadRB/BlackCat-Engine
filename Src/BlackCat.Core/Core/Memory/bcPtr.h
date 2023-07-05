//  [09/08/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Memory/bcAlloc.h"

namespace black_cat::core
{
	class bc_default_deleter
	{
	public:
		template<typename T>
		void operator()(T* p_pointer) const noexcept
		{
			static_assert(sizeof(T)> 0, "Can not delete an incomplete type");
			BC_DELETE(p_pointer);
		}
	};

	inline void _register_pointer(void** p_pointer)
	{
		if (is_movable_pointer(*p_pointer))
		{
			register_movable_pointer(p_pointer);
		}	
	}

	inline void _unregister_pointer(void** p_pointer)
	{
		if (is_movable_pointer(*p_pointer))
		{
			unregister_movable_pointer(p_pointer);
		}	
	}

#pragma region bc_unique_ptr

	template <typename T, typename TDeleter = bc_default_deleter>
	class bc_unique_ptr : TDeleter
	{
		template<typename T1, typename TDeleter1>
		friend class bc_unique_ptr;

	public:
		using this_type = bc_unique_ptr;
		using pointer = T*;
		using element_type = T;
		using deleter_type = TDeleter;
		template<typename TOther>
		struct rebind
		{
			using other = bc_unique_ptr<TOther, deleter_type>;
		};

	public:
		constexpr bc_unique_ptr() noexcept
			: TDeleter(),
			  m_pointer(nullptr)
		{
		}
			
		constexpr bc_unique_ptr(std::nullptr_t) noexcept
			: TDeleter(),
			  m_pointer(nullptr)
		{
		}
			
		explicit bc_unique_ptr(pointer p_pointer) noexcept
			: TDeleter()
		{
			_construct(p_pointer);
		}

		bc_unique_ptr(pointer p_pointer, deleter_type p_deleter) noexcept
			: TDeleter(p_deleter)
		{
			_construct(p_pointer);
		}

		bc_unique_ptr(const this_type& p_other) = delete;

		bc_unique_ptr(this_type&& p_other) noexcept
			: TDeleter(),
			  m_pointer(nullptr)
		{
			_assign(std::move(p_other));
		}

		template<typename T1, typename TDeleter1>
		bc_unique_ptr(bc_unique_ptr<T1, TDeleter1>&& p_other) noexcept
			: TDeleter(),
			  m_pointer(nullptr)
		{
			_assign(std::move(p_other));
		}

		~bc_unique_ptr()
		{
			_destruct();
		}

		this_type& operator =(const this_type& p_other) = delete;

		this_type& operator =(this_type&& p_other) noexcept
		{
			_assign(std::move(p_other));

			return *this;
		}

		template<typename T1, typename TDeleter1>
		this_type& operator =(bc_unique_ptr<T1, TDeleter1>&& p_other) noexcept
		{
			_assign(std::move(p_other));

			return *this;
		}

		this_type& operator =(std::nullptr_t) noexcept
		{
			_destruct();
			m_pointer = nullptr;

			return *this;
		}

		T* release() noexcept
		{
			T* lPointer = get();
			_unregister_pointer(reinterpret_cast<void**>(&m_pointer));
			m_pointer = nullptr;

			return lPointer;
		}

		void reset() noexcept
		{
			_destruct();
		}

		void reset(T* p_pointer) noexcept
		{
			_destruct();
			_construct(p_pointer);
		}

		void swap(this_type& p_other) noexcept
		{
			using std::swap;

			_unregister_pointer(reinterpret_cast<void**>(&m_pointer));
			_unregister_pointer(reinterpret_cast<void**>(&p_other.m_pointer));

			std::swap(m_pointer, p_other.m_pointer);
			swap(static_cast<TDeleter&>(*this), static_cast<TDeleter&>(p_other));

			_register_pointer(reinterpret_cast<void**>(&m_pointer));
			_register_pointer(reinterpret_cast<void**>(&p_other.m_pointer));
		}

		T* get() const noexcept
		{
			return m_pointer;
		}

		deleter_type get_deleter() const noexcept
		{
			return static_cast<TDeleter>(*this);
		}

		explicit operator bool() const noexcept
		{
			return m_pointer != nullptr;
		}

		T& operator *() const
		{
			return *m_pointer;
		}

		T* operator ->() const noexcept
		{
			return m_pointer;
		}

	private:
		void _construct(T* p_pointer)
		{
			m_pointer = p_pointer;
			_register_pointer(reinterpret_cast<void**>(&m_pointer));
		}

		void _destruct()
		{
			_unregister_pointer(reinterpret_cast<void**>(&m_pointer));

			static_cast<TDeleter&>(*this)(m_pointer);
			m_pointer = nullptr;
		}

		template<typename T1, typename TDeleter1>
		void _assign(bc_unique_ptr<T1, TDeleter1>&& p_other)
		{
			reset(static_cast<T*>(p_other.release()));
			static_cast<TDeleter&>(*this) = static_cast<TDeleter>(p_other.get_deleter());

			p_other.m_pointer = nullptr;
		}

		pointer m_pointer;
	};

	template<class T1, class D1, class T2, class D2>
	bool operator ==(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
	{
		return p_first.get() == p_second.get();
	}

	template<class T1, class D1, class T2, class D2>
	bool operator !=(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
	{
		return p_first.get() != p_second.get();
	}

	template<class T1, class D1, class T2, class D2>
	bool operator <(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
	{
		return p_first.get() <p_second.get();
	}

	template<class T1, class D1, class T2, class D2>
	bool operator <=(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
	{
		return p_first.get() <= p_second.get();
	}
		
	template<class T1, class D1, class T2, class D2>
	bool operator>(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
	{
		return p_first.get()> p_second.get();
	}
		
	template<class T1, class D1, class T2, class D2>
	bool operator>=(const bc_unique_ptr<T1, D1>& p_first, const bc_unique_ptr<T2, D2>& p_second)
	{
		return p_first.get()>= p_second.get();
	}

	template <class T, class D>
	bool operator ==(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
	{
		return p_first.get() == nullptr;
	}

	template <class T, class D>
	bool operator ==(std::nullptr_t, const bc_unique_ptr<T, D>& p_first)
	{
		return p_first.get() == nullptr;
	}

	template <class T, class D>
	bool operator !=(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
	{
		return p_first.get() != nullptr;
	}

	template <class T, class D>
	bool operator !=(std::nullptr_t, const bc_unique_ptr<T, D>& p_first)
	{
		return p_first.get() != nullptr;
	}

	template <class T, class D>
	bool operator <(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
	{
		return p_first.get() <nullptr;
	}

	template <class T, class D>
	bool operator <(std::nullptr_t, const bc_unique_ptr<T, D>& p_first)
	{
		return p_first.get() <nullptr;
	}

	template <class T, class D>
	bool operator <=(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
	{
		return p_first.get() <= nullptr;
	}

	template <class T, class D>
	bool operator <=(nullptr_t, const bc_unique_ptr<T, D>& p_first)
	{
		return p_first.get() <= nullptr;
	}

	template <class T, class D>
	bool operator>(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
	{
		return p_first.get()> nullptr;
	}

	template <class T, class D>
	bool operator>(std::nullptr_t, const bc_unique_ptr<T, D>& p_first)
	{
		return p_first.get()> nullptr;
	}

	template <class T, class D>
	bool operator>=(const bc_unique_ptr<T, D>& p_first, std::nullptr_t)
	{
		return p_first.get()>= nullptr;
	}

	template <class T, class D>
	bool operator>=(std::nullptr_t, const bc_unique_ptr<T, D>& p_first)
	{
		return p_first.get()>= nullptr;
	}

#pragma endregion

#pragma region bc_shared_ptr
		
	template <typename T>
	class bc_shared_ptr
	{
		template<typename T1>
		friend class bc_shared_ptr;

	public:
		using this_type = bc_shared_ptr;
		using pointer = T*;
		using element_type = T;
		class meta_data;
		template<typename TDeleter> 
		class meta_data_imp;
		template<typename TOther>
		struct rebind
		{
			using other = bc_shared_ptr<TOther>;
		};

	public:
		constexpr bc_shared_ptr() noexcept
			: m_pointer(nullptr),
			  m_meta(nullptr)
		{
		}

		constexpr bc_shared_ptr(std::nullptr_t) noexcept
			: m_pointer(nullptr),
			  m_meta(nullptr)
		{
		}

		template<typename TDeleter>
		bc_shared_ptr(std::nullptr_t, TDeleter p_deleter)
		{
			_construct(nullptr, p_deleter);
		}

		explicit bc_shared_ptr(pointer p_pointer)
		{
			_construct(p_pointer, bc_default_deleter());
		}

		template<typename TDeleter>
		explicit bc_shared_ptr(pointer p_pointer, TDeleter p_deleter)
		{
			_construct(p_pointer, p_deleter);
		}

		template<typename T1>
		explicit bc_shared_ptr(T1* p_pointer)
		{
			_construct(static_cast<T*>(p_pointer), bc_default_deleter());
		}

		template<typename T1, typename TDeleter>
		explicit bc_shared_ptr(T1* p_pointer, TDeleter p_deleter)
		{
			_construct(static_cast<T*>(p_pointer), p_deleter);
		}

		bc_shared_ptr(const this_type& p_other) noexcept
			: m_pointer(nullptr),
			  m_meta(nullptr)
		{
			_assign(p_other);
		}

		template<typename T1>
		bc_shared_ptr(const bc_shared_ptr<T1>& p_other) noexcept
			: m_pointer(nullptr),
			  m_meta(nullptr)
		{
			_assign(p_other);
		}

		bc_shared_ptr(this_type&& p_other) noexcept
			: m_pointer(nullptr),
			  m_meta(nullptr)
		{
			_assign(std::move(p_other));
		}
			
		template<typename T1>
		bc_shared_ptr(bc_shared_ptr<T1>&& p_other) noexcept
			: m_pointer(nullptr),
			  m_meta(nullptr)
		{
			_assign(std::move(p_other));
		}

		template<typename T1>
		bc_shared_ptr(bc_unique_ptr<T1>&& p_other)
			: m_pointer(nullptr),
			  m_meta(nullptr)
		{
			_construct(static_cast<T*>(p_other.release()), p_other.get_deleter());
		}

		~bc_shared_ptr()
		{
			_destruct();
		}

		this_type& operator =(const this_type& p_other) noexcept
		{
			_assign(p_other);

			return *this;
		}

		template<typename T1>
		this_type& operator =(const bc_shared_ptr<T1>& p_other) noexcept
		{
			_assign(p_other);

			return *this;
		}

		this_type& operator =(this_type&& p_other) noexcept
		{
			_assign(std::move(p_other));

			return *this;
		}

		template<typename T1>
		this_type& operator =(bc_shared_ptr<T1>&& p_other) noexcept
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

		void reset() noexcept
		{
			_destruct();
		}

		template<typename T1>
		void reset(T1* p_pointer)
		{
			_destruct();
			_construct(p_pointer, bc_default_deleter());
		}

		template<typename T1, typename TDeleter>
		void reset(T1* p_pointer, TDeleter p_deleter)
		{
			_destruct();
			_construct(p_pointer, p_deleter);
		}

		void swap(this_type& p_other) noexcept
		{
			_unregister_pointer(reinterpret_cast<void**>(&m_pointer));
			_unregister_pointer(reinterpret_cast<void**>(&p_other.m_pointer));

			std::swap(m_pointer, p_other.m_pointer);
			std::swap(m_meta, p_other.m_meta);

			_register_pointer(reinterpret_cast<void**>(&m_pointer));
			_register_pointer(reinterpret_cast<void**>(&p_other.m_pointer));
		}

		pointer get() const noexcept
		{
			return m_pointer;
		}

		T& operator *() const
		{
			return *m_pointer;
		}

		pointer operator ->() const noexcept
		{
			return m_pointer;
		}

		bcUINT32 use_count() const noexcept
		{
			if (m_meta)
				return m_meta->ref_count();

			return 0;
		}

		bool unique() const noexcept
		{
			return use_count() == 1;
		}

		explicit operator bool() const noexcept
		{
			return m_pointer != nullptr;
		}
			
		void _make(pointer p_pointer, meta_data* p_meta)
		{
			_construct(p_pointer, p_meta);
		}

	private:
		void _inc_reference_count()
		{
			m_meta->inc_ref_count();
		}
			
		void _dec_reference_count()
		{
			if (m_meta && m_meta->dec_ref_count() == 0)
			{
				// If meta structure is shared with main data call it's destructor otherwise free it's memory
				if (m_meta->is_shared())
				{
					m_meta->call_deleter_and_destruct(m_pointer);
				}
				else
				{
					m_meta->call_deleter(m_pointer);
					BC_DELETE(m_meta);
				}
			}
		}

		template<typename TDeleter>
		void _construct(pointer p_pointer, TDeleter p_deleter)
		{
			try
			{
				m_pointer = p_pointer;
				m_meta = BC_NEW(meta_data_imp<TDeleter>(p_deleter, false), bc_alloc_type::unknown);
			}
			catch (...)
			{
				p_deleter(p_pointer);
				throw;
			}

			if (m_pointer)
			{
				_register_pointer(reinterpret_cast<void**>(&m_pointer));
				_inc_reference_count();
			}
		}

		void _construct(pointer p_pointer, meta_data* p_meta)
		{
			m_pointer = p_pointer;
			m_meta = p_meta;

			if (m_pointer)
			{
				_register_pointer(reinterpret_cast<void**>(&m_pointer));
				_inc_reference_count();
			}
		}

		void _destruct()
		{
			_unregister_pointer(reinterpret_cast<void**>(&m_pointer));
			_dec_reference_count();
		}

		template<typename T1>
		void _assign(const bc_shared_ptr<T1>& p_other)
		{
			if (m_pointer != static_cast<T*>(p_other.m_pointer))
			{
				_destruct();
				_construct
				(
					static_cast<T*>(const_cast<T1*>(p_other.m_pointer)),
					reinterpret_cast<meta_data*>(const_cast<typename bc_shared_ptr<T1>::meta_data*>(p_other.m_meta))
				);
			}
		}

		template<typename T1>
		void _assign(bc_shared_ptr<T1>&& p_other)
		{
			if (m_pointer != static_cast<T*>(p_other.m_pointer))
			{
				_destruct();
					
				m_pointer = static_cast<T*>(p_other.m_pointer);
				m_meta = reinterpret_cast<meta_data*>(p_other.m_meta);

				_register_pointer(reinterpret_cast<void**>(&m_pointer));
				_unregister_pointer(reinterpret_cast<void**>(&p_other.m_pointer));
				p_other.m_pointer = nullptr;
				p_other.m_meta = nullptr;
			}
		}

		pointer m_pointer;
		meta_data* m_meta;
	};

	template <typename T>
	class bc_shared_ptr<T>::meta_data
	{
	public:
		meta_data(bool p_is_shared)
			: m_ref_count(0),
			  m_is_shared(p_is_shared)
		{
		}

		virtual ~meta_data() = default;
			
		bcSIZE inc_ref_count() noexcept
		{
			const bcSIZE l_count = m_ref_count.fetch_add(1, platform::bc_memory_order::relaxed);
			return l_count + 1;
		}

		bcSIZE dec_ref_count() noexcept
		{
			const bcSIZE l_count = m_ref_count.fetch_sub(1, platform::bc_memory_order::relaxed);
			return l_count - 1;
		}

		bcSIZE ref_count() const noexcept
		{
			return m_ref_count.load(platform::bc_memory_order::relaxed);
		}

		bool is_shared() const noexcept
		{
			return m_is_shared;
		}

		virtual void call_deleter(pointer p_pointer) const = 0;

		virtual void call_deleter_and_destruct(pointer p_pointer) const = 0;

	private:
		platform::bc_atomic<bcSIZE> m_ref_count;
		bool m_is_shared;
	};

	template<typename T>
	template<typename TDeleter>
	class bc_shared_ptr<T>::meta_data_imp : public meta_data
	{
	public:
		meta_data_imp(TDeleter p_deleter, bool p_is_shared)
			: meta_data(p_is_shared),
			  m_deleter(p_deleter)
		{
		}

		void call_deleter(pointer p_pointer) const override
		{
			m_deleter(p_pointer);
		}

		void call_deleter_and_destruct(pointer p_pointer) const override
		{
			// If meta is shared, we need to first destruct it then deallocate memory
			TDeleter l_deleter = m_deleter;
			this->~meta_data_imp();

			l_deleter(p_pointer);
		}

	private:
		TDeleter m_deleter;
	};

	template<class T1, class T2>
	bool operator ==(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
	{
		return p_first.get() == p_second.get();
	}

	template<class T1, class T2>
	bool operator !=(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
	{
		return p_first.get() != p_second.get();
	}

	template<class T1, class T2>
	bool operator <(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
	{
		return p_first.get() <p_second.get();
	}

	template<class T1, class T2>
	bool operator <=(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
	{
		return p_first.get() <= p_second.get();
	}

	template<class T1, class T2>
	bool operator>(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
	{
		return p_first.get()> p_second.get();
	}

	template<class T1, class T2>
	bool operator>=(const bc_shared_ptr<T1>& p_first, const bc_shared_ptr<T2>& p_second)
	{
		return p_first.get()>= p_second.get();
	}

	template <class T>
	bool operator ==(const bc_shared_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get() == nullptr;
	}

	template <class T>
	bool operator ==(std::nullptr_t, const bc_shared_ptr<T>& p_first)
	{
		return p_first.get() == nullptr;
	}

	template <class T>
	bool operator !=(const bc_shared_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get() != nullptr;
	}

	template <class T>
	bool operator !=(std::nullptr_t, const bc_shared_ptr<T>& p_first)
	{
		return p_first.get() != nullptr;
	}

	template <class T>
	bool operator <(const bc_shared_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get() <nullptr;
	}

	template <class T>
	bool operator <(std::nullptr_t, const bc_shared_ptr<T>& p_first)
	{
		return p_first.get() <nullptr;
	}

	template <class T>
	bool operator <=(const bc_shared_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get() <= nullptr;
	}

	template <class T>
	bool operator <=(nullptr_t, const bc_shared_ptr<T>& p_first)
	{
		return p_first.get() <= nullptr;
	}

	template <class T>
	bool operator>(const bc_shared_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get()> nullptr;
	}

	template <class T>
	bool operator>(std::nullptr_t, const bc_shared_ptr<T>& p_first)
	{
		return p_first.get()> nullptr;
	}

	template <class T>
	bool operator>=(const bc_shared_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get()>= nullptr;
	}

	template <class T>
	bool operator>=(std::nullptr_t, const bc_shared_ptr<T>& p_first)
	{
		return p_first.get()>= nullptr;
	}

#pragma endregion

#pragma region bc_handle_ptr

	/**
		 * \brief Movable aware smart pointer.
		 * <b> Does not </b> provide auto clean-up after destruction
		 * \tparam T 
		 */
	template <typename T>
	class bc_handle_ptr
	{
		template<typename T1>
		friend class bc_handle_ptr;

	public:
		template<typename TOther>
		struct rebind
		{
			using other = bc_handle_ptr<TOther>;
		};

	public:
		constexpr bc_handle_ptr() noexcept
			: m_pointer(nullptr)
		{
		}

		constexpr bc_handle_ptr(std::nullptr_t) noexcept
			: bc_handle_ptr()
		{
		}

		bc_handle_ptr(T* p_pointer) noexcept
			: bc_handle_ptr()
		{
			_construct(p_pointer);
		}

		template<typename T1>
		bc_handle_ptr(T1* p_pointer) noexcept
			: bc_handle_ptr(static_cast<T*>(p_pointer))
		{
		}

		bc_handle_ptr(const bc_handle_ptr<T>& p_other) noexcept
			: bc_handle_ptr()
		{
			_construct(p_other.get());
		}

		bc_handle_ptr(bc_handle_ptr<T>&& p_other) noexcept
			: bc_handle_ptr()
		{
			_assign(std::move(p_other));
		}

		template<typename T1>
		bc_handle_ptr(const bc_handle_ptr<T1>& p_other) noexcept
			: bc_handle_ptr()
		{
			_construct(static_cast<T*>(p_other.get()));
		}

		template<typename T1>
		bc_handle_ptr(bc_handle_ptr<T1>&& p_other) noexcept
			: bc_handle_ptr()
		{
			_assign(std::move(p_other));
		}

		~bc_handle_ptr()
		{
			_destruct();
		}

		bc_handle_ptr<T>& operator =(std::nullptr_t) noexcept
		{
			_destruct();

			return *this;
		}

		bc_handle_ptr<T>& operator =(const bc_handle_ptr<T>& p_other) noexcept
		{
			_assign(p_other);

			return *this;
		}

		bc_handle_ptr<T>& operator =(bc_handle_ptr<T>&& p_other) noexcept
		{
			_assign(std::move(p_other));

			return *this;
		}

		template<typename T1>
		bc_handle_ptr<T>& operator =(const bc_handle_ptr<T1>& p_other) noexcept
		{
			_assign(p_other);

			return *this;
		}

		template<typename T1>
		bc_handle_ptr<T>& operator =(bc_handle_ptr<T1>&& p_other) noexcept
		{
			_assign(std::move(p_other));

			return *this;
		}

		T* release() noexcept
		{
			T* l_pointer = get();

			_destruct();

			return l_pointer;
		}

		template<typename T1>
		void reset(T1* p_pointer) noexcept
		{
			_destruct();
			_construct(static_cast<T*>(p_pointer));
		}

		void swap(bc_handle_ptr<T>& p_other) noexcept
		{
			_unregister_pointer(reinterpret_cast<void**>(&m_pointer));
			_unregister_pointer(reinterpret_cast<void**>(&p_other.m_pointer));

			std::swap(m_pointer, p_other.m_pointer);

			_register_pointer(reinterpret_cast<void**>(&m_pointer));
			_register_pointer(reinterpret_cast<void**>(&p_other.m_pointer));
		}

		T* get() const noexcept
		{
			return m_pointer;
		}

		T& operator *() const
		{
			return *m_pointer;
		}

		T* operator ->() const noexcept
		{
			return m_pointer;
		}
				
		operator T*() const noexcept
		{
			return get();
		}

		explicit operator bool() const noexcept
		{
			return m_pointer != nullptr;
		}

	private:
		void _construct(T* p_pointer)
		{
			m_pointer = p_pointer;
			_register_pointer(reinterpret_cast<void**>(&m_pointer));
		}

		void _destruct()
		{
			_unregister_pointer(reinterpret_cast<void**>(&m_pointer));
			m_pointer = nullptr;
		}

		template<typename T1>
		void _assign(const bc_handle_ptr<T1>& p_other)
		{
			reset(p_other.get());
		}

		template<typename T1>
		void _assign(bc_handle_ptr<T1>&& p_other)
		{
			reset(p_other.release());
		}

		T* m_pointer;
	};

	template<class T1, class T2>
	bool operator ==(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
	{
		return p_first.get() == p_second.get();
	}

	template<class T1, class T2>
	bool operator !=(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
	{
		return p_first.get() != p_second.get();
	}

	template<class T1, class T2>
	bool operator <(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
	{
		return p_first.get() < p_second.get();
	}

	template<class T1, class T2>
	bool operator <=(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
	{
		return p_first.get() <= p_second.get();
	}

	template<class T1, class T2>
	bool operator>(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
	{
		return p_first.get() > p_second.get();
	}

	template<class T1, class T2>
	bool operator>=(const bc_handle_ptr<T1>& p_first, const bc_handle_ptr<T2>& p_second)
	{
		return p_first.get()>= p_second.get();
	}

	template <class T>
	bool operator ==(const bc_handle_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get() == nullptr;
	}

	template <class T>
	bool operator ==(std::nullptr_t, const bc_handle_ptr<T>& p_first)
	{
		return p_first.get() == nullptr;
	}

	template <class T>
	bool operator !=(const bc_handle_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get() != nullptr;
	}

	template <class T>
	bool operator !=(std::nullptr_t, const bc_handle_ptr<T>& p_first)
	{
		return p_first.get() != nullptr;
	}

	template <class T>
	bool operator <(const bc_handle_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get() < nullptr;
	}

	template <class T>
	bool operator <(std::nullptr_t, const bc_handle_ptr<T>& p_first)
	{
		return p_first.get() < nullptr;
	}

	template <class T>
	bool operator <=(const bc_handle_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get() <= nullptr;
	}

	template <class T>
	bool operator <=(nullptr_t, const bc_handle_ptr<T>& p_first)
	{
		return p_first.get() <= nullptr;
	}

	template <class T>
	bool operator>(const bc_handle_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get() > nullptr;
	}

	template <class T>
	bool operator>(std::nullptr_t, const bc_handle_ptr<T>& p_first)
	{
		return p_first.get() > nullptr;
	}

	template <class T>
	bool operator>=(const bc_handle_ptr<T>& p_first, std::nullptr_t)
	{
		return p_first.get() >= nullptr;
	}

	template <class T>
	bool operator>=(std::nullptr_t, const bc_handle_ptr<T>& p_first)
	{
		return p_first.get() >= nullptr;
	}

	template <class T>
	T* operator+(const bc_handle_ptr<T>& p_first, bcINTPTR p_offset)
	{
		return p_first.get() + p_offset;
	}

	template <class T>
	T* operator+(const bc_handle_ptr<T>& p_first, bcUINTPTR p_offset)
	{
		return p_first.get() + p_offset;
	}

	template <class T>
	T* operator+(bcINTPTR p_offset, const bc_handle_ptr<T>& p_first)
	{
		return p_first.get() + p_offset;
	}

	template <class T>
	T* operator+(bcUINTPTR p_offset, const bc_handle_ptr<T>& p_first)
	{
		return p_first.get() + p_offset;
	}

	template <class T>
	T* operator-(const bc_handle_ptr<T>& p_first, bcINTPTR p_offset)
	{
		return p_first.get() - p_offset;
	}

	template <class T>
	T* operator-(const bc_handle_ptr<T>& p_first, bcUINTPTR p_offset)
	{
		return p_first.get() - p_offset;
	}

	template <class T>
	T* operator-(bcINTPTR p_offset, const bc_handle_ptr<T>& p_first)
	{
		return p_first.get() - p_offset;
	}

	template <class T>
	T* operator-(bcUINTPTR p_offset, const bc_handle_ptr<T>& p_first)
	{
		return p_first.get() - p_offset;
	}

#pragma endregion

#ifdef BC_MEMORY_DEFRAG

		template<typename T>
		using bc_ptr = bc_handle_ptr<T>;

#else

	template<typename T>
	using bc_ptr = bc_handle_ptr<T>;

#endif

	template<typename T, typename ...TArgs>
	bc_unique_ptr<T> bc_make_unique(TArgs&&... p_args)
	{
		return bc_make_unique<T>(bc_alloc_type::unknown, std::forward<TArgs>(p_args)...);
	}

	template<typename T, typename ...TArgs>
	bc_unique_ptr<T> bc_make_unique(bc_alloc_type p_alloc_type, TArgs&&... p_args)
	{
		return bc_unique_ptr<T>(BC_NEW(T(std::forward<TArgs>(p_args)...), p_alloc_type));
	}

	template<typename T, typename ...TArgs>
	bc_unique_ptr<T> bc_make_unique(bc_alloc_type p_alloc_type, bcUINT8 p_alignment, TArgs&&... p_args)
	{
		return bc_unique_ptr<T>(BC_ALIGNED_NEW(T(std::forward<TArgs>(p_args)...), p_alignment, p_alloc_type));
	}

	template<typename T, typename ...TArgs>
	bc_shared_ptr<T> bc_make_shared(TArgs&&... p_args)
	{
		return bc_make_shared<T>(bc_alloc_type::unknown, std::forward<TArgs>(p_args)...);
	}

	template<typename T, typename ...TArgs>
	bc_shared_ptr<T> bc_make_shared(bc_alloc_type p_alloc_type, TArgs&&... p_args)
	{
		using meta_type = typename bc_shared_ptr<T>::template meta_data_imp<bc_default_deleter>;

		const bcSIZE l_required_size = sizeof(T) + sizeof(meta_type);
		void* l_alloc = BC_ALLOC(l_required_size, p_alloc_type);
		if (!l_alloc)
		{
			throw std::bad_alloc();
		}

		T* l_pointer = static_cast<T*>(l_alloc);
		meta_type* l_meta_pointer = reinterpret_cast<meta_type*>(l_pointer + 1);

		new(l_pointer)T(std::forward<TArgs>(p_args)...);
		new(l_meta_pointer)meta_type(bc_default_deleter(), true);

		bc_shared_ptr<T> l_result;
		l_result._make(l_pointer, l_meta_pointer);

		return l_result;
	}

	template<typename T, typename TDeleter>
	void swap(bc_unique_ptr<T, TDeleter>& p_first, bc_unique_ptr<T, TDeleter>& p_second) noexcept
	{
		p_first.swap(p_second);
	}

	template<typename T>
	void swap(bc_shared_ptr<T>& p_first, bc_shared_ptr<T>& p_second) noexcept
	{
		p_first.swap(p_second);
	}

	template<typename T>
	void swap(bc_handle_ptr<T>& p_first, bc_handle_ptr<T>& p_second) noexcept
	{
		p_first.swap(p_second);
	}

	template<class T, class U>
	bc_shared_ptr<T> static_pointer_cast(const bc_shared_ptr<U>& r)
	{
		return bc_shared_ptr<T>(static_cast<T*>(r.get()));
	}

	template<class T, class U, class TDeleter>
	bc_unique_ptr<T, TDeleter> static_pointer_cast(bc_unique_ptr<U, TDeleter>& r)
	{
		return bc_unique_ptr<T, TDeleter>(static_cast<T*>(r.release()));
	}

	template<class T, class U>
	bc_shared_ptr<T> dynamic_pointer_cast(const bc_shared_ptr<U>& r)
	{
		return bc_shared_ptr<T>(dynamic_cast<T*>(r.get()));
	}

	template<class T, class U, class TDeleter>
	bc_unique_ptr<T, TDeleter> dynamic_pointer_cast(bc_unique_ptr<U, TDeleter>& r)
	{
		return bc_unique_ptr<T, TDeleter>(dynamic_cast<T*>(r.release()));
	}

	template<class T, class U>
	bc_shared_ptr<T> const_pointer_cast(const bc_shared_ptr<U>& r)
	{
		return bc_shared_ptr<T>(const_cast<T*>(r.get()));
	}

	template<class T, class U, class TDeleter>
	bc_unique_ptr<T, TDeleter> const_pointer_cast(bc_unique_ptr<U, TDeleter>& r)
	{
		return bc_unique_ptr<T, TDeleter>(const_cast<T*>(r.release()));
	}
}

namespace std
{
	template<class T>
	struct std::hash<black_cat::core::bc_unique_ptr<T>>
	{
		std::size_t operator ()(const black_cat::core::bc_unique_ptr<T>& p_pointer)
		{
			return std::hash<T*>()(p_pointer.get());
		}
	};

	template<class T>
	struct std::hash<black_cat::core::bc_shared_ptr<T>>
	{
		std::size_t operator ()(const black_cat::core::bc_shared_ptr<T>& p_pointer)
		{
			return std::hash<T*>()(p_pointer.get());
		}
	};

	template<class T>
	struct std::hash<black_cat::core::bc_handle_ptr<T>>
	{
		std::size_t operator ()(const black_cat::core::bc_handle_ptr<T>& p_pointer)
		{
			return std::hash<T*>()(p_pointer.get());
		}
	};
}
