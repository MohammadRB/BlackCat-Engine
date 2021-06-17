// [10/02/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "Core/Container/bcList.h"
#include "Core/Utility/bcParameterPack.h"

namespace black_cat
{
	namespace platform
	{
		template<core_platform::bc_platform TPlatform>
		class bc_platform_script_context;
		using bc_script_context = bc_platform_script_context<core_platform::g_current_platform>;

		template<core_platform::bc_platform TPlatform>
		class bc_platform_script_bytecode;
		using bc_script_bytecode = bc_platform_script_bytecode<core_platform::g_current_platform>;

		template<core_platform::bc_platform TPlatform>
		class bc_platform_script_variable;
		using bc_script_variable = bc_platform_script_variable<core_platform::g_current_platform>;

		template<typename T>
		class bc_script_external_object;

		template<core_platform::bc_platform TPlatform>
		struct bc_platform_script_runtime_pack
		{
		};

		/**
		 * \brief Incompatible with movable memory
		 * \tparam TPlatform 
		 */
		template<core_platform::bc_platform TPlatform>
		class bc_platform_script_runtime
		{
		public:
			using platform_pack = bc_platform_script_runtime_pack<TPlatform>;

		public:
			bc_platform_script_runtime() noexcept;

			bc_platform_script_runtime(bc_platform_script_runtime&&) = delete;

			~bc_platform_script_runtime();

			bc_platform_script_runtime& operator=(bc_platform_script_runtime&&) = delete;

			bc_script_context create_context();

			/**
			 * \brief Pass a context as active context, otherwise null to reset active context
			 * \param p_context 
			 */
			void set_active_context(bc_script_context* p_context);

			/**
			 * \brief Get current active context
			 * \return Active context or null
			 */
			bc_script_context* get_active_context() const;

			/**
			 * \brief Do any preprocessing required to run script
			 * \param p_script 
			 * \return 
			 */
			bc_script_bytecode compile_script(const bcWCHAR* p_script);

			/**
			 * \brief Run a script on active context. Throw invalid operation exception if there is no active context
			 * \param p_script 
			 * \return 
			 */
			bc_script_variable run_script(bc_script_bytecode& p_script);

			/**
			 * \brief Interrupt execution of script with an exception. 
			 * A running script will be terminated with an uncatchable exception as soon as possible.
			 * Can be called in a thread other than the one that runtime is running on.
			 */
			void interrupt_script_execution();

			///**
			// * \brief If runtime is in suspend state, put it in normal state
			// */
			//void enable_execution();

			/**
			 * \brief Hint runtime to run it's garbage collector
			 */
			void collect_garbage();

			bcSIZE memory_usage() const;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

			template<typename T, typename ...TA>
			static bc_script_external_object<T>* create_native(TA&&... p_args);

			template<typename T>
			static void destroy_native(bc_script_external_object<T>* p_object);

			template<typename T>
			typename bc_script_external_object<T>::meta_data* _create_external_object_meta_data();

		private:
			// Hold all external object meta data in one place so when runtime destroyed we can clear these data
			core::bc_list_program<core::bc_any> m_external_object_meta_data;
			platform_pack m_pack;
		};

		using bc_script_runtime = bc_platform_script_runtime<core_platform::g_current_platform>;
	}
}