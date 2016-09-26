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
		struct bc_game_system_parameter
		{
			explicit bc_game_system_parameter(const bc_render_system_parameter& p_render_system_parameter)
				: m_render_system_parameter(p_render_system_parameter)
			{
			}

			bc_render_system_parameter m_render_system_parameter;
		};

		class bc_game_system : public core::bc_iservice, public core::bc_initializable< bc_game_system_parameter >
		{
		public:
			bc_game_system() = default;

			bc_game_system(bc_game_system&&) = default;

			~bc_game_system();

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
			void update(core_platform::bc_clock::update_param p_clock_update_param) override;

		private:
			void _initialize(bc_game_system_parameter) override;

			void _destroy() override;

			bc_render_system m_render_system;
			bc_input_system m_input_system;
			bc_file_system m_file_system;
		};

		inline bc_game_system::~bc_game_system()
		{
			if (m_initialized)
			{
				destroy();
			}
		}

		inline void bc_game_system::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			bc_icamera::extend l_camera_extends;

			m_input_system.update(p_clock_update_param);
			m_input_system.get_camera().get_extend_points(l_camera_extends);

			auto l_render_system_update_params = bc_render_system::update_param
			(
				p_clock_update_param,
				m_input_system.get_camera().get_position(),
				m_input_system.get_camera().get_view(),
				m_input_system.get_camera().get_projection(),
				l_camera_extends
			);

			m_render_system.update(l_render_system_update_params);
		}

		inline void bc_game_system::_initialize(bc_game_system_parameter p_paramter)
		{
			m_render_system.initialize(p_paramter.m_render_system_parameter);
		}

		inline void bc_game_system::_destroy()
		{
			m_render_system.destroy();
		}
	}
}