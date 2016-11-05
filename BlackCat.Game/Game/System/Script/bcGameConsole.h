 // [10/22/2016 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
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
		enum class bc_console_output_type : bcUBYTE
		{
			info = static_cast< bcUBYTE >(core::bc_log_type::info),
			debug = static_cast< bcUBYTE >(core::bc_log_type::debug),
			error = static_cast< bcUBYTE >(core::bc_log_type::error),
			script = 8,
			_count = 4
		};

		class bc_igame_console_imp;

		class BC_GAME_DLL bc_game_console : private core::bc_ilog_listener
		{
		public:
			bc_game_console(bc_script_system& pP_script_system);

			bc_game_console(bc_game_console&&) noexcept;

			~bc_game_console();

			bc_game_console& operator=(bc_game_console&&) noexcept;

			void set_implementation(core::bc_unique_ptr< bc_igame_console_imp >&& p_imp);

			void run_script(const bcWCHAR* p_script);

			void disable_output(bc_console_output_type p_output) noexcept;

			void enable_output(bc_console_output_type p_output) noexcept;

			void clear_output();

			void update(core_platform::bc_clock::update_param p_clock_update_param);

		protected:

		private:
			void on_log(core::bc_log_type p_type, const bcECHAR* p_log) override;

			bool _on_key(core::bc_ievent& p_event);

			void _write_logs();

			bc_script_system& m_script_system;
			core::bc_unique_ptr< bc_igame_console_imp > m_imp;
			core::bc_array< bool, static_cast<bcSIZE>(bc_console_output_type::_count) > m_log_types;
			core::bc_vector< std::pair< bc_console_output_type, core::bc_estring > > m_logs;
			core_platform::bc_mutex m_logs_mutex;
			core::bc_wstring m_script;
			core_platform::bc_mutex m_script_mutex;
			core::bc_event_listener_handle m_key_event_handle;
		};

		/**
		 * \brief Implementation class must be thread safe due to multiple threads can write to game console
		 */
		class bc_igame_console_imp
		{
		public:
			explicit bc_igame_console_imp(bc_game_console& p_game_console)
				: m_game_console(p_game_console)
			{
			}

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
			bc_game_console& m_game_console;

		private:
		};
	}
}