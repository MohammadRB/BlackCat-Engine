// [04/04/2016 MRB]

#pragma once

#include "Core/File/bcContentLoader.h"
#include "Graphic/bcDeviceResourceContent.h"
#include "GraphicImp/Device/bcDevice.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	template<>
	inline const bcCHAR* graphic::bc_device_resource_content< graphic::bc_vertex_shader >::content_name()
	{
		return core::g_cnt_vertex_shader;
	}

	class BC_BLACKCAT_DLL_EXP bc_vertex_shader_loader : public core::bc_base_content_loader
	{
	public:
		explicit bc_vertex_shader_loader();

		bc_vertex_shader_loader(bc_vertex_shader_loader&&);

		~bc_vertex_shader_loader();

		bc_vertex_shader_loader& operator=(bc_vertex_shader_loader&&);

		void content_offline_processing(core::bc_content_loader_context& p_context) override;

		void content_processing(core::bc_content_loader_context& p_context) override;

	protected:

	private:
	};
}
