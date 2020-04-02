// [ MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Utility/bcTemplateMetaType.h"

namespace black_cat
{
	namespace core
	{
		// wrapper for free functions
		template< typename TR, typename ...TA >
		class _functor_func
		{
		public:
			using this_type = _functor_func;
			using func_type = TR(*)(TA...);

		public:
			_functor_func(func_type p_func) noexcept
				: m_func(p_func)
			{
			}

			_functor_func(const this_type& p_other) noexcept
			{
				_assign(p_other);
			}

			~_functor_func() = default;

			this_type& operator =(const this_type& p_other) noexcept
			{
				_assign(p_other);
				return *this;
			}

			TR operator ()(TA... p_args)
			{
				return (*m_func)(std::forward<TA>(p_args)...);
			}

		private:
			void _assign(const this_type& p_other)
			{
				m_func = p_other.m_func;
			}

			func_type m_func;
		};

		// wrapper for member functions
		template< typename TClass, typename TR, typename ...TA >
		class _functor_member_func
		{
		public:
			using this_type = _functor_member_func;
			using func_type = TR(TClass::*)(TA...);

		public:
			_functor_member_func(TClass* p_obj, func_type p_func) noexcept
				: m_obj(p_obj),
				m_func(p_func)
			{
			}

			_functor_member_func(const this_type& p_other) noexcept
			{
				_assign(p_other);
			}

			~_functor_member_func() = default;

			this_type& operator =(const this_type& p_other) noexcept
			{
				_assign(p_other);
				return *this;
			}

			TR operator ()(TA... p_args)
			{
				return (m_obj->*m_func)(std::forward<TA>(p_args)...);
			}

		private:
			void _assign(const this_type& p_other)
			{
				m_obj = p_other.m_obj;
				m_func = p_other.m_func;
			}

			TClass* m_obj;
			func_type m_func;
		};

		// wrapper for const member functions
		template< typename TClass, typename TR, typename ...TA >
		class _functor_const_member_func
		{
		public:
			using this_type = _functor_const_member_func;
			using func_type = TR(TClass::*)(TA...) const;

		public:
			_functor_const_member_func(const TClass* p_obj, func_type p_func) noexcept
				: m_obj(p_obj),
				m_func(p_func)
			{
			}

			_functor_const_member_func(const this_type& p_other) noexcept
			{
				_assign(p_other);
			}

			~_functor_const_member_func() = default;

			this_type& operator =(const this_type& p_other) noexcept
			{
				_assign(p_other);
				return *this;
			}

			TR operator ()(TA... p_args)
			{
				return (m_obj->*m_func)(std::forward<TA>(p_args)...);
			}

		private:
			void _assign(const this_type& p_other)
			{
				m_obj = p_other.m_obj;
				m_func = p_other.m_func;
			}

			const TClass* m_obj;
			func_type m_func;
		};

		enum class bc_alloc_type : bcUBYTE;

		template< typename T >
		class bc_delegate;
		
		// Delegate class that use internal buffer for it's allocations and never call dynamic memory
		// allocation functions. In case of large functor objects that doesn't fit in this class use
		// make_from_big_object method
		template< typename TR, typename ...TA >
		class bc_delegate< TR(TA...) >
		{
		public:
			using this_type = bc_delegate<TR(TA...)>;
			using result_type = TR;
			using func_type = TR(*)(TA...);
			template< typename TClass > 
			using member_func_type = TR(TClass::*)(TA...);
			template< typename TClass >
			using const_member_func_type = TR(TClass::*)(TA...) const;

		public:
			bc_delegate() noexcept
				: m_stub_call(nullptr),
				m_stub_action(nullptr),
				m_buffer()
			{
			}

			template<typename TObj>
			bc_delegate(TObj& p_obj, member_func_type<TObj> p_func) noexcept
			{
				_bind(p_obj, p_func);
			}

			template< typename TObj >
			bc_delegate(const TObj& p_obj, const_member_func_type<TObj> p_func) noexcept
			{
				_bind(p_obj, p_func);
			}

			explicit bc_delegate(func_type p_func_ptr) noexcept
			{
				_bind(p_func_ptr);
			}

			bc_delegate(const this_type& p_other) noexcept
			{
				_assign(p_other);
			}

			bc_delegate(this_type&& p_other) noexcept // for compatibility
			{
				_assign(std::move(p_other));
			}

			template
			<
				typename TFunctor,
				typename = std::enable_if_t // Disable template constructor for delegate type itself so copy ctor and move ctor has a chance to be called
				<
					(
						!std::is_same_v< this_type, std::decay_t< TFunctor > > ||
						!std::is_same_v< func_type, std::decay_t< TFunctor > >
					) && bc_is_callable< TFunctor >::value // Check if TFunctor is actually a callable object
				>
			>
			bc_delegate(TFunctor p_functor) noexcept(std::is_nothrow_copy_constructible< TFunctor >::value)
			{
				_bind(p_functor);
			}

			~bc_delegate()
			{
				reset();
			};

			this_type& operator=(const this_type& p_other) noexcept
			{
				_assign(p_other);

				return *this;
			}

			this_type& operator=(this_type&& p_other) noexcept
			{
				_assign(std::move(p_other));

				return *this;
			}

			template< typename TObj >
			void bind(TObj* p_obj_ptr, member_func_type<TObj> p_func) noexcept
			{
				_bind(p_obj_ptr, p_func);
			}

			template< typename TObj >
			void bind(const TObj* p_obj_ptr, const_member_func_type<TObj> p_func) noexcept
			{
				_bind(p_obj_ptr, p_func);
			}

			void bind(func_type p_func_ptr) noexcept
			{
				_bind(p_func_ptr);
			}

			template
			<
				typename TFunctor,
				// Disable template bind for func_type so appropriate bind function overload can be called 
				typename = typename std::enable_if
				<
					!std::is_same< func_type, typename std::decay< TFunctor >::type >::value && 
					bc_is_callable< TFunctor >::value // Check if TFunctor is actually a callable object
				>::type
			>
			void bind(TFunctor& p_functor) noexcept(std::is_nothrow_copy_constructible<TFunctor>::value)
			{
				_bind(p_functor);
			}

			void swap(const this_type& p_other) noexcept
			{
				std::swap(*this, p_other);
			}

			void reset() noexcept
			{
				if (*this != nullptr)
				{
					m_stub_action(m_buffer, nullptr, 2);

					m_stub_call = nullptr;
					m_stub_action = nullptr;
				}
			}

			bool is_valid() const noexcept
			{
				return *this != nullptr;
			}
			
			TR operator()(TA... p_args) const 
			{
				bcAssert(*this != nullptr);

				return m_stub_call(const_cast<bcCHAR*>(m_buffer), std::forward<TA>(p_args)...);
			}

			bool operator==(std::nullptr_t) const noexcept
			{
				return m_stub_call == nullptr;
			}

			bool operator !=(std::nullptr_t) const noexcept
			{
				return !operator ==(nullptr);
			}

			template< typename TFunctor >
			static this_type make_from_big_object(bc_alloc_type p_alloc_type, TFunctor&& p_functor);

			template< typename TFunctor >
			static this_type make_from_big_object(TFunctor&& p_functor);

		private:
			using _stub_call_type = TR(*)(void*, TA...);
			using _stub_action_type = void(*)(void*, void*, bcUINT8);

			// http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
			template< typename TFunctor >
			static TR _func_stub_call(void* p_buffer, TA... p_args)
			{
				TFunctor* l_functor = static_cast<TFunctor*>(p_buffer);
				return (*l_functor)(std::forward<TA>(p_args)...);
			}

			template< typename TFunctor > // 1 copy-construction, 2 destruction
			static void _func_stub_action(void* p_buffer, void* p_other, const bcUINT8 p_action) 
			{
				if (p_action == 1)
				{
					TFunctor* l_other = static_cast<TFunctor*>(p_other);
					new (p_buffer)TFunctor(std::move(*l_other));		// Try to move functor object because it's possible 												
				}													// functor object not be copyable
				else
				{
					TFunctor* l_functor = static_cast<TFunctor*>(p_buffer);
					l_functor->~TFunctor();
				}
			}

			// bind to member function
			template< typename TObj >
			void _bind(TObj& p_obj, member_func_type<TObj> p_func)
			{
				_bind(_functor_member_func<TObj, TR, TA...>(&p_obj, p_func));
			}

			// bind to const member function
			template< typename TObj >
			void _bind(const TObj& p_obj, const_member_func_type<TObj> p_func)
			{
				_bind(_functor_const_member_func<TObj, TR, TA...>(&p_obj, p_func));
			}

			// bind to static member function & free functions
			void _bind(func_type p_func_ptr)
			{
				_bind(_functor_func<TR, TA...>(p_func_ptr));
			}

			// bind to functor objects
			template< typename TFunctor >
			void _bind(TFunctor& p_functor)
			{
				static_assert(sizeof(TFunctor) <= s_buffer_size, "Functor object is much large");

				m_stub_call = &_func_stub_call<TFunctor>;
				m_stub_action = &_func_stub_action<TFunctor>;
				
				m_stub_action(m_buffer, &p_functor, 1);
			}

			void _assign(const this_type& p_other)
			{
				m_stub_call = p_other.m_stub_call;
				m_stub_action = p_other.m_stub_action;
				
				m_stub_action(m_buffer, const_cast<bcCHAR*>(p_other.m_buffer), 1);
			}

			void _assign(this_type&& p_other)
			{
				m_stub_call = p_other.m_stub_call;
				m_stub_action = p_other.m_stub_action;

				m_stub_action(m_buffer, const_cast<bcCHAR*>(p_other.m_buffer), 1);
			}

			static const bcUINT32 s_buffer_size = sizeof(void*) * 5;

			_stub_call_type m_stub_call;
			_stub_action_type m_stub_action;
			bcCHAR m_buffer[s_buffer_size];
		};
	}
}