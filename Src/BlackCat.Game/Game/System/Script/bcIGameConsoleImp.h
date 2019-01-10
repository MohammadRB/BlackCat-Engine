// [10/22/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Game/System/Script/bcGameConsole.h"

namespace black_cat
{
	namespace game
	{
		class bc_game_console;

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
			virtual bool is_visible() = 0;

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