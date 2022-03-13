// [05/22/2016 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/File/bcFileInfo.h"
#include "Core/Container/bcString.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/File/bcPath.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Game/Application/bcRenderApplication.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/bcEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_file_system::bc_file_system(core::bc_content_manager& p_content_manager, core::bc_content_stream_manager& p_content_stream_manager)
			: m_content_manager(&p_content_manager),
			m_content_stream_manager(&p_content_stream_manager),
			m_global_config_last_write_time(0),
			m_global_config_last_check_time(0)
		{
			const auto l_execute_path = core::bc_path(core::bc_path(core::bc_path::get_program_path()).get_directory());
			m_execute_path = l_execute_path.get_string();

			auto l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content")));
			m_content_base_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content\\Data")));
			m_content_data_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content\\Texture")));
			m_content_texture_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content\\Sound")));
			m_content_sound_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content\\Model")));
			m_content_model_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine
			(
				core::bc_path
				(
					(
						core::bc_estring_frame(bcL("Content\\Shader\\"))
						+
						core::bc_to_exclusive_wstring(graphic::bc_render_api_info::api_name()).c_str()
					).c_str()
				)
			);
			m_content_platform_shader_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content\\Script")));
			m_content_script_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content\\Scene")));
			m_content_scene_path = l_temp.get_string();

			const auto l_config_name = core::bc_estring_frame(bc_get_application().get_app_name()) + bcL(".config");
			m_global_config = core::bc_make_unique<bc_global_config>
			(
				core::bc_alloc_type::program,
				bc_global_config(m_content_base_path.c_str(), l_config_name.c_str())
			);
			g_global_config = m_global_config.get();
		}

		bc_file_system::bc_file_system(bc_file_system&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_file_system::~bc_file_system() = default;

		bc_file_system& bc_file_system::operator=(bc_file_system&& p_other) noexcept
		{
			m_content_manager = p_other.m_content_manager;
			m_content_stream_manager = p_other.m_content_stream_manager;

			m_execute_path = std::move(p_other.m_execute_path);
			m_content_base_path = std::move(p_other.m_content_base_path);
			m_content_data_path = std::move(p_other.m_content_data_path);
			m_content_texture_path = std::move(p_other.m_content_texture_path);
			m_content_model_path = std::move(p_other.m_content_model_path);
			m_content_platform_shader_path = std::move(m_content_platform_shader_path);
			m_content_script_path = std::move(m_content_script_path);
			m_content_scene_path = std::move(m_content_scene_path);

			m_global_config = std::move(p_other.m_global_config);
			m_global_config_last_check_time = p_other.m_global_config_last_check_time;

			return *this;
		}

		core::bc_content_manager& bc_file_system::get_content_manager()
		{
			return *m_content_manager;
		}

		const core::bc_content_manager& bc_file_system::get_content_manager() const
		{
			return *m_content_manager;
		}

		core::bc_content_stream_manager& bc_file_system::get_content_stream_manager()
		{
			return *m_content_stream_manager;
		}

		const core::bc_content_stream_manager& bc_file_system::get_content_stream_manager() const
		{
			return *m_content_stream_manager;
		}

		core::bc_estring_view bc_file_system::get_content_path() const noexcept
		{
			return m_content_base_path;
		}

		core::bc_estring bc_file_system::get_content_path(const bcECHAR* p_path) const noexcept
		{
			core::bc_path l_path(get_content_path());
			l_path.combine(core::bc_path(p_path));

			return l_path.get_string();
		}

		core::bc_estring_view bc_file_system::get_content_data_path() const noexcept
		{
			return m_content_data_path;
		}

		core::bc_estring bc_file_system::get_content_data_path(const bcECHAR* p_data_path) const noexcept
		{
			core::bc_path l_path(get_content_data_path());
			l_path.combine(core::bc_path(p_data_path));

			return l_path.get_string();
		}

		core::bc_estring_view bc_file_system::get_content_texture_path() const noexcept
		{
			return m_content_texture_path;
		}

		core::bc_estring bc_file_system::get_content_texture_path(const bcECHAR* p_texture_path) const noexcept
		{
			core::bc_path l_path(get_content_texture_path());
			l_path.combine(core::bc_path(p_texture_path));

			return l_path.get_string();
		}

		core::bc_estring_view bc_file_system::get_content_sound_path() const noexcept
		{
			return m_content_sound_path;
		}

		core::bc_estring bc_file_system::get_content_sound_path(const bcECHAR* p_sound_path) const noexcept
		{
			core::bc_path l_path(get_content_sound_path());
			l_path.combine(core::bc_path(p_sound_path));

			return l_path.get_string();
		}

		core::bc_estring_view bc_file_system::get_content_model_path() const noexcept
		{
			return m_content_model_path;
		}

		core::bc_estring bc_file_system::get_content_model_path(const bcECHAR* p_model_path) const noexcept
		{
			core::bc_path l_path(get_content_model_path());
			l_path.combine(core::bc_path(p_model_path));

			return l_path.get_string();
		}

		core::bc_estring_view bc_file_system::get_content_platform_shader_path() const noexcept
		{
			return m_content_platform_shader_path;
		}

		core::bc_estring bc_file_system::get_content_platform_shader_path(const bcECHAR* p_shader_path) const noexcept
		{
			core::bc_path l_path(get_content_platform_shader_path());
			l_path.combine(core::bc_path(p_shader_path));

			return l_path.get_string();
		}

		core::bc_estring_view bc_file_system::get_content_script_path() const noexcept
		{
			return m_content_script_path;
		}

		core::bc_estring bc_file_system::get_content_script_path(const bcECHAR* p_script_path) const noexcept
		{
			core::bc_path l_path(get_content_script_path());
			l_path.combine(core::bc_path(p_script_path));

			return l_path.get_string();
		}

		core::bc_estring_view bc_file_system::get_content_scene_path() const noexcept
		{
			return m_content_scene_path;
		}

		core::bc_estring bc_file_system::get_content_scene_path(const bcECHAR* p_scene_path) const noexcept
		{
			core::bc_wstring_frame l_scene_name = p_scene_path;
			const core::bc_wstring_frame l_scene_extension(L".json");
			const bool l_end_with_json = core::bc_string_ends_with
			(
				l_scene_name.data(),
				l_scene_name.length(),
				l_scene_extension.data(),
				l_scene_extension.length()
			);
			if (!l_end_with_json)
			{
				l_scene_name += l_scene_extension;
			}
			
			core::bc_path l_path(get_content_scene_path());
			l_path.combine(core::bc_path(l_scene_name.data()));

			return l_path.get_string();
		}

		void bc_file_system::update(const platform::bc_clock::update_param& p_clock)
		{
			m_global_config_last_check_time += p_clock.m_elapsed;
			if(m_global_config_last_check_time <= 1000)
			{
				return;
			}

			m_global_config_last_check_time = 0;
			
			platform::bc_basic_file_info l_global_config_file_info;
			platform::bc_file_info::get_basic_info(m_global_config->get_path().data(), &l_global_config_file_info);

			const bool l_global_config_has_changed = l_global_config_file_info.m_last_write_time.m_total_milliseconds > m_global_config_last_write_time;
			if(l_global_config_has_changed)
			{
				m_global_config->reload();

				bc_event_global_config_changed l_event(*m_global_config);
				core::bc_get_service<core::bc_event_manager>()->process_event(l_event);

				m_global_config_last_write_time = l_global_config_file_info.m_last_write_time.m_total_milliseconds;
			}
		}
	}
}