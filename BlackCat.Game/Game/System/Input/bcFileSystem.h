// [05/22/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Container/bcString.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_file_system : public core_platform::bc_no_copy
		{
		public:
			bc_file_system();

			bc_file_system(bc_file_system&&);

			~bc_file_system();

			bc_file_system& operator=(bc_file_system&&);

			const bcECHAR* get_content_stream_path() const;

			const bcECHAR* get_entity_type_path() const;

			const bcECHAR* get_platform_shaders_path() const;

			core::bc_estring get_platform_shader_path(const bcECHAR* p_shader_path) const;

			const bcECHAR* get_content_base_path() const;

			const bcECHAR* get_content_models_path() const;

			core::bc_estring get_content_model_path(const bcECHAR* p_model_path) const;

		protected:

		private:
			core::bc_estring m_execute_path;
			core::bc_estring m_content_base_path;
			core::bc_estring m_content_stream_path;
			core::bc_estring m_platform_shader_path;
			core::bc_estring m_entity_type_path;
			core::bc_estring content_models_path;
		};
	}
}