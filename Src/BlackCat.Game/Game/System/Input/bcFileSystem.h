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
		class BC_GAME_DLL bc_file_system : public core_platform::bc_no_copy
		{
		public:
			bc_file_system(core::bc_content_manager& p_content_manager, core::bc_content_stream_manager& p_content_stream_manager);

			bc_file_system(bc_file_system&&) noexcept;

			~bc_file_system();

			bc_file_system& operator=(bc_file_system&&) noexcept;

			core::bc_content_manager& get_content_manager()
			{
				return m_content_manager;
			}

			const core::bc_content_manager& get_content_manager() const
			{
				return m_content_manager;
			}

			core::bc_content_stream_manager& get_content_stream_manager()
			{
				return m_content_stream_manager;
			}

			const core::bc_content_stream_manager& get_content_stream_manager() const
			{
				return m_content_stream_manager;
			}

			const bcECHAR* get_content_path() const;

			core::bc_estring get_content_path(const bcECHAR* p_path) const;

			const bcECHAR* get_content_data_path() const;

			core::bc_estring get_content_data_path(const bcECHAR* p_data_path) const;

			const bcECHAR* get_content_texture_path() const;

			core::bc_estring get_content_texture_path(const bcECHAR* p_texture_path) const;

			const bcECHAR* get_content_model_path() const;

			core::bc_estring get_content_model_path(const bcECHAR* p_model_path) const;

			const bcECHAR* get_content_platform_shader_path() const;

			core::bc_estring get_content_platform_shader_path(const bcECHAR* p_shader_path) const;

			const bcECHAR* get_content_script_path() const;

			core::bc_estring get_content_script_path(const bcECHAR* p_script_path) const;

		protected:

		private:
			core::bc_estring m_execute_path;
			core::bc_estring m_content_base_path;
			core::bc_estring m_content_data_path;
			core::bc_estring m_content_texture_path;
			core::bc_estring m_content_model_path;
			core::bc_estring m_content_platform_shader_path;
			core::bc_estring m_content_script_path;

			core::bc_content_manager& m_content_manager;
			core::bc_content_stream_manager& m_content_stream_manager;
		};
	}
}