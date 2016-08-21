// [02/16/2016 MRB]

#pragma once

#include "Core/Utility/bcRefCountPtr.h"
#include "Graphic/bcExport.h"
#include "Graphic/bcPlatformRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_platform_render_api>
		class bc_platform_device;

		using bc_device = bc_platform_device< g_current_platform_render_api >;

		class bc_device_object;

		class BC_GRAPHIC_DLL _bc_resource_ptr_deleter
		{
		public:
			_bc_resource_ptr_deleter();

			_bc_resource_ptr_deleter(bc_device* p_device);

			void operator ()(bc_device_object* p_resource) const;

		protected:

		private:
			bc_device* m_device;
		};

		template< typename TResource >
		using bc_resource_ptr = core::bc_ref_count_ptr< TResource, _bc_resource_ptr_deleter >;

		/*
		// Reference counting pointer for resources created by device
		// (device resources don't use movale memroy, so this smart pointer isn't movable aware)
		template<class TResource>
		class bc_resource_ptr
		{
		public:
			using pointer = TResource*;
			using element_type = TResource;

			template< typename T1 >
			friend class bc_resource_ptr;
			template<bc_platform_render_api>
			friend class bc_platform_device;

		public:
			bc_resource_ptr();

			bc_resource_ptr(std::nullptr_t);

			bc_resource_ptr(const bc_resource_ptr& p_other) noexcept(true);

			bc_resource_ptr(bc_resource_ptr&& p_other) noexcept(true);

			template<class TOtherResource>
			bc_resource_ptr(const bc_resource_ptr<TOtherResource>& p_other) noexcept(true);

			template<class TOtherResource>
			bc_resource_ptr(bc_resource_ptr<TOtherResource>&& p_other) noexcept(true);

			~bc_resource_ptr();

			bc_resource_ptr& operator=(std::nullptr_t) noexcept(true);

			bc_resource_ptr& operator=(const bc_resource_ptr& p_other) noexcept(true);

			bc_resource_ptr& operator=(bc_resource_ptr&& p_other) noexcept(true);

			template<class TOtherResource>
			bc_resource_ptr& operator=(const bc_resource_ptr<TOtherResource>& p_other) noexcept(true);

			template<class TOtherResource>
			bc_resource_ptr& operator=(bc_resource_ptr<TOtherResource>&& p_other) noexcept(true);

			void reset(pointer p_resource) noexcept(true);

			void swap(bc_resource_ptr& p_other) noexcept(true);

			pointer get() const noexcept(true);

			element_type& operator*() const;

			pointer operator->() const noexcept(true);

			bool operator==(std::nullptr_t) noexcept;

			bool operator!=(std::nullptr_t) noexcept;

		protected:

		private: 
			// Only device can create resource pointers
			explicit bc_resource_ptr(bc_device* p_device) noexcept(true);

			bc_resource_ptr(bc_device* p_device, std::nullptr_t) noexcept(true);

			bc_resource_ptr(bc_device* p_device, pointer p_pointer) noexcept(true);

			template<class TOtherResource>
			bc_resource_ptr(bc_device* p_device, TOtherResource* p_pointer) noexcept(true);

			void _construct(bc_device* p_device, pointer p_resource);

			void _destruct();

			template<typename T1>
			void _assign(const bc_resource_ptr< T1 >& p_other);

			template<typename T1>
			void _assign(bc_resource_ptr< T1 >&& p_other);

			pointer m_resource;
			bc_device* m_device;
		};

		template< class TResource >
		bc_resource_ptr<TResource>::bc_resource_ptr()
			: m_resource(nullptr),
			m_device(nullptr)
		{
		}

		template< class TResource >
		bc_resource_ptr<TResource>::bc_resource_ptr(std::nullptr_t)
			: bc_resource_ptr()
		{
		}

		template <class TResource >
		bc_resource_ptr<TResource>::bc_resource_ptr(const bc_resource_ptr& p_other) noexcept(true)
		{
			_assign(p_other);
		}

		template <class TResource >
		bc_resource_ptr<TResource>::bc_resource_ptr(bc_resource_ptr&& p_other) noexcept(true)
		{
			_assign(std::move(p_other));
		}

		template <class TResource >
		template <class TOtherResource >
		bc_resource_ptr<TResource>::bc_resource_ptr(const bc_resource_ptr<TOtherResource>& p_other) noexcept(true)
		{
			_assign(p_other);
		}

		template <class TResource >
		template <class TOtherResource >
		bc_resource_ptr<TResource>::bc_resource_ptr(bc_resource_ptr<TOtherResource>&& p_other) noexcept(true)
		{
			_assign(std::move(p_other));
		}

		template <class TResource >
		bc_resource_ptr<TResource>::~bc_resource_ptr()
		{
			_destruct();
		}

		template< class TResource >
		bc_resource_ptr<TResource>& bc_resource_ptr<TResource>::operator=(std::nullptr_t) noexcept(true)
		{
			reset(nullptr);

			return *this;
		}

		template <class TResource >
		bc_resource_ptr<TResource>& bc_resource_ptr<TResource>::operator=(const bc_resource_ptr& p_other) noexcept(true)
		{
			if(this != &p_other)
			{
				_assign(p_other);
			}
			
			return *this;
		}

		template <class TResource >
		bc_resource_ptr<TResource>& bc_resource_ptr<TResource>::operator=(bc_resource_ptr&& p_other) noexcept(true)
		{
			if (this != &p_other)
			{
				_assign(std::move(p_other));
			}

			return *this;
		}

		template <class TResource >
		template <class TOtherResource >
		bc_resource_ptr<TResource>& bc_resource_ptr<TResource>::operator=(const bc_resource_ptr<TOtherResource>& p_other) noexcept(true)
		{
			if (this != &p_other)
			{
				_assign(p_other);
			}

			return *this;
		}

		template <class TResource >
		template <class TOtherResource >
		bc_resource_ptr<TResource>& bc_resource_ptr<TResource>::operator=(bc_resource_ptr<TOtherResource>&& p_other) noexcept(true)
		{
			if (this != &p_other)
			{
				_assign(std::move(p_other));
			}

			return *this;
		}

		template< class TResource >
		void bc_resource_ptr<TResource>::reset(pointer p_resource) noexcept(true)
		{
			_destruct();
			_construct(m_device, p_resource);
		}

		template <class TResource >
		void bc_resource_ptr<TResource>::swap(bc_resource_ptr& p_other) noexcept(true)
		{
			std::swap(m_resource, p_other.m_resource);
			std::swap(m_device, p_other.m_device);
		}

		template <class TResource >
		typename bc_resource_ptr<TResource>::pointer bc_resource_ptr<TResource>::get() const noexcept(true)
		{
			return m_resource;
		}

		template <class TResource >
		typename bc_resource_ptr<TResource>::element_type& bc_resource_ptr<TResource>::operator*() const
		{
			return *m_resource;
		}

		template <class TResource >
		typename bc_resource_ptr<TResource>::pointer bc_resource_ptr<TResource>::operator->() const noexcept(true)
		{
			return m_resource;
		}

		template< class TResource >
		bool bc_resource_ptr<TResource>::operator==(std::nullptr_t) noexcept
		{
			return m_resource == nullptr;
		}

		template< class TResource >
		bool bc_resource_ptr<TResource>::operator!=(std::nullptr_t) noexcept
		{
			return !operator==(nullptr);
		}

		template <class TResource >
		bc_resource_ptr<TResource>::bc_resource_ptr(bc_device* p_device) noexcept(true)
		{
			_construct(nullptr, p_device);
		}

		template <class TResource >
		bc_resource_ptr<TResource>::bc_resource_ptr(bc_device* p_device, std::nullptr_t) noexcept(true)
			: bc_resource_ptr(p_device)
		{
		}

		template <class TResource >
		bc_resource_ptr<TResource>::bc_resource_ptr(bc_device* p_device, pointer p_pointer) noexcept(true)
		{
			_construct(p_device, p_pointer);
		}

		template <class TResource >
		template <class TOtherResource >
		bc_resource_ptr<TResource>::bc_resource_ptr(bc_device* p_device, TOtherResource* p_pointer) noexcept(true)
		{
			_construct(p_device, p_pointer);
		}

		template <class TResource >
		void bc_resource_ptr<TResource>::_construct(bc_device* p_device, pointer p_resource)
		{
			m_resource = p_resource;
			m_device = p_device;
			
			if(m_resource)
				static_cast<bc_device_object*>(m_resource)->add_ref();
		}

		template <class TResource >
		void bc_resource_ptr<TResource>::_destruct()
		{
			if(m_resource)
			{
				if(static_cast<bc_device_object*>(m_resource)->dec_ref())
				{
					m_device->destroy_resource(m_resource);
				}

				m_resource = nullptr;
			}
		}

		template< class TResource >
		template< typename T1 >
		void bc_resource_ptr< TResource >::_assign(const bc_resource_ptr< T1 >& p_other)
		{
			_destruct();
			_construct(p_other.m_device, p_other.m_resource);
		}

		template< class TResource >
		template< typename T1 >
		void bc_resource_ptr< TResource >::_assign(bc_resource_ptr< T1 >&& p_other)
		{
			_destruct();

			m_resource = p_other.m_resource;
			m_device = p_other.m_device;

			if (p_other.m_resource)
			{
				p_other.m_resource = nullptr;
			}
		}*/
	}
}