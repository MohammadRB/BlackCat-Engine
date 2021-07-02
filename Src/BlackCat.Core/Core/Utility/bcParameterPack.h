// [02/24/2016 MRB]

#pragma once

#include <utility>

#include "Core/CorePCH.h"
#include "Core/bcException.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Container/bcAllocator.h"

namespace black_cat
{
	namespace core
	{
		class _bc_parameter_pack_object
		{
		public:
			_bc_parameter_pack_object() = default;

			virtual ~_bc_parameter_pack_object() = default;

			virtual bcSIZE size() const = 0;

			virtual _bc_parameter_pack_object* copy(void* p_memory) = 0;

			virtual _bc_parameter_pack_object* move(void* p_memory) = 0;
		};

		template<typename T>
		class _bc_parameter_pack_concrete_object : public _bc_parameter_pack_object
		{
		public:
			explicit _bc_parameter_pack_concrete_object(T p_data)
				: _bc_parameter_pack_object(),
				m_value(std::move(p_data))
			{
			}

			~_bc_parameter_pack_concrete_object() override = default;

			bcSIZE size() const override
			{
				return sizeof(_bc_parameter_pack_concrete_object<T>);
			}

			_bc_parameter_pack_object* copy(void* p_memory) override
			{
				auto* l_pointer = static_cast<_bc_parameter_pack_concrete_object*>(p_memory);
				new (l_pointer) _bc_parameter_pack_concrete_object(m_value);

				return l_pointer;
			}

			_bc_parameter_pack_object* move(void* p_memory) override
			{
				auto* l_pointer = static_cast<_bc_parameter_pack_concrete_object*>(p_memory);
				new (l_pointer) _bc_parameter_pack_concrete_object(std::move(m_value));

				return l_pointer;
			}

			T m_value;
		};

		/**
		 * \brief This class can hold any data type, which for small data types it use an internal buffer and for large data,
		 * memory will be allocated from frame allocator.
		 * You can use both copy and movable objects but if you use movable only object and try to copy this object
		 * a logic exception will be thrown
		 */
		class bc_parameter_pack : public bc_object_allocator
		{
			template<typename T>
			using decay_t = std::remove_cv_t<std::remove_reference_t<T>>;
			
		public:
			bc_parameter_pack();

			bc_parameter_pack(const bc_parameter_pack& p_other);

			bc_parameter_pack(bc_parameter_pack&& p_other) noexcept;

			template<typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, bc_parameter_pack>>>
			explicit bc_parameter_pack(T&& p_value);

			~bc_parameter_pack();

			bc_parameter_pack& operator=(const bc_parameter_pack& p_other);

			bc_parameter_pack& operator=(bc_parameter_pack&& p_other) noexcept;

			template<typename T>
			void set_value(T&& p_data);

			template<typename T>
			bool is() const;

			/**
			 * \brief Cast underlying type to requested type.
			 * \tparam T
			 * \return Return null if underlying type mismatch
			 */
			template<typename T>
			T* as();

			template<typename T>
			const T* as() const;

			/**
			 * \brief Cast underlying type to requested type. Throw bad_cast exception if underlying type mismatch
			 * \tparam T
			 * \return
			 */
			template<typename T>
			T& as_throw();

			template<typename T>
			const T& as_throw() const;

			template<typename T>
			T release_as();

			/**
			 * \brief Clear internal states if any exist
			 */
			void reset();

			bool has_value() const;

			bool used_internal_buffer() const;

		private:
			static constexpr bcUINT s_buffer_size = 5 * sizeof(void*);

			bcBYTE m_buffer[s_buffer_size];
			_bc_parameter_pack_object* m_object;
		};

		/**
		 * \brief Same as bc_parameter_pack but does't use frame allocator by default.
		 */
		class bc_any : public bc_parameter_pack
		{
		public:
			bc_any();

			explicit bc_any(const bc_parameter_pack& p_parameter);

			template
			<
				typename T,
				typename = std::enable_if_t
				<
					!std::is_same_v<std::decay_t<T>, bc_parameter_pack> &&
					!std::is_same_v<std::decay_t<T>, bc_any>
				>>
			explicit bc_any(T&& p_value);
			
			bc_any(const bc_any& p_other);

			bc_any(bc_any&& p_other) noexcept;

			~bc_any();

			bc_any& operator=(const bc_parameter_pack& p_other);

			bc_any& operator=(const bc_any& p_other);

			bc_any& operator=(bc_any&& p_other) noexcept;
		};

		inline bc_parameter_pack::bc_parameter_pack()
			: m_buffer(),
			m_object(nullptr)
		{
			set_allocator_alloc_type(bc_alloc_type::frame);
		}

		inline bc_parameter_pack::bc_parameter_pack(const bc_parameter_pack& p_other)
			: bc_parameter_pack()
		{
			operator=(p_other);
		}

		inline bc_parameter_pack::bc_parameter_pack(bc_parameter_pack&& p_other) noexcept
			: bc_parameter_pack()
		{
			operator=(std::move(p_other));
		}

		template<typename T, typename>
		bc_parameter_pack::bc_parameter_pack(T&& p_value)
			: bc_parameter_pack()
		{
			set_value(std::forward<T>(p_value));
		}

		inline bc_parameter_pack::~bc_parameter_pack()
		{
			reset();
		}

		inline bc_parameter_pack& bc_parameter_pack::operator=(const bc_parameter_pack& p_other)
		{
			reset();

			if(!p_other.has_value())
			{
				return *this;
			}

			if (p_other.used_internal_buffer())
			{
				m_object = p_other.m_object->copy(&m_buffer);
			}
			else
			{
				bcBYTE* l_buffer = allocate_raw(p_other.m_object->size()).release();
				m_object = p_other.m_object->copy(l_buffer);
			}

			return *this;
		}

		inline bc_parameter_pack& bc_parameter_pack::operator=(bc_parameter_pack&& p_other) noexcept
		{
			reset();

			if (!p_other.has_value())
			{
				return *this;
			}

			if (p_other.used_internal_buffer())
			{
				m_object = p_other.m_object->move(&m_buffer);
			}
			else
			{
				m_object = p_other.m_object;
			}

			p_other.m_object = nullptr;

			return *this;
		}

		template <typename T>
		void bc_parameter_pack::set_value(T&& p_data)
		{
			// Because function is perfect forwarded if parameter be l-value T will be deduced to T&
			using T1 = decay_t<T>;

			reset();

			if (sizeof(_bc_parameter_pack_concrete_object<T1>) <= s_buffer_size)
			{
				new (&m_buffer) _bc_parameter_pack_concrete_object<T1>(std::forward<T>(p_data));
				m_object = reinterpret_cast<_bc_parameter_pack_object*>(&m_buffer);
			}
			else
			{
				m_object = allocate<_bc_parameter_pack_concrete_object<T1>>(std::forward<T>(p_data)).release();
			}
		}

		template<typename T>
		bool bc_parameter_pack::is() const
		{
			using T1 = decay_t<T>;
			
			if (!has_value())
			{
				return false;
			}

			return dynamic_cast<_bc_parameter_pack_concrete_object<T1>*>(m_object) != nullptr;
		}

		template <typename T>
		T* bc_parameter_pack::as()
		{
			using T1 = decay_t<T>;
			
			if (!has_value())
			{
				return nullptr;
			}

			auto* l_imp = dynamic_cast<_bc_parameter_pack_concrete_object<T1>*>(m_object);
			if (l_imp)
			{
				return static_cast<T*>(&l_imp->m_value);
			}

			return nullptr;
		}

		template<typename T>
		const T* bc_parameter_pack::as() const
		{
			return const_cast<bc_parameter_pack&>(*this).as<T>();
		}

		template <typename T>
		T& bc_parameter_pack::as_throw()
		{
			T* l_value = as<T>();

			if (l_value == nullptr)
			{
				throw bc_bad_cast_exception();
			}

			return *l_value;
		}

		template<typename T>
		const T& bc_parameter_pack::as_throw() const
		{
			return const_cast<bc_parameter_pack&>(*this).as_throw<T>();
		}

		template<typename T>
		T bc_parameter_pack::release_as()
		{
			T l_data(std::move(*as<T>()));

			reset();

			return l_data;
		}

		inline void bc_parameter_pack::reset()
		{
			// Free allocated memory
			if (m_object && !used_internal_buffer())
			{
				deallocate(m_object);
			}
			// Only destruct constructed object in internal buffer
			else if (m_object && used_internal_buffer())
			{
				m_object->~_bc_parameter_pack_object();
			}

			m_object = nullptr;
		}

		inline bool bc_parameter_pack::has_value() const
		{
			return m_object != nullptr;
		}

		inline bool bc_parameter_pack::used_internal_buffer() const
		{
			return static_cast<const void*>(m_object) == static_cast<const void*>(&m_buffer[0]);
		}

		inline bc_any::bc_any()
			: bc_parameter_pack()
		{
			set_allocator_alloc_type(bc_alloc_type::unknown);
		}

		inline bc_any::bc_any(const bc_parameter_pack& p_parameter)
			: bc_any()
		{
			operator=(p_parameter);
		}

		template<typename T, typename>
		bc_any::bc_any(T&& p_value)
			: bc_any()
		{
			set_value(std::forward<T>(p_value));
		}
		
		inline bc_any::bc_any(const bc_any& p_other) = default;

		inline bc_any::bc_any(bc_any&& p_other) noexcept = default;

		inline bc_any::~bc_any() = default;

		inline bc_any& bc_any::operator=(const bc_parameter_pack& p_other)
		{
			bc_parameter_pack::operator=(p_other);

			return *this;
		}

		inline bc_any& bc_any::operator=(const bc_any& p_other) = default;

		inline bc_any& bc_any::operator=(bc_any&& p_other) noexcept = default;
	}
}