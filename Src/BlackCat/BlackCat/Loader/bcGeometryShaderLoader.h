// [04/04/2016 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "GraphicImp/Device/bcDevice.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_geometry_shader_loader : public core::bc_base_content_loader
	{
	public:
		explicit bc_geometry_shader_loader();

		bc_geometry_shader_loader(bc_geometry_shader_loader&&) noexcept;

		~bc_geometry_shader_loader();

		bc_geometry_shader_loader& operator=(bc_geometry_shader_loader&&) noexcept;

		bool support_offline_processing() const override;

		void content_offline_processing(core::bc_content_loading_context& p_context) const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;
		
	protected:

	private:
	};
}