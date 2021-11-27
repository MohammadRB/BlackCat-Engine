// [11/13/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcStack.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "PhysicsImp/Joint/bcSphericalJoint.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcHumanRagdollComponent.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidControllerComponent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/bcException.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_human_ragdoll_component::bc_human_ragdoll_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept
			: bci_actor_component(p_actor_index, p_index),
			m_physics_system(nullptr),
			m_scene(nullptr),
			m_mesh_component(nullptr),
			m_rigid_controller_component(nullptr)
		{
		}

		bc_human_ragdoll_component::bc_human_ragdoll_component(bc_human_ragdoll_component&&) noexcept = default;

		bc_human_ragdoll_component::~bc_human_ragdoll_component() = default;

		bc_human_ragdoll_component& bc_human_ragdoll_component::operator=(bc_human_ragdoll_component&&) noexcept = default;

		bc_actor bc_human_ragdoll_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_human_ragdoll_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
		}

		void bc_human_ragdoll_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
			m_physics_system = &p_context.m_game_system.get_physics_system();
			m_mesh_component = get_actor().get_component<bc_skinned_mesh_component>();
			m_rigid_controller_component = get_actor().get_component<bc_rigid_controller_component>();

			if(!m_mesh_component || !m_rigid_controller_component)
			{
				throw bc_invalid_argument_exception("ragdoll component needs skinned_mesh and rigid_controller components");
			}

			const auto& l_joints_value = p_context.m_parameters.get_value_throw<core::bc_json_key_value>(constant::g_param_ragdoll_joints);
			_fill_joints_map(l_joints_value);
			_fill_colliders_map(p_context.m_actor);
			
			m_ragdoll_animation_job = core::bc_make_unique<bc_ragdoll_animation_job>(bc_ragdoll_animation_job
			(
				*m_mesh_component->get_skeleton(),
				p_context.m_actor,
				*m_mesh_component,
				m_mesh_component->get_world_transforms()
			));
		}
		
		void bc_human_ragdoll_component::update(const bc_actor_component_update_content& p_context)
		{
			if(m_joints.empty())
			{
				return;
			}

			auto& l_model_transforms = m_mesh_component->get_model_transforms();
			auto& l_collider_model_transforms = m_mesh_component->get_collider_model_transforms();
			const auto l_mesh_scale_transform = core::bc_matrix4f::scale_matrix(m_mesh_component->get_mesh().get_mesh_scale());
			const auto& l_world_transform = p_context.m_actor.get_component<bc_mediate_component>()->get_world_transform();
			const auto l_inv_world_transform = physics::bc_transform(l_world_transform).get_inverse();
			
			{
				physics::bc_scene_shared_lock l_lock(m_scene);

				for (auto& [l_joint_name, l_joint_actor] : m_joint_actors)
				{
					const auto l_colliders_ite = std::find_if
					(
						std::begin(m_colliders_map),
						std::end(m_colliders_map),
						[&](const _bc_ragdoll_collider_entry& p_entry)
						{
							return p_entry.m_attached_node_name == l_joint_name;
						}
					);

					BC_ASSERT(l_colliders_ite != std::end(m_colliders_map));

					for (auto l_affected_nodes : l_colliders_ite->m_affected_node_indices)
					{
						l_collider_model_transforms[l_affected_nodes] = l_inv_world_transform * l_joint_actor->get_global_pose();
						l_model_transforms[l_affected_nodes] = l_mesh_scale_transform * l_collider_model_transforms[l_affected_nodes].get_matrix4();
					}
				}
			}

			m_ragdoll_animation_job->set_world(l_world_transform);
			m_mesh_component->add_animation_job(m_ragdoll_animation_job.get());
		}

		void bc_human_ragdoll_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_scene_add_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if(l_scene_add_event)
			{
				m_scene = &l_scene_add_event->get_scene().get_px_scene();
				return;
			}

			const auto* l_scene_remove_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event);
			if(l_scene_remove_event)
			{
				{
					physics::bc_scene_lock l_lock(m_scene);

					_destroy_physics_joints();
				}
				
				m_scene = nullptr;
				return;
			}
		}

		void bc_human_ragdoll_component::set_enable(bool p_enable)
		{
			{
				physics::bc_scene_lock l_lock(m_scene);

				if (p_enable)
				{
					_create_physics_joints();
				}
				else
				{
					_destroy_physics_joints();
				}
			}
		}

		void bc_human_ragdoll_component::_fill_colliders_map(const bc_actor& p_actor)
		{
			const auto& l_mesh = m_mesh_component->get_mesh();
			const auto& l_mesh_collider = l_mesh.get_mesh_collider();

			core::bc_stack_frame<_bc_ragdoll_collider_entry> l_colliders_stack;
			m_colliders_map.reserve(l_mesh_collider.size());

			bool l_dummy;
			l_mesh.iterate_over_nodes(l_dummy, [&](const bc_mesh_node& p_node, bool)
			{
				if(l_colliders_stack.empty())
				{
					l_colliders_stack.push(_bc_ragdoll_collider_entry());
					l_colliders_stack.top().m_attached_node_index = bc_mesh_node::s_invalid_index;
				}
				
				auto* l_last_collider = &l_colliders_stack.top();
				const auto l_node_colliders = l_mesh_collider.find_mesh_collider(p_node.get_name());

				if (!l_node_colliders.empty())
				{
					if (l_last_collider->m_attached_node_index != bc_mesh_node::s_invalid_index)
					{
						l_colliders_stack.push(_bc_ragdoll_collider_entry());
						l_last_collider = &l_colliders_stack.top();
					}

					l_last_collider->m_attached_node_index = p_node.get_index();
					l_last_collider->m_attached_node_name = p_node.get_name();
				}

				l_last_collider->m_affected_node_indices.push_back(p_node.get_index());

				return false;
			}, 
			[&](const bc_mesh_node& p_node, const bool)
			{
				if(l_colliders_stack.empty())
				{
					return;
				}
				
				if(p_node.get_index() == l_colliders_stack.top().m_attached_node_index)
				{
					m_colliders_map.push_back(std::move(l_colliders_stack.top()));
					l_colliders_stack.pop();
				}
			});
		}

		void bc_human_ragdoll_component::_fill_joints_map(const core::bc_json_key_value& p_joint_parameters)
		{
			m_joint_actors[s_head_index].first = p_joint_parameters.find("head")->second.as_throw<core::bc_string>();
			m_joint_actors[s_body_index].first = p_joint_parameters.find("body")->second.as_throw<core::bc_string>();
			m_joint_actors[s_left_arm_index].first = p_joint_parameters.find("left_arm")->second.as_throw<core::bc_string>();
			m_joint_actors[s_left_fore_arm_index].first = p_joint_parameters.find("left_fore_arm")->second.as_throw<core::bc_string>();
			m_joint_actors[s_left_hand_index].first = p_joint_parameters.find("left_hand")->second.as_throw<core::bc_string>();
			m_joint_actors[s_right_arm_index].first = p_joint_parameters.find("right_arm")->second.as_throw<core::bc_string>();
			m_joint_actors[s_right_fore_arm_index].first = p_joint_parameters.find("right_fore_arm")->second.as_throw<core::bc_string>();
			m_joint_actors[s_right_hand_index].first = p_joint_parameters.find("right_hand")->second.as_throw<core::bc_string>();
			m_joint_actors[s_left_up_leg_index].first = p_joint_parameters.find("left_up_leg")->second.as_throw<core::bc_string>();
			m_joint_actors[s_left_leg_index].first = p_joint_parameters.find("left_leg")->second.as_throw<core::bc_string>();
			m_joint_actors[s_left_foot_index].first = p_joint_parameters.find("left_foot")->second.as_throw<core::bc_string>();
			m_joint_actors[s_right_up_leg_index].first = p_joint_parameters.find("right_up_leg")->second.as_throw<core::bc_string>();
			m_joint_actors[s_right_leg_index].first = p_joint_parameters.find("right_leg")->second.as_throw<core::bc_string>();
			m_joint_actors[s_right_foot_index].first = p_joint_parameters.find("right_foot")->second.as_throw<core::bc_string>();
		}

		void bc_human_ragdoll_component::_create_physics_joints()
		{
			const auto& l_mesh = m_mesh_component->get_mesh();
			const auto& l_mesh_colliders = l_mesh.get_mesh_collider();

			auto& l_physics = m_physics_system->get_physics();
			auto l_rigid_body = m_rigid_controller_component->get_dynamic_body();
			auto l_rigid_body_global_transform = l_rigid_body.get_global_pose();
			
			core::bc_vector_frame<physics::bc_shape> l_rigid_body_shapes(l_rigid_body.get_shape_count());
			l_rigid_body.get_shapes(l_rigid_body_shapes.data(), l_rigid_body_shapes.size());

			for(auto& l_rigid_body_shape : l_rigid_body_shapes)
			{
				const auto l_shape_global_transform = l_rigid_body_global_transform * l_rigid_body_shape.get_local_pose();
				auto l_px_joint_actor = l_physics.create_rigid_dynamic(l_shape_global_transform);
				auto l_px_joint_shape = bc_copy_shape(l_physics, l_rigid_body_shape);
				
				l_px_joint_actor->attach_shape(l_px_joint_shape.get());

				auto l_joint_ite = std::find_if
				(
					std::begin(m_joint_actors),
					std::end(m_joint_actors),
					[&, this](const auto& p_entry)
					{
						auto* l_shape_data = m_physics_system->get_game_shape_data(l_rigid_body_shape);
						return p_entry.first == l_shape_data->m_collider_entry->m_attached_mesh_name;
					}
				);

				m_scene->add_actor(l_px_joint_actor.get());
				l_joint_ite->second = std::move(l_px_joint_actor);
			}

			m_joints.reserve(l_mesh_colliders.size());

			auto l_head_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_head_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_body_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_left_arm_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_left_arm_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_body_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_left_fore_arm_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_left_fore_arm_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_left_arm_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_left_hand_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_left_hand_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_left_fore_arm_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_right_arm_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_right_arm_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_body_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_right_fore_arm_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_right_fore_arm_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_right_arm_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_right_hand_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_right_hand_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_right_fore_arm_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_left_up_leg_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_left_up_leg_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_body_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_left_leg_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_left_leg_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_left_up_leg_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_left_foot_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_left_foot_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_left_leg_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_right_up_leg_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_right_up_leg_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_body_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_right_leg_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_right_leg_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_right_up_leg_index].second.get(),
				physics::bc_transform::identity()
			);
			auto l_right_foot_joint = l_physics.create_spherical_joint
			(
				&m_joint_actors[s_right_foot_index].second.get(),
				physics::bc_transform::identity(),
				&m_joint_actors[s_right_leg_index].second.get(),
				physics::bc_transform::identity()
			);
			
			m_joints.push_back(physics::bc_joint_ref(l_head_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_left_arm_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_left_fore_arm_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_left_hand_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_right_arm_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_right_fore_arm_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_right_hand_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_left_up_leg_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_left_leg_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_left_foot_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_right_up_leg_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_right_leg_joint.release()));
			m_joints.push_back(physics::bc_joint_ref(l_right_foot_joint.release()));

			for(auto& l_joint : m_joints)
			{
				l_joint->set_visualization(true);
			}
		}

		void bc_human_ragdoll_component::_destroy_physics_joints()
		{
			if(m_joints.empty())
			{
				return;
			}
			
			m_joints.clear();

			for (auto& [l_joint_name, l_joint_actor] : m_joint_actors)
			{
				m_scene->remove_actor(*l_joint_actor);
				l_joint_actor = physics::bc_rigid_dynamic_ref();
			}
		}
	}
}