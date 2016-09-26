// [05/04/2016 MRB]

#pragma once

#include "Core/File/bcContent.h"
#include "Core/File/bcContentLoader.h"
#include "Core/Container/bcString.h"
#include "GraphicImp/Device/bcDevice.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL_EXP bc_mesh_loader : public core::bc_base_content_loader
	{
	public:
		bc_mesh_loader();

		bc_mesh_loader(bc_mesh_loader&&) noexcept;

		~bc_mesh_loader();

		bc_mesh_loader& operator=(bc_mesh_loader&&) noexcept;

		void content_offline_processing(core::bc_content_loader_context& p_context) override;

		void content_processing(core::bc_content_loader_context& p_context) override;

	protected:

	private:
	};

	using bc_mesh_ptr = core::bc_content_ptr< game::bc_mesh >;
}