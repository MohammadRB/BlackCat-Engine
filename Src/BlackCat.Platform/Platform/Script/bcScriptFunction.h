// [10/10/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "Core/Container/bcArray.h"
#include "Platform/Script/bcScriptRef.h"
#include "Platform/Script/bcScriptReference.h"
#include "Platform/Script/bcScriptContext.h"
#include "Platform/Script/bcScriptVariable.h"

namespace black_cat
{
	namespace platform
	{
		template< typename ...TA >
		using bc_script_var_pack = core::bc_array< bc_script_variable, sizeof...(TA) >;

		template< core_platform::bc_platform TPlatform >
		struct bc_platform_script_function_base_pack
		{
		};

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_function_base : public bc_platform_script_reference< TPlatform >
		{
		public:
			using platform_pack = bc_platform_script_function_base_pack< TPlatform >;
			using callback_t = bc_script_variable(*)(bc_script_variable*, bcSIZE);
			friend bc_script_context;

		public:
			bc_platform_script_function_base();

			bc_platform_script_function_base(const bc_platform_script_function_base&) noexcept;

			~bc_platform_script_function_base();

			bc_platform_script_function_base& operator=(const bc_platform_script_function_base&) noexcept;

			bc_script_variable operator()(bc_script_variable& p_this, bc_script_variable* p_args, bcSIZE p_arg_count) const;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:
			bc_platform_script_function_base(bc_script_context& p_context/*, callback_t p_callback*/);

		private:
			platform_pack m_pack;
		};

		/**
		 * \brief Wrap a script function.
		 * \tparam TR Function return type.
		 * \tparam TA Function argument types.
		 */
		template< core_platform::bc_platform TPlatform, typename TR, typename ...TA >
		class bc_platform_script_function< TPlatform, TR(TA...) > : public bc_platform_script_function_base< TPlatform >
		{
		public:
			using callback_t = TR(*)(TA...);
			friend bc_script_context;
			template< core_platform::bc_platform TPlatform, typename TR, typename ...TA >
			friend class bc_platform_script_function;

		public:
			bc_platform_script_function();

			bc_platform_script_function(const bc_platform_script_function&) noexcept;

			~bc_platform_script_function();

			bc_platform_script_function& operator=(const bc_platform_script_function&) noexcept;

			TR operator()(bc_script_variable& p_this, TA&... p_args) const;

			/**
			 * \brief Convert provided arguments to function signature parameters and call callable object 
			 * with parameters
			 * \tparam TCallable 
			 * \param p_callable 
			 * \param p_args 
			 * \param p_arg_count 
			 * \return 
			 */
			template< typename TCallable >
			static TR _call_callback(TCallable p_callable, bc_script_variable* p_args, bcUINT32 p_arg_count);

		protected:

		private:
			bc_platform_script_function(bc_script_context& p_context/*, callback_t p_callback*/);

			static void _pack_args(bc_script_var_pack< TA... >& p_pack, const TA&... p_args);

			static void _unpack_args(bc_script_var_pack< TA... >& p_pack, const TA&... p_args);
		};

		using bc_script_function_base = bc_platform_script_function_base< core_platform::g_current_platform >;
		using bc_script_function_base_ref = bc_script_ref< bc_script_function_base >;

		template< typename TR, typename ...TA >
		using bc_script_function = bc_platform_script_function< core_platform::g_current_platform, TR, TA... >;
		template< typename TR, typename ...TA >
		using bc_script_function_ref = bc_script_ref< bc_script_function< TR, TA... > >;

		template <core_platform::bc_platform TPlatform, typename TR, typename ... TA >
		bc_platform_script_function< TPlatform, TR(TA...) >::bc_platform_script_function()
			: bc_platform_script_function_base()
		{
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ... TA >
		bc_platform_script_function< TPlatform, TR(TA...) >::bc_platform_script_function(bc_script_context& p_context/*, callback_t p_callback*/)
			: bc_platform_script_function_base(p_context)
		{
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ... TA >
		bc_platform_script_function< TPlatform, TR(TA...) >::bc_platform_script_function(const bc_platform_script_function& p_other) noexcept
		{
			operator=(p_other);
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ... TA >
		bc_platform_script_function< TPlatform, TR(TA...) >::~bc_platform_script_function()
		{
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ... TA >
		bc_platform_script_function< TPlatform, TR(TA...) >& bc_platform_script_function< TPlatform, TR(TA...) >::operator=(const bc_platform_script_function& p_other) noexcept
		{
			bc_platform_script_function_base::operator=(p_other);

			return *this;
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ...TA >
		TR bc_platform_script_function< TPlatform, TR(TA...) >::operator()(bc_script_variable& p_this, TA&... p_args) const
		{
			bc_script_variable l_call_result;
			bc_script_var_pack< TA... > l_packed_args;

			bc_platform_script_function::_pack_args(l_packed_args, p_args...);

			l_call_result = bc_platform_script_function_base::operator()(p_this, l_packed_args.data(), l_packed_args.size());

			TR l_result;
			bc_script_variable::_unpack_arg(l_call_result, &l_result);

			return l_result;
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ...TA >
		void bc_platform_script_function< TPlatform, TR(TA...) >::_pack_args(bc_script_var_pack< TA... >& p_pack, const TA&... p_args)
		{
			bcUINT32 l_counter = 0;

			auto l_expansion_list =
			{
				(
					[&l_counter, &p_pack, &p_args]()
					{
						p_pack[l_counter++] = bc_script_variable::_pack_arg(p_args);
						return true;
					}()
				)...
			};
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ...TA >
		void bc_platform_script_function< TPlatform, TR(TA...) >::_unpack_args(bc_script_var_pack< TA... >& p_pack, const TA&... p_args)
		{
			bcUINT32 l_counter = 0;

			auto l_expansion_list =
			{
				(
					[&l_counter, &p_pack, &p_args]()
					{
						bc_script_variable::_unpack_arg(p_pack[l_counter++], &p_args);
						return true;
					}()
				)...
			};
		}

		template< core_platform::bc_platform TPlatform, typename TR, typename ...TA >
		template< typename TCallable >
		TR bc_platform_script_function< TPlatform, TR(TA...) >::_call_callback(TCallable p_callable, bc_script_variable* p_args, bcUINT32 p_arg_count)
		{
			// TODO Make a choose for more and less argument count than expected
			BC_ASSERT(sizeof...(TA) == p_arg_count);

			bcUINT32 l_counter = 0;
			return p_callable
			(
				(
					[&l_counter, p_args]()
					{
						TA l_var;

						bc_script_variable::_unpack_arg(p_args[l_counter++], &l_var);

						return l_var;
					}()
				)...
			);
		}
	}
}