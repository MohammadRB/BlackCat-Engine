// [12/22/2018 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_mediate_component::bc_mediate_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index)
		{
		}

		bc_actor bc_mediate_component::get_actor() const noexcept
		{
			return get_manager()->component_get_actor(*this);
		}

		core::bc_vector3f bc_mediate_component::get_world_position() const
		{
			auto l_actor = get_actor();

			auto* l_rigid_body_component = l_actor.get_component<bc_rigid_body_component>();
						
			if(l_rigid_body_component)
			{
				auto l_position = l_rigid_body_component->get_body().get_global_pose().get_position();

				auto* l_height_map_component = l_actor.get_component<bc_height_map_component>();
				if (l_height_map_component) // TODO
				{
					const auto& l_height_map = l_height_map_component->get_height_map();
					const auto l_half_width = (l_height_map.get_width() * l_height_map.get_xz_multiplier()) / 2;
					const auto l_half_height = (l_height_map.get_height() * l_height_map.get_xz_multiplier()) / 2;

					l_position.x += l_half_width;
					l_position.z -= l_half_height;
				}

				return l_position;
			}

			auto* l_mesh_component = l_actor.get_component<bc_mesh_component>();
			if (l_mesh_component)
			{
				return l_mesh_component->get_world_position();
			}

			auto* l_light_component = l_actor.get_component<bc_light_component>();
			if(l_light_component)
			{
				return l_light_component->get_world_position();
			}

			bcAssert(false);
			return core::bc_vector3f();
		}

		void bc_mediate_component::set_world_position(const core::bc_vector3f& p_position)
		{
			core::bc_matrix4f l_transform;
			l_transform.translate(p_position.x, p_position.y, p_position.z);

			set_world_transform(l_transform);
		}

		void bc_mediate_component::set_world_transform(const core::bc_matrix4f& p_transform)
		{
			auto l_actor = get_actor();
			auto* l_rigid_body_component = l_actor.get_component< bc_rigid_body_component >();
			auto* l_mesh_component = l_actor.get_component< bc_mesh_component >();
			auto* l_height_map_component = l_actor.get_component< bc_height_map_component >();
			auto* l_light_component = l_actor.get_component< bc_light_component >();

			if (l_rigid_body_component)
			{
				physics::bc_transform l_transform;

				if(l_height_map_component) // TODO
				{
					const auto& l_height_map = l_height_map_component->get_height_map();
					const auto l_half_width = (l_height_map.get_width() * l_height_map.get_xz_multiplier()) / 2;
					const auto l_half_height = (l_height_map.get_height() * l_height_map.get_xz_multiplier()) / 2;
					const auto l_position = p_transform.get_translation() + core::bc_vector3f(-l_half_width, 0, l_half_height);

					core::bc_matrix4f l_new_transform = p_transform;
					l_new_transform.set_translation(l_position);

					l_transform = physics::bc_transform(l_new_transform);
				}
				else
				{
					l_transform = physics::bc_transform(p_transform);
				}

				auto l_rigid_body = l_rigid_body_component->get_body();
				l_rigid_body.set_global_pose(l_transform);
			}

			if (l_mesh_component)
			{
				l_mesh_component->set_world_transform(*this, p_transform);
			}

			if(l_height_map_component)
			{
				l_height_map_component->set_world_transform(p_transform);
			}

			if(l_light_component)
			{
				l_light_component->set_world_transform(p_transform);
			}
		}

		void bc_mediate_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
		}

		void bc_mediate_component::update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}

		void bc_mediate_component::write_instance(bc_actor& p_actor, core::bc_json_key_value& p_parameters)
		{
			p_parameters.add(std::make_pair(core::bc_string(s_position_json_key), core::bc_any(get_world_position())));
		}
	}
}