// [04/29/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcString.h"
#include "Core/Utility/bcServiceManager.h"
#include "Game/bcExport.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/System/Input/bcFileSystem.h"

namespace black_cat
{
	namespace game
	{
		class bc_game_system : public core::bc_iservice
		{
		public:
			bc_game_system() = default;

			bc_game_system(bc_game_system&&) = default;

			~bc_game_system() = default;

			bc_game_system& operator=(bc_game_system&&) = default;

			bc_render_system& get_render_system()
			{
				return m_render_system;
			}

			bc_input_system& get_input_system()
			{
				return m_input_system;
			}

			bc_file_system& get_file_system()
			{
				return m_file_system;
			}

			static const bcCHAR* service_name()
			{
				return core::g_srv_game_system;
			}

		protected:
			void update(core_platform::bc_clock::update_param p_clock_update_param) override
			{
				m_input_system.update(p_clock_update_param);
				m_render_system.update(bc_render_system::update_param
				(
					p_clock_update_param,
					m_input_system.get_camera().get_view(),
					m_input_system.get_camera().get_projection())
				);
			}

		private:
			bc_render_system m_render_system;
			bc_input_system m_input_system;
			bc_file_system m_file_system;
		};
	}
}