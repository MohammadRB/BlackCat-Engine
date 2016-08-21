// [06/07/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scence/bcActor.hpp"
#include "Game/Object/Scence/Component/bcRenderComponent.h"
#include "Game/Object/Scence/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		template< typename TIterator >
		void _render_mesh_node(bc_render_system& p_render_system,
			const bc_mesh_part& p_mesh_part, 
			const bc_mesh_part_transformation& p_transformations,
			TIterator p_begin, 
			TIterator p_end)
		{
			bc_render_instance l_instance;

			for (; p_begin != p_end; ++p_begin)
			{
				const bc_mesh_node* l_node = *p_begin;

				auto& l_node_render_state = p_mesh_part.get_node_render_state(l_node);

				if(l_node_render_state)
				{
					auto* l_node_transformation = p_mesh_part.get_node_absolute_transformation(p_transformations, l_node);

					l_instance.set_world(*l_node_transformation);

					p_render_system.add_render_instance(l_node_render_state, l_instance);
				}

				_render_mesh_node
				(
					p_render_system,
					p_mesh_part,
					p_transformations,
					std::begin(p_mesh_part.get_node_childs(l_node)),
					std::end(p_mesh_part.get_node_childs(l_node))
				);
			}
		}

		bc_render_component::bc_render_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index),
			m_mesh_component(nullptr)
		{
		}

		bc_render_component::bc_render_component(bc_render_component&& p_other) noexcept
			: bc_iactor_component(std::move(p_other)),
			m_mesh_component(nullptr)
		{
		}

		bc_render_component::~bc_render_component()
		{	
		}

		bc_render_component& bc_render_component::operator=(bc_render_component&& p_other) noexcept
		{
			bc_iactor_component::operator=(std::move(p_other));

			return *this;
		}

		void bc_render_component::initialize(const core::bc_data_driven_parameter& p_parameters)
		{
		}

		void bc_render_component::update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param)
		{
			m_mesh_component = p_actor.get_component< bc_mesh_component >();
			bcAssert(m_mesh_component != nullptr);
		}

		void bc_render_component::render(bc_render_system& p_render_system)
		{
			const bc_mesh_part& l_mesh_part = m_mesh_component->get_mesh_part();
			const bc_mesh_part_transformation& l_mesh_part_transformation = m_mesh_component->get_mesh_part_transformation();
			const bc_mesh_node* l_node = l_mesh_part.get_node();

			core::bc_array<const bc_mesh_node*, 1> l_nodes = { l_node };
			_render_mesh_node(p_render_system, l_mesh_part, l_mesh_part_transformation, std::begin(l_nodes), std::end(l_nodes));
		}
	}
}