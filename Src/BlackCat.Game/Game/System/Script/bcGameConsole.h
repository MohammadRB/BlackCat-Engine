 // [10/22/2016 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcFuture.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcArray.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Messaging/Event/bcEvent.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "PlatformImp/Script/bcScriptObject.h"
#include "Game/System/Script/bcGameConsoleScript.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_script_system;
		class bc_game_console;
		class bci_game_console_imp;

		enum class bc_console_output_type : bcUBYTE
		{
			info = static_cast<bcUBYTE>(core::bc_log_type::info),
			debug = static_cast<bcUBYTE>(core::bc_log_type::debug),
			warning = static_cast<bcUBYTE>(core::bc_log_type::warning),
			error = static_cast<bcUBYTE>(core::bc_log_type::error),
			script = 16,
			_count = 5
		};

		struct _bc_script_queue_entry
		{
			bool m_output_to_console;
			core::bc_wstring m_script;
			core_platform::bc_promise< platform::bc_script_variable > m_promise;
		};

		class BC_GAME_DLL bc_game_console : private core::bci_log_listener
		{
		public:
			explicit bc_game_console(bc_script_system& p_script_system);

			bc_game_console(bc_game_console&&) noexcept;

			~bc_game_console() override;

			bc_game_console& operator=(bc_game_console&&) noexcept;
			
			/**
			 * \brief Set console implementation object. Ownership of object is by caller.
			 * \param p_imp 
			 */
			void set_implementation(bci_game_console_imp* p_imp);

			void disable_output(bc_console_output_type p_output) noexcept;

			void enable_output(bc_console_output_type p_output) noexcept;

			void clear_output();

			void run_script(const bcWCHAR* p_script, bool p_output_to_console = false);

			void run_script(const bcWCHAR* p_script, core_platform::bc_future<platform::bc_script_variable>* p_result, bool p_output_to_console = false);

			void update(const core_platform::bc_clock::update_param& p_clock);

			static void bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bc_game_console& p_instance);
			
		private:
			void on_log(core::bc_log_type p_type, const bcECHAR* p_log) override;

			bool _on_key(core::bci_event& p_event);

			bc_script_system* m_script_system;
			bci_game_console_imp* m_imp;
			
			core::bc_array<bool, static_cast<bcSIZE>(bc_console_output_type::_count)> m_log_types;

			core_platform::bc_mutex m_scripts_mutex;
			core::bc_vector<_bc_script_queue_entry> m_scripts;

			platform::bc_script_context* m_bound_context;
			platform::bc_script_object_ref m_bound_console;

			core::bc_event_listener_handle m_key_event_handle;
		};
	}
}