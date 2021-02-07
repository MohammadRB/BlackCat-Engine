// [03/07/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcRenderMaterial.h"
#include "Game/System/Render/bcMaterialManager.h"

namespace black_cat
{
	namespace game
	{
		_bc_material_ptr_deleter::_bc_material_ptr_deleter()
			: m_material_manager(nullptr)
		{
		}

		_bc_material_ptr_deleter::_bc_material_ptr_deleter(bc_material_manager* p_material_manager)
			: m_material_manager(p_material_manager)
		{
		}

		void _bc_material_ptr_deleter::operator()(bc_render_material* p_material) const
		{
			BC_ASSERT(m_material_manager != nullptr);

			m_material_manager->destroy_material(p_material);
		}
	}
}