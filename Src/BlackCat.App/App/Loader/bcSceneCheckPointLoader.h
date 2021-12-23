// [04/23/2021 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_scene_check_point_loader : public core::bc_base_content_loader
	{
	public:
		bool support_offline_processing() const override;
		
		void content_processing(core::bc_content_loading_context& p_context) const override;
		
		void content_processing(core::bc_content_saving_context& p_context) const override;
	};
}