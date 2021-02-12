// [03/07/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/Material/bcMeshMaterial.h"
#include "Game/System/Render/Material/bcMaterialManager.h"

namespace black_cat
{
	namespace game
	{
		_bc_mesh_material_ptr_deleter::_bc_mesh_material_ptr_deleter()
			: m_material_manager(nullptr)
		{
		}

		_bc_mesh_material_ptr_deleter::_bc_mesh_material_ptr_deleter(bc_material_manager* p_material_manager)
			: m_material_manager(p_material_manager)
		{
		}

		void _bc_mesh_material_ptr_deleter::operator()(bc_mesh_material* p_material) const
		{
			BC_ASSERT(m_material_manager != nullptr);

			m_material_manager->destroy_mesh_material(p_material);
		}
	}
}