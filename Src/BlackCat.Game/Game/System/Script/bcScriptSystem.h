 // [10/21/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Container/bcString.h"
#include "Core/Memory/bcPtr.h"
#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptRuntime.h"
#include "PlatformImp/Script/bcScriptContext.hpp"
#include "PlatformImp/Script/bcScriptByteCode.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptString.h"
#include "PlatformImp/Script/bcScriptArray.h"
#include "PlatformImp/Script/bcScriptObject.h"
#include "PlatformImp/Script/bcScriptFunction.h"
#include "PlatformImp/Script/bcScriptError.h"
#include "PlatformImp/Script/bcScriptPrototype.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_script_binding;

		enum class bc_script_context : bcUBYTE
		{
			app = 0,
			_count = 1
		};

		class BC_GAME_DLL bc_script_system : public core::bc_initializable<bool>
		{
		public:
			bc_script_system();

			bc_script_system(bc_script_system&&) noexcept;

			~bc_script_system();

			bc_script_system& operator=(bc_script_system&&) noexcept;

			platform::bc_script_context& get_app_context() noexcept
			{
				return *m_app_context;
			}
			
			const platform::bc_script_context& get_app_context() const noexcept
			{
				return m_app_context.get();
			}

			bc_script_binding get_script_binder() const noexcept;

			void set_script_binder(bc_script_binding&& p_bindings);

			/**
			 * \brief Throw bc_script_compile_exception if compilation fail
			 * \param p_context 
			 * \param p_script_file 
			 * \return 
			 */
			platform::bc_script_bytecode load_script(bc_script_context p_context, const bcECHAR* p_script_file);

			/**
			 * \brief Return bc_script_error if execution fail
			 * \param p_context 
			 * \param p_script 
			 * \return 
			 */
			platform::bc_script_variable run_script(bc_script_context p_context, const bcWCHAR* p_script);

			/**
			* \brief Return bc_script_error if execution fail
			* \param p_context
			* \param p_script
			* \return
			*/
			platform::bc_script_variable run_script(bc_script_context p_context, platform::bc_script_bytecode& p_script);

			/**
			 * \brief Throw bc_script_compile_exception or bc_script_execution_exception if fail
			 * \param p_context 
			 * \param p_script 
			 * \return 
			 */
			platform::bc_script_variable run_script_throw(bc_script_context p_context, const bcWCHAR* p_script);

			/**
			 * \brief Throw bc_script_execution_exception if fail
			 * \param p_context 
			 * \param p_script 
			 * \return 
			 */
			platform::bc_script_variable run_script_throw(bc_script_context p_context, platform::bc_script_bytecode& p_script);

			core::bc_wstring stringify(platform::bc_script_variable& p_value);

			void interrupt_script_execution();

			void collect_garbage();

			bcSIZE memory_usage() const;

			void update(core_platform::bc_clock::update_param p_clock_update_param);

		protected:
			void _initialize(bool) override;

			void _destroy() override;
			
		private:
			platform::bc_script_context& _get_context(bc_script_context p_context)
			{
				switch (p_context)
				{
				case bc_script_context::app:
					return m_app_context.get();
				default: 
					throw bc_invalid_argument_exception("Invalid script context");
				}
			}

			core::bc_unique_ptr<platform::bc_script_runtime> m_script_runtime;
			platform::bc_script_context_ref m_app_context;
			platform::bc_script_function_wrapper_ref<platform::bc_script_string(platform::bc_script_variable)> m_stringify_function;
		};
	}
}