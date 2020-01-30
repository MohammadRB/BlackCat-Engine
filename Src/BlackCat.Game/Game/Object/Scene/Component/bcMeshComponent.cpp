// [06/06/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcLazyContent.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcConstant.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventBoundBoxChanged.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"

namespace black_cat
{
	namespace game
	{
		void _render_mesh_node(bc_render_system& p_render_system,
			const bc_sub_mesh& p_mesh_part,
			const bc_sub_mesh_transformation& p_transformations,
			const bc_mesh_node* p_begin,
			const bc_mesh_node* p_end,
			const bcCHAR* p_mesh_prefix)
		{
			for (; p_begin != p_end; ++p_begin)
			{
				const bc_mesh_node* l_node = p_begin;

				for (bc_mesh_node::node_index l_mesh_index = 0, l_mesh_count = l_node->get_mesh_count(); l_mesh_index < l_mesh_count; ++l_mesh_index)
				{
					if (p_mesh_prefix != nullptr)
					{
						const auto& l_mesh_name = p_mesh_part.get_node_mesh_name(l_node, l_mesh_index);
						const bool l_starts_with_prefix = l_mesh_name.compare(0, std::strlen(p_mesh_prefix), p_mesh_prefix) == 0;

						if (!l_starts_with_prefix)
						{
							continue;
						}
					}

					auto& l_node_mesh_render_state = p_mesh_part.get_node_mesh_render_state(l_node, l_mesh_index);
					auto& l_node_transformation = p_mesh_part.get_node_absolute_transformation(l_node, p_transformations);

					bc_render_instance l_instance(l_node_transformation);
					p_render_system.add_render_instance(l_node_mesh_render_state, l_instance);
				}

				const auto& l_node_children = p_mesh_part.get_node_children(l_node);

				if (!l_node_children.empty())
				{
					_render_mesh_node
					(
						p_render_system,
						p_mesh_part,
						p_transformations,
						*std::begin(l_node_children),
						*std::begin(l_node_children) + l_node_children.size(),
						p_mesh_prefix
					);
				}
			}
		}

		bc_mesh_component::bc_mesh_component(bc_actor_component_index p_index)
			: bc_render_component(p_index),
			m_sub_mesh(),
			m_mesh_part_transformation()
		{
		}

		bc_mesh_component::bc_mesh_component(bc_mesh_component&& p_other) noexcept
			: bc_render_component(std::move(p_other)),
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
			bc_render_component::operator=(std::move(p_other));

			return *this;
		}
		
		core::bc_vector3f bc_mesh_component::get_world_position() const
		{
			return m_mesh_part_transformation.get_node_translation(*m_sub_mesh.get_root_node());
		}

		core::bc_matrix4f bc_mesh_component::get_world_transform() const
		{
			return m_mesh_part_transformation.get_node_transform(*m_sub_mesh.get_root_node());
		}

		void bc_mesh_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			auto& l_mesh_name = p_parameters.get_value_throw< core::bc_string >(constant::g_param_mesh);
			auto* l_sub_mesh_name = p_parameters.get_value< core::bc_string >(constant::g_param_sub_mesh);
			const bc_mesh_ptr l_mesh = core::bc_lazy_content::get_content<bc_mesh>(l_mesh_name.c_str());

			m_sub_mesh = l_sub_mesh_name ? bc_sub_mesh(l_mesh, l_sub_mesh_name->c_str()) : bc_sub_mesh(l_mesh);
			m_mesh_part_transformation = bc_sub_mesh_transformation(*m_sub_mesh.get_root_node());
		}

		void bc_mesh_component::render(bc_render_system& p_render_system) const
		{
			const bc_mesh_node* l_node = m_sub_mesh.get_root_node();
			
			_render_mesh_node(p_render_system, m_sub_mesh, m_mesh_part_transformation, l_node, l_node + 1, nullptr);
		}

		void bc_mesh_component::set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform)
		{
			physics::bc_bound_box l_bound_box;
			m_sub_mesh.calculate_absolute_transformations(p_transform, m_mesh_part_transformation, l_bound_box);

			p_actor.add_event(bc_actor_event_bound_box_changed(l_bound_box));
		}
	}
}