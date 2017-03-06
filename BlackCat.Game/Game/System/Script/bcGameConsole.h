 // [10/22/2016 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcFuture.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcArray.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Event/bcEvent.h"
#include "Game/bcExport.h"
#include "Game/System/Script/bcScriptSystem.h"

namespace black_cat
{
	namespace game
	{
		class bc_game_console;
		class bc_igame_console_imp;

		enum class bc_console_output_type : bcUBYTE
		{
			info = static_cast< bcUBYTE >(core::bc_log_type::info),
			debug = static_cast< bcUBYTE >(core::bc_log_type::debug),
			error = static_cast< bcUBYTE >(core::bc_log_type::error),
			script = 8,
			_count = 4
		};

		class bc_game_console_bind
		{
		public:
			explicit bc_game_console_bind(bc_game_console& p_game_console);

			bc_game_console_bind(const bc_game_console_bind&);

			~bc_game_console_bind();

			bc_game_console_bind& operator=(const bc_game_console_bind&);

			void disable_output(const platform::bc_script_int& p_output);

			void enable_output(const platform::bc_script_int& p_output);

			void clear_output();

		protected:

		private:
			bc_game_console& m_game_console;
		};

		struct _bc_script_queue_entry
		{
			bool m_output_to_console;
			core::bc_wstring m_script;
			core_platform::bc_promise< platform::bc_script_variable > m_promise;
		};

		class BC_GAME_DLL bc_game_console : private core::bc_ilog_listener
		{
		public:
			explicit bc_game_console(bc_script_system& pP_script_system);

			bc_game_console(bc_game_console&&) noexcept;

			~bc_game_console();

			bc_game_console& operator=(bc_game_console&&) noexcept;

			/**
			 * \brief Set concrete console object. Ownership of object is by caller.
			 * \param p_imp 
			 */
			void set_implementation(bc_igame_console_imp* p_imp);

			void disable_output(bc_console_output_type p_output) noexcept;

			void enable_output(bc_console_output_type p_output) noexcept;

			void clear_output();

			void run_script(const bcWCHAR* p_script, bool p_output_to_console = false);

			void run_script(const bcWCHAR* p_script, core_platform::bc_future<platform::bc_script_variable>* p_result, bool p_output_to_console = false);

			void update(core_platform::bc_clock::update_param p_clock_update_param);

			static void bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bc_game_console& p_instance);

		protected:

		private:
			void on_log(core::bc_log_type p_type, const bcECHAR* p_log) override;

			bool _on_key(core::bc_ievent& p_event);

			void _write_logs();

			bc_script_system& m_script_system;
			bc_igame_console_imp* m_imp;

			core_platform::bc_mutex m_logs_mutex;
			core::bc_array< bool, static_cast<bcSIZE>(bc_console_output_type::_count) > m_log_types;
			core::bc_vector< std::pair< bc_console_output_type, core::bc_estring > > m_logs;

			core_platform::bc_mutex m_scripts_mutex;
			core::bc_vector< _bc_script_queue_entry > m_scripts;

			core::bc_event_listener_handle m_key_event_handle;

			platform::bc_script_context* m_bound_context;
			platform::bc_script_object_ref m_bound_console;
		};

		/**
		 * \brief Implementation class must be thread safe due to multiple threads can write to game console
		 */
		class bc_igame_console_imp
		{
		public:
			virtual ~bc_igame_console_imp() = default;
			
			/**
			 * \brief Write log message to output screen
			 * \param p_type 
			 * \param p_msg 
			 */
			virtual void write_output(bc_console_output_type p_type, const core::bc_estring& p_msg) = 0;

			/**
			 * \brief Clear output screen
			 */
			virtual void clear_output() = 0;

			/**
			 * \brief Show output screen
			 */
			virtual void show() = 0;

			/**
			 * \brief Hide output screen
			 */
			virtual void hide() = 0;

			/**
			 * \brief Method to update output screen if required
			 * \param p_clock_update_param 
			 */
			virtual void update(core_platform::bc_clock::update_param p_clock_update_param) = 0;

			/**
			 * \brief Check the visibility of output screen
			 * \return 
			 */
			virtual bool is_visibile() = 0;

		protected:
			explicit bc_igame_console_imp(bc_game_console& p_game_console)
				: m_game_console(p_game_console)
			{
				m_game_console.set_implementation(this);
			}

			bc_igame_console_imp(bc_igame_console_imp&& p_other) noexcept
				: m_game_console(p_other.m_game_console)
			{
			}

			bc_igame_console_imp& operator=(bc_igame_console_imp&& p_other) noexcept
			{
				return *this;
			}

			bc_game_console& m_game_console;

		private:
		};
	}
}