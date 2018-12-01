// [06/06/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/bcConstant.h"
#include "Core/File/bcLazyContent.h"
#include "Game/Object/Scene/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/System/Render/bcRenderInstance.h"

namespace black_cat
{
	namespace game
	{
		template< typename TIterator >
		void _render_mesh_node(const bc_render_component& p_render_component,
			const bc_sub_mesh& p_mesh_part,
			const bc_sub_mesh_transformation& p_transformations,
			TIterator p_begin,
			TIterator p_end)
		{
			bc_render_instance l_instance;

			for (; p_begin != p_end; ++p_begin)
			{
				const bc_mesh_node* l_node = *p_begin;
				
				for(bc_mesh_node::node_indexing l_mesh_index= 0, l_mesh_count = l_node->get_mesh_count(); l_mesh_index < l_mesh_count; ++l_mesh_index)
				{
					auto* l_node_mesh_render_state = p_mesh_part.get_node_mesh_render_state(l_node, l_mesh_index);

					if (l_node_mesh_render_state)
					{
						auto* l_node_transformation = p_mesh_part.get_node_absolute_transformation(l_node, p_transformations);

						l_instance.set_world(*l_node_transformation);

						p_render_component.render(l_node_mesh_render_state, l_instance);
					}
				}

				_render_mesh_node
				(
					p_render_component,
					p_mesh_part,
					p_transformations,
					std::begin(p_mesh_part.get_node_children(l_node)),
					std::end(p_mesh_part.get_node_children(l_node))
				);
			}
		}

		bc_mesh_component::bc_mesh_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index),
			m_sub_mesh()
		{
		}

		bc_mesh_component::bc_mesh_component(bc_mesh_component&& p_other) noexcept
			: bc_iactor_component(std::move(p_other)),
			m_sub_mesh(std::move(p_other.m_sub_mesh)),
			m_mesh_part_transformation(std::move(p_other.m_mesh_part_transformation))
		{
		}

		bc_mesh_component::~bc_mesh_component()
		{
		}

		bc_mesh_component& bc_mesh_component::operator=(bc_mesh_component&& p_other) noexcept
		{
			m_sub_mesh = std::move(p_other.m_sub_mesh);
			m_mesh_part_transformation = std::move(p_other.m_mesh_part_transformation);
			bc_iactor_component::operator=(std::move(p_other));

			return *this;
		}

		bc_actor bc_mesh_component::get_actor() const noexcept
		{
			return get_manager()->component_get_actor(*this);
		}

		void bc_mesh_component::set_world_pos(const core::bc_matrix4f& p_pos)
		{
			m_sub_mesh.calculate_absolute_transformations(p_pos, m_mesh_part_transformation, m_bound_box);
		}

		void bc_mesh_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			const bc_mesh_ptr l_mesh = p_parameters.get_value_throw< core::bc_lazy_content >(core::g_param_mesh).get_content< bc_mesh >();
			core::bc_string* l_sub_mesh_name = p_parameters.get_value< core::bc_string >(core::g_param_sub_mesh);

			m_sub_mesh = l_sub_mesh_name ? bc_sub_mesh(l_mesh, l_sub_mesh_name->c_str()) : bc_sub_mesh(l_mesh);

			set_world_pos(core::bc_matrix4f::identity());
		}

		void bc_mesh_component::update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}

		void bc_mesh_component::render(const bc_render_component& p_render_component) const
		{
			const bc_mesh_node* l_node = m_sub_mesh.get_node();

			core::bc_array< const bc_mesh_node*, 1 > l_nodes = {l_node};
			_render_mesh_node(p_render_component, m_sub_mesh, m_mesh_part_transformation, std::begin(l_nodes), std::end(l_nodes));
		}
	}
}