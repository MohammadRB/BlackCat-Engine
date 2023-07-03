// [11/03/2022 MRB]

#pragma once

#include "SoundImp/Resource/bcSound.h"
#include "Core/Content/bcContentLoader.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_sound_loader : public core::bc_base_content_loader
	{
	public:
		bool support_offline_processing() const override;

		void content_file_open_succeeded(core::bc_content_loading_context& p_context) const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;
	};
}