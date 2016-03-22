// [ MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Utility/bcTemplateMetaType.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_delegate;
			
		// Delegate class that use internall buffer for it's allocations and never call dynamic memory
		// allocation functions. in case of large functor objects that doesn't fit in this class use
		// bc_function_wrapper class
		template< typename TR, typename ...TA >
		class bc_delegate< TR(TA...) >
		{
		public:
			using this_type = bc_delegate<TR(TA...)>;
			using result_type = TR;
			using func_type = TR(*)(TA...);
			template< typename TClass > 
			using member_func_type = TR(TClass::*)(TA...);

		public:
			bc_delegate() noexcept
				: m_stub_call(nullptr),
				m_stub_action(nullptr)
			{
			}

			template<typename TObj>
			bc_delegate(TObj* p_objptr, TR(TObj::*p_func)(TA...)) noexcept
			{
				_bind(p_objptr, p_func);
			}

			template< typename TObj >
			bc_delegate(const TObj* p_objptr, TR(TObj::*p_func)(TA...) const) noexcept
			{
				_bind(p_objptr, p_func);
			}

			explicit bc_delegate(func_type p_funcptr) noexcept
			{
				_bind(p_funcptr);
			}

			bc_delegate(const this_type& p_other) noexcept
			{
				_assign(p_other);
			}

			bc_delegate(this_type&& p_other) noexcept // for compatibility
			{
				_assign(p_other);
			}

			template
			<
				typename TFunctor,
				// Disable template constructor for delegate type itself so copy ctor and move ctor has a chance to be called
				typename = typename std::enable_if
				<
					(!std::is_same< this_type, typename std::decay< TFunctor >::type >::value ||
					!std::is_same< func_type, typename std::decay< TFunctor >::type >::value) &&
					is_callable< TFunctor >::value // Check if TFunctor is actually a callable object
				>::type
			>
			bc_delegate(TFunctor p_functor) noexcept(std::is_nothrow_copy_constructible<TFunctor>::value)
			{
				_bind(p_functor);
			}

			~bc_delegate()
			{
				if(*this != nullptr)
					m_stub_action(m_buffer, nullptr, 2);
			};

			this_type& operator=(const this_type& p_other) noexcept
			{
				_assign(p_other);

				return *this;
			}

			this_type& operator=(this_type&& p_other) noexcept
			{
				_assign(p_other);

				return *this;
			}

			template< typename TObj >
			void bind(TObj* p_objptr, TR(TObj::*p_func)(TA...)) noexcept
			{
				_bind(p_objptr, p_func);
			}

			template< typename TObj >
			void bind(const TObj* p_objptr, TR(TObj::*p_func)(TA...) const) noexcept
			{
				_bind(p_objptr, p_func);
			}

			void bind(func_type p_funcptr) noexcept
			{
				_bind(p_funcptr);
			}

			template
			<
				typename TFunctor,
				// Disable template bind for func_type so appropriate bind function overload can be called 
				typename = typename std::enable_if
				<
					!std::is_same< func_type, typename std::decay< TFunctor >::type >::value && 
					is_callable< TFunctor >::value // Check if TFunctor is actually a callable object
				>::type
			>
			void bind(TFunctor p_functor) noexcept(std::is_nothrow_copy_constructible<TFunctor>::value)
			{
				_bind(p_functor);
			}

			void swap(const this_type& p_other) noexcept
			{
				std::swap(*this, p_other);
			}

			template < typename ...TA > // For perfect forwarding
			TR operator()(TA&&... p_args) const 
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

			void reset() noexcept
			{
				m_stub_call = nullptr;
				m_stub_action = nullptr;
			}

		protected:

		private:
			using _stub_call_type = TR(*)(void*, TA...);
			using _stub_action_type = void(*)(void*, void*, bcUINT8);

			// http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
			template< typename TFunctor, typename ...TA >
			static TR _func_stub_call(void* p_buffer, TA... p_args)
			{
				TFunctor* l_functor = static_cast<TFunctor*>(p_buffer);
				return (*l_functor)(std::forward<TA>(p_args)...);
			}

			template< typename TFunctor > // 1 copy-construction 2 destruction
			static void _func_stub_action(void* p_buffer, void* p_other, bcUINT8 p_action) 
			{
				if (p_action == 1)
				{
					TFunctor* l_other = static_cast<TFunctor*>(p_other);
					new (p_buffer)TFunctor(*l_other);
				}
				else
				{
					TFunctor* l_functor = static_cast<TFunctor*>(p_buffer);
					l_functor->~TFunctor();
				}
			}

			// wrapper for free functions
			template< typename TR, typename ...TA >
			class _functor_func
			{
			public:
				using this_type = _functor_func;
				using func_type = TR(*)(TA...);

			public:
				_functor_func(func_type p_func) noexcept(true)
					: m_func(p_func)
				{
				}

				_functor_func(const this_type& p_other) noexcept(true)
				{
					_assign(p_other);
				}

				~_functor_func() = default;

				this_type& operator =(const this_type& p_other) noexcept(true)
				{
					_assign(p_other);
					return *this;
				}

				template< typename ...TA >
				TR operator ()(TA&&... p_args)
				{
					return (*m_func)(std::forward<TA>(p_args)...);
				}
				
			protected:

			private:
				bcInline void _assign(const this_type& p_other)
				{
					m_func = p_other.m_func;
				}

				func_type m_func;
			};

			// wrapper for member functions
			template< typename TClass, typename TR, typename ...TA >
			class _functor_memeber_func
			{
			public:
				using this_type = _functor_memeber_func;
				using func_type = TR(TClass::*)(TA...);

			public:
				_functor_memeber_func(TClass* p_obj, func_type p_func) noexcept(true)
					: m_obj(p_obj),
					m_func(p_func)
				{
				}

				_functor_memeber_func(const this_type& p_other) noexcept(true)
				{
					_assign(p_other);
				}

				~_functor_memeber_func() = default;

				this_type& operator =(const this_type& p_other) noexcept(true)
				{
					_assign(p_other);
					return *this;
				}

				template< typename ...TA >
				TR operator ()(TA&&... p_args)
				{
					return (m_obj->*m_func)(std::forward<TA>(p_args)...);
				}

			protected:

			private:
				bcInline void _assign(const this_type& p_other)
				{
					m_obj = p_other.m_obj;
					m_func = p_other.m_func;
				}

				TClass* m_obj;
				func_type m_func;
			};

			// bind to member function
			template< typename TObj >
			bcInline void _bind(TObj* p_objptr, TR(TObj::*p_func)(TA...))
			{
				_bind(_functor_memeber_func<TObj, TR, TA...>(p_objptr, p_func));
			}

			// bind to const member function
			template< typename TObj >
			bcInline void _bind(const TObj* p_objptr, TR(TObj::*p_func)(TA...) const)
			{
				_bind(_functor_memeber_func<TObj, TR, TA...>(p_objptr, p_func));
			}

			// bind to static member function & free functions
			bcInline void _bind(func_type p_funcptr)
			{
				_bind(_functor_func<TR, TA...>(p_funcptr));
			}

			// bind to functor objects
			template< typename TFunctor >
			bcInline void _bind(TFunctor p_functor)
			{
				static_assert(sizeof(TFunctor) < s_buffer_size, "functor object is much large");

				m_stub_call = &_func_stub_call<TFunctor, TA...>;
				m_stub_action = &_func_stub_action<TFunctor>;
				
				m_stub_action(m_buffer, &p_functor, 1);
			}

			bcInline void _assign(const this_type& p_other)
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