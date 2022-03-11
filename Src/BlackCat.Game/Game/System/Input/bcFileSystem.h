// [05/22/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Container/bcString.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_global_config;
		
		class BC_GAME_DLL bc_file_system : public platform::bc_no_copy
		{
		public:
			bc_file_system(core::bc_content_manager& p_content_manager, core::bc_content_stream_manager& p_content_stream_manager);

			bc_file_system(bc_file_system&&) noexcept;

			~bc_file_system();

			bc_file_system& operator=(bc_file_system&&) noexcept;

			core::bc_content_manager& get_content_manager();

			const core::bc_content_manager& get_content_manager() const;

			core::bc_content_stream_manager& get_content_stream_manager();

			const core::bc_content_stream_manager& get_content_stream_manager() const;

			const bcECHAR* get_content_path() const noexcept;

			core::bc_estring get_content_path(const bcECHAR* p_path) const noexcept;

			const bcECHAR* get_content_data_path() const noexcept;

			core::bc_estring get_content_data_path(const bcECHAR* p_data_path) const noexcept;

			const bcECHAR* get_content_texture_path() const noexcept;

			core::bc_estring get_content_texture_path(const bcECHAR* p_texture_path) const noexcept;

			const bcECHAR* get_content_model_path() const noexcept;

			core::bc_estring get_content_model_path(const bcECHAR* p_model_path) const noexcept;

			const bcECHAR* get_content_platform_shader_path() const noexcept;

			core::bc_estring get_content_platform_shader_path(const bcECHAR* p_shader_path) const noexcept;

			const bcECHAR* get_content_script_path() const noexcept;

			core::bc_estring get_content_script_path(const bcECHAR* p_script_path) const noexcept;

			const bcECHAR* get_content_scene_path() const noexcept;

			core::bc_estring get_content_scene_path(const bcECHAR* p_scene_path) const noexcept;

			bc_global_config& get_global_config() noexcept;

			const bc_global_config& get_global_config() const noexcept;

			void update(const platform::bc_clock::update_param& p_clock);
			
		private:
			core::bc_content_manager* m_content_manager;
			core::bc_content_stream_manager* m_content_stream_manager;

			core::bc_estring m_execute_path;
			core::bc_estring m_content_base_path;
			core::bc_estring m_content_data_path;
			core::bc_estring m_content_texture_path;
			core::bc_estring m_content_model_path;
			core::bc_estring m_content_platform_shader_path;
			core::bc_estring m_content_script_path;
			core::bc_estring m_content_scene_path;

			core::bc_unique_ptr<bc_global_config> m_global_config;
			bcUINT64 m_global_config_last_write_time;
			bcUINT32 m_global_config_last_check_time;
		};

		inline bc_global_config& bc_file_system::get_global_config() noexcept
		{
			return *m_global_config;
		}

		inline const bc_global_config& bc_file_system::get_global_config() const noexcept
		{
			return *m_global_config;
		}
	}
}