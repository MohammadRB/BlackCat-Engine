// [05/22/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Container/bcString.h"
#include "Graphic/bcRenderApi.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_file_system : public core_platform::bc_no_copy
		{
		public:
			bc_file_system();

			bc_file_system(bc_file_system&&) noexcept;

			~bc_file_system();

			bc_file_system& operator=(bc_file_system&&) noexcept;

			const bcECHAR* get_content_base_path() const;

			const bcECHAR* get_content_data_path() const;

			core::bc_estring get_content_data_path(const bcECHAR* p_data_path) const;

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
			core::bc_estring m_content_model_path;
			core::bc_estring m_content_platform_shader_path;
			core::bc_estring m_content_script_path;
		};
	}
}