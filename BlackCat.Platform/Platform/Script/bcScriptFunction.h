// [10/10/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "Core/Container/bcArray.h"
#include "Platform/Script/bcScriptReference.h"
#include "Platform/Script/bcScriptContext.h"
#include "Platform/Script/bcScriptVariable.h"

namespace black_cat
{
	namespace platform
	{
		template< typename ...TA >
		using bc_script_var_pack = core::bc_array< bc_script_variable, sizeof...(TA) >;

		template< core_platform::bc_platform TPlatform, typename TR, typename ...TA  >
		struct bc_platform_script_function_pack
		{
		};
		
		/**
		 * \brief Wrap a script function.
		 * Incompatible with movable memory.
		 * \tparam TR Function return type.
		 * \tparam TA Function argument types.
		 */
		template< core_platform::bc_platform TPlatform, typename TR, typename ...TA >
		class bc_platform_script_function< TPlatform, TR(TA...) > : public bc_platform_script_reference< TPlatform >
		{
		public:
			using platform_pack = bc_platform_script_function_pack< TPlatform, TR, TA... >;
			using callback_t = TR(*)(TA...);
			friend bc_script_context;
			template< core_platform::bc_platform TPlatform, typename TR, typename ...TA >
			friend class bc_platform_script_function;

		public:
			bc_platform_script_function();

			bc_platform_script_function(const bc_platform_script_function&) noexcept;

			~bc_platform_script_function();

			bc_platform_script_function& operator=(const bc_platform_script_function&) noexcept;

			TR operator()(bc_script_var_pack< bc_script_variable, TA... >& p_args) const;
			
			TR operator()(bc_script_variable& p_this, TA&... p_args) const;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

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
			bc_platform_script_function(bc_script_context& p_context, callback_t p_callback);

			static void _pack_args(bc_script_var_pack< TA... >& p_pack, const TA&... p_args);

			static void _unpack_args(bc_script_var_pack< TA... >& p_pack, const TA&... p_args);

			platform_pack m_pack;
		};

		template< typename TR, typename ...TA >
		using bc_script_function = bc_platform_script_function< core_platform::g_current_platform, TR, TA... >;

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
			// TODO Make a chiose for more and less argument count than expected
			bcAssert(sizeof...(TA) == p_arg_count);

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