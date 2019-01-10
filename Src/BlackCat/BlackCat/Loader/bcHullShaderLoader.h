// [04/04/2016 MRB]

#pragma once

#include "Core/File/bcContentLoader.h"
#include "GraphicImp/Device/bcDevice.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL bc_hull_shader_loader : public core::bc_base_content_loader
	{
	public:
		explicit bc_hull_shader_loader();

		bc_hull_shader_loader(bc_hull_shader_loader&&) noexcept;

		~bc_hull_shader_loader();

		bc_hull_shader_loader& operator=(bc_hull_shader_loader&&) noexcept;

		void content_offline_processing(core::bc_content_loading_context& p_context) const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;

	protected:

	private:
	};
}