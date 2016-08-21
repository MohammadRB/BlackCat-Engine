// [06/06/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/bcConstant.h"
#include "Core/File/bcLazyContent.h"
#include "Game/Object/Scence/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_component::bc_mesh_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index),
			m_mesh_part()
		{
		}

		bc_mesh_component::bc_mesh_component(bc_mesh_component&& p_other) noexcept
			: bc_iactor_component(std::move(p_other)),
			m_mesh_part(std::move(p_other.m_mesh_part))
		{
		}

		bc_mesh_component::~bc_mesh_component()
		{
		}

		bc_mesh_component& bc_mesh_component::operator=(bc_mesh_component&& p_other) noexcept
		{
			bc_iactor_component::operator=(std::move(p_other));
			m_mesh_part = std::move(p_other.m_mesh_part);

			return *this;
		}

		void bc_mesh_component::initialize(const core::bc_data_driven_parameter& p_parameters)
		{
			bc_mesh_ptr l_mesh = p_parameters.get_value_throw<core::bc_lazy_content>(core::g_param_mesh).get_content< bc_mesh >();
			core::bc_string* l_mesh_part_name = p_parameters.get_value<core::bc_string>(core::g_param_mesh_part);

			m_mesh_part = l_mesh_part_name ? bc_mesh_part(l_mesh, l_mesh_part_name->c_str()) : bc_mesh_part(l_mesh);
			m_mesh_part_transformation = m_mesh_part.calculate_absolute_transformations(graphic::bc_matrix4f::identity());
		}

		void bc_mesh_component::update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param)
		{
		}
	}
}