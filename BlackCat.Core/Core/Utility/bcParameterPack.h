// [02/24/2016 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Container/bcAllocator.h"

namespace black_cat
{
	namespace core
	{
		class _bc_parameter_pack_internal_buffer_base
		{
		public:
			_bc_parameter_pack_internal_buffer_base() = default;

			virtual ~_bc_parameter_pack_internal_buffer_base() = default;

			virtual void move(void* p_memory) = 0;

			virtual void* get_data() const = 0;
		};

		template< typename T >
		class _bc_parameter_pack_internal_buffer : public _bc_parameter_pack_internal_buffer_base
		{
		public:
			_bc_parameter_pack_internal_buffer(T& p_data)
				: m_data(p_data)
			{
			}

			_bc_parameter_pack_internal_buffer(T&& p_data)
				: m_data(std::move(p_data))
			{
			}

			~_bc_parameter_pack_internal_buffer() = default;

			void move(void* p_memory) override
			{
				new (p_memory) _bc_parameter_pack_internal_buffer(std::move(m_data));
			}

			void* get_data() const override
			{
				return &m_data;
			}

		protected:

		private:
			T m_data;
		};

		// This class can hold any data type, (for small data types it use an internall buffer 
		// and for large data memory will be allocated)
		class bc_parameter_pack : public bc_object_allocator
		{
		public:
			bc_parameter_pack();

			bc_parameter_pack(bc_parameter_pack&& p_other);

			~bc_parameter_pack();

			bc_parameter_pack& operator=(bc_parameter_pack&& p_other);

			template<typename T>
			void set_data(T& p_data);

			template<typename T>
			T* get_data() const;

			// Clear internal states if any exist
			void reset();

			bool has_data() const;

			bool used_internall_buffer() const;

		protected:

		private:
			static constexpr bcUINT s_buffer_size = 5 * sizeof(bcUINT);
			
			bcUINT m_buffer[s_buffer_size];

			bool m_used_internal_buffer;
			_bc_parameter_pack_internal_buffer_base* m_object;
		};

		inline bc_parameter_pack::bc_parameter_pack()
			: m_used_internal_buffer(false),
			m_object(nullptr)
		{
		}

		inline bc_parameter_pack::bc_parameter_pack(bc_parameter_pack&& p_other)
		{
			operator=(std::move(p_other));
		}

		inline bc_parameter_pack::~bc_parameter_pack()
		{
			reset();
		}

		inline bc_parameter_pack& bc_parameter_pack::operator=(bc_parameter_pack&& p_other)
		{
			if (p_other.m_used_internal_buffer)
			{
				p_other.m_object->move(&m_buffer);
				m_object = reinterpret_cast<_bc_parameter_pack_internal_buffer_base*>(&m_buffer);
				m_used_internal_buffer = true;
			}
			else
			{
				m_object = p_other.m_object;
				m_used_internal_buffer = false;
			}

			p_other.m_object = nullptr;
			p_other.m_used_internal_buffer = false;

			return *this;
		}
		
		template < typename T >
		void bc_parameter_pack::set_data(T& p_data)
		{
			reset();

			if (sizeof(_bc_parameter_pack_internal_buffer<T>) <= s_buffer_size)
			{
				new (&m_buffer) _bc_parameter_pack_internal_buffer<T>(p_data);
				m_object = reinterpret_cast<_bc_parameter_pack_internal_buffer_base*>(&m_buffer);
				m_used_internal_buffer = true;
			}
			else
			{
				m_object = allocate<_bc_parameter_pack_internal_buffer<T>>(p_data).release();
				m_used_internal_buffer = false;
			}
		}

		template < typename T >
		T* bc_parameter_pack::get_data() const
		{
			if (!has_data())
				return nullptr;

			return static_cast< T* >(m_object->get_data());
		}

		inline void bc_parameter_pack::reset()
		{
			// Free allocated memory
			if(m_object && !m_used_internal_buffer)
			{
				deallocate(m_object);
			}
			// Only destruct constructed object in internal buffer
			else if(m_object && m_used_internal_buffer)
			{
				m_object->~_bc_parameter_pack_internal_buffer_base();
			}

			m_used_internal_buffer = false;
			m_object = nullptr;
		}

		inline bool bc_parameter_pack::has_data() const
		{
			return m_object != nullptr;
		}

		inline bool bc_parameter_pack::used_internall_buffer() const
		{
			return m_used_internal_buffer;
		}
	}
}
