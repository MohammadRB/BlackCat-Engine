// [05/22/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/File/bcPath.h"
#include "Core/Container/bcString.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Game/System/Input/bcFileSystem.h"

namespace black_cat
{
	namespace game
	{
		bc_file_system::bc_file_system(core::bc_content_manager& p_content_manager, core::bc_content_stream_manager& p_content_stream_manager)
			: m_content_manager(&p_content_manager),
			m_content_stream_manager(&p_content_stream_manager)
		{
			core::bc_path l_execute_path(core::bc_path(core::bc_path::get_program_path().c_str()).get_directory().c_str());
			m_execute_path = l_execute_path.get_string();

			core::bc_path l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content")));
			m_content_base_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content\\Data")));
			m_content_data_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content\\Texture")));
			m_content_texture_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content\\Model")));
			m_content_model_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine(core::bc_path
			(
				(
					core::bc_estring_frame(bcL("Content\\Shader\\"))
					+
					core::bc_to_exclusive_wstring(graphic::bc_render_api_info::api_name()).c_str()
				).c_str()
			));
			m_content_platform_shader_path = l_temp.get_string();

			l_temp = l_execute_path;
			l_temp.combine(core::bc_path(bcL("Content\\Script")));
			m_content_script_path = l_temp.get_string();

			m_global_config = core::bc_make_unique<bc_config_file>(core::bc_alloc_type::program, m_content_base_path.c_str(), bcL("config.json"));
		}

		bc_file_system::bc_file_system(bc_file_system&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_file_system::~bc_file_system()
		{
		}

		bc_file_system& bc_file_system::operator=(bc_file_system&& p_other) noexcept
		{
			m_execute_path = std::move(p_other.m_execute_path);
			m_content_base_path = std::move(p_other.m_content_base_path);
			m_content_data_path = std::move(p_other.m_content_data_path);
			m_content_texture_path = std::move(p_other.m_content_texture_path);
			m_content_model_path = std::move(p_other.m_content_model_path);
			m_content_platform_shader_path = std::move(m_content_platform_shader_path);
			m_global_config = std::move(p_other.m_global_config);

			m_content_manager = p_other.m_content_manager;
			m_content_stream_manager = p_other.m_content_stream_manager;

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

		const bcECHAR* bc_file_system::get_content_path() const noexcept
		{
			return m_content_base_path.c_str();
		}

		core::bc_estring bc_file_system::get_content_path(const bcECHAR* p_path) const noexcept
		{
			core::bc_path l_path(get_content_path());
			l_path.combine(core::bc_path(p_path));

			return l_path.get_string();
		}

		const bcECHAR* bc_file_system::get_content_data_path() const noexcept
		{
			return m_content_data_path.c_str();
		}

		core::bc_estring bc_file_system::get_content_data_path(const bcECHAR* p_data_path) const noexcept
		{
			core::bc_path l_path(get_content_data_path());
			l_path.combine(core::bc_path(p_data_path));

			return l_path.get_string();
		}

		const bcECHAR* bc_file_system::get_content_texture_path() const noexcept
		{
			return m_content_texture_path.c_str();
		}

		core::bc_estring bc_file_system::get_content_texture_path(const bcECHAR* p_texture_path) const noexcept
		{
			core::bc_path l_path(get_content_texture_path());
			l_path.combine(core::bc_path(p_texture_path));

			return l_path.get_string();
		}

		const bcECHAR* bc_file_system::get_content_model_path() const noexcept
		{
			return m_content_model_path.c_str();
		}

		core::bc_estring bc_file_system::get_content_model_path(const bcECHAR* p_model_path) const noexcept
		{
			core::bc_path l_path(get_content_model_path());
			l_path.combine(core::bc_path(p_model_path));

			return l_path.get_string();
		}

		const bcECHAR* bc_file_system::get_content_platform_shader_path() const noexcept
		{
			return m_content_platform_shader_path.c_str();
		}

		core::bc_estring bc_file_system::get_content_platform_shader_path(const bcECHAR* p_shader_path) const noexcept
		{
			core::bc_path l_path(get_content_platform_shader_path());
			l_path.combine(core::bc_path(p_shader_path));

			return l_path.get_string();
		}

		const bcECHAR* bc_file_system::get_content_script_path() const noexcept
		{
			return m_content_script_path.c_str();
		}

		core::bc_estring bc_file_system::get_content_script_path(const bcECHAR* p_script_path) const noexcept
		{
			core::bc_path l_path(get_content_script_path());
			l_path.combine(core::bc_path(p_script_path));

			return l_path.get_string();
		}

		bc_config_file& bc_file_system::get_global_config() noexcept
		{
			return *m_global_config;
		}

		const bc_config_file& bc_file_system::get_global_config() const noexcept
		{
			return *m_global_config;
		}
	}
}