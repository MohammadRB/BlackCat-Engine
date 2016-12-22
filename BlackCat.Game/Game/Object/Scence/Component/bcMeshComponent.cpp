// [06/06/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/bcConstant.h"
#include "Core/File/bcLazyContent.h"
#include "Game/Object/Scence/Component/bcMeshComponent.h"
#include "Game/System/Render/bcRenderInstance.h"

namespace black_cat
{
	namespace game
	{
		template< typename TIterator >
		void _render_mesh_node(const bc_render_component& p_render_component,
			const bc_mesh_part& p_mesh_part,
			const bc_mesh_part_transformation& p_transformations,
			TIterator p_begin,
			TIterator p_end)
		{
			bc_render_instance l_instance;

			for (; p_begin != p_end; ++p_begin)
			{
				const bc_mesh_node* l_node = *p_begin;
				
				for(bcUINT32 l_mesh_index= 0, l_mesh_count = l_node->get_mesh_count(); l_mesh_index < l_mesh_count; ++l_mesh_index)
				{
					auto* l_node_mesh_render_state = p_mesh_part.get_node_mesh_render_state(l_node, l_mesh_index);

					if (l_node_mesh_render_state)
					{
						auto* l_node_transformation = p_mesh_part.get_node_absolute_transformation(p_transformations, l_node);

						l_instance.set_world(*l_node_transformation);

						p_render_component.render(l_node_mesh_render_state, l_instance);
					}
				}

				_render_mesh_node
				(
					p_render_component,
					p_mesh_part,
					p_transformations,
					std::begin(p_mesh_part.get_node_childs(l_node)),
					std::end(p_mesh_part.get_node_childs(l_node))
				);
			}
		}

		bc_mesh_component::bc_mesh_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index),
			m_mesh_part()
		{
		}

		bc_mesh_component::bc_mesh_component(bc_mesh_component&& p_other) noexcept
			: bc_iactor_component(std::move(p_other)),
			m_mesh_part(std::move(p_other.m_mesh_part)),
			m_mesh_part_transformation(std::move(p_other.m_mesh_part_transformation))
		{
		}

		bc_mesh_component::~bc_mesh_component()
		{
		}

		bc_mesh_component& bc_mesh_component::operator=(bc_mesh_component&& p_other) noexcept
		{
			bc_iactor_component::operator=(std::move(p_other));
			m_mesh_part = std::move(p_other.m_mesh_part);
			m_mesh_part_transformation = std::move(m_mesh_part_transformation);

			return *this;
		}

		void bc_mesh_component::initialize(const core::bc_data_driven_parameter& p_parameters)
		{
			bc_mesh_ptr l_mesh = p_parameters.get_value_throw< core::bc_lazy_content >(core::g_param_mesh).get_content< bc_mesh >();
			core::bc_string* l_mesh_part_name = p_parameters.get_value< core::bc_string >(core::g_param_mesh_part);

			m_mesh_part = l_mesh_part_name ? bc_mesh_part(l_mesh, l_mesh_part_name->c_str()) : bc_mesh_part(l_mesh);
			m_mesh_part_transformation = m_mesh_part.calculate_absolute_transformations(core::bc_matrix4f::identity());
		}

		void bc_mesh_component::update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param)
		{
		}

		void bc_mesh_component::render(const bc_render_component& p_render_component) const
		{
			const bc_mesh_node* l_node = m_mesh_part.get_node();

			core::bc_array< const bc_mesh_node*, 1 > l_nodes = {l_node};
			_render_mesh_node(p_render_component, m_mesh_part, m_mesh_part_transformation, std::begin(l_nodes), std::end(l_nodes));
		}
	}
}
