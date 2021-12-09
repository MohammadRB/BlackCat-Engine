// [11/13/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcStack.h"
#include "Core/Math/bcCoordinate.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Container/bcStringStream.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "PhysicsImp/Joint/bcSphericalJoint.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcHumanRagdollComponent.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
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
			m_px_scene(nullptr),
			m_mesh_component(nullptr),
			m_rigid_body_component(nullptr)
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
			m_rigid_body_component = get_actor().get_component<bc_rigid_body_component>();

			if (!m_mesh_component || !m_rigid_body_component)
			{
				throw bc_invalid_argument_exception("ragdoll component needs skinned_mesh and rigid_controller components");
			}

			const auto& l_joints_value = p_context.m_parameters.get_value_throw<core::bc_json_key_value>(constant::g_param_ragdoll_joints);
			_fill_joints_map(l_joints_value);
			_validate_mesh_colliders();

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
			if (m_joints.empty())
			{
				return;
			}

			{
				physics::bc_scene_shared_lock l_lock(m_px_scene);

				const auto l_any_active = std::any_of
				(
					std::begin(m_joint_actors),
					std::end(m_joint_actors),
					[](const auto& p_entry)
					{
						const physics::bc_rigid_dynamic& l_rigid_dynamic = p_entry.second.get();
						return !l_rigid_dynamic.is_sleeping();
					}
				);
				if (!l_any_active)
				{
					return;
				}
			}

			auto& l_model_transforms = m_mesh_component->get_model_transforms();
			auto& l_collider_model_transforms = m_mesh_component->get_collider_model_transforms();
			const auto l_mesh_scale_transform = core::bc_matrix4f::scale_matrix(m_mesh_component->get_mesh().get_mesh_scale());
			core::bc_matrix4f l_world_transform;

			{
				physics::bc_scene_shared_lock l_lock(m_px_scene);

				const auto l_body_transform = m_joint_actors[s_body_index].second->get_global_pose();
				const auto l_body_rotation = l_body_transform.as_rotation();
				l_world_transform = core::bc_matrix4f::translation_matrix(l_body_transform.get_position() + l_body_rotation.transform(m_body_to_origin_vector));
				const auto l_inv_world_transform = physics::bc_transform(l_world_transform).get_inverse();

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

					auto& l_collider_model_transform = l_collider_model_transforms[l_colliders_ite->m_attached_node_index];
					l_collider_model_transform = l_inv_world_transform * l_joint_actor->get_global_pose();

					for (const auto& l_affected_node : l_colliders_ite->m_affected_nodes)
					{
						auto l_affected_node_transform = l_collider_model_transform * l_affected_node.m_collider_inv_bind_pose_rotation;
						l_affected_node_transform = l_affected_node_transform * l_affected_node.m_collider_offset_transform;

						l_model_transforms[l_affected_node.m_node_index] = l_mesh_scale_transform * l_affected_node_transform.get_matrix4();
					}
				}
			}

			p_context.m_actor.add_event(bc_world_transform_actor_event(l_world_transform));
			m_ragdoll_animation_job->set_world(l_world_transform);
			m_mesh_component->add_animation_job(m_ragdoll_animation_job.get());
		}

		void bc_human_ragdoll_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_scene_add_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if (l_scene_add_event)
			{
				m_px_scene = &l_scene_add_event->get_scene().get_px_scene();
				return;
			}

			const auto* l_scene_remove_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event);
			if (l_scene_remove_event)
			{
				set_enable(false);
				m_px_scene = nullptr;
				return;
			}
		}

		void bc_human_ragdoll_component::set_enable(bool p_enable)
		{
			{
				physics::bc_scene_lock l_lock(m_px_scene);

				if (p_enable)
				{
					_fill_colliders_map();
					_create_physics_joints();
				}
				else
				{
					_destroy_physics_joints();
				}
			}
		}

		void bc_human_ragdoll_component::add_force(bc_ragdoll_part p_part, const core::bc_vector3f& p_force)
		{
			BC_ASSERT(p_part < m_joint_actors.size());

			auto l_actor = m_joint_actors[p_part].second.get();
			if (!l_actor.is_valid())
			{
				return;
			}

			{
				physics::bc_scene_lock l_lock(m_px_scene);

				l_actor.add_force(p_force);
			}
		}

		void bc_human_ragdoll_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
			/*if (m_joints.empty())
			{
				return;
			}
			
			{
				physics::bc_scene_shared_lock l_lock(m_px_scene);

				const auto l_body_transform = m_joint_actors[s_body_index].second->get_global_pose();
				const auto l_body_position = l_body_transform.get_position();
				const auto l_world_position = l_body_position + l_body_transform.as_rotation().transform(m_body_to_origin_vector);

				core::bc_vector3f l_points[2] = { l_body_position, l_world_position };
				bcUINT32 l_indices[2] = {0, 1};
				
				p_context.m_shape_drawer.draw_point_list(core::bc_span<core::bc_vector3f>(&l_points[0], 2), core::bc_span<bcUINT32>(&l_indices[0], 2));
			}*/
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

		void bc_human_ragdoll_component::_validate_mesh_colliders()
		{
			bool l_is_valid = true;

			auto l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_head_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_body_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_left_arm_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_left_fore_arm_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_left_hand_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_right_arm_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_right_fore_arm_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_right_hand_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_left_up_leg_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_left_leg_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_left_foot_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_right_up_leg_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_right_leg_index].first);
			l_is_valid &= l_colliders.size() == 1;
			l_colliders = m_mesh_component->get_mesh().get_mesh_collider().find_mesh_collider(m_joint_actors[s_right_foot_index].first);
			l_is_valid &= l_colliders.size() == 1;

			if (!l_is_valid)
			{
				throw bc_invalid_operation_exception("nodes with more than one collider is not supported in ragdoll");
			}
		}

		void bc_human_ragdoll_component::_fill_colliders_map()
		{
			if (!m_colliders_map.empty())
			{
				return;
			}

			const auto& l_mesh = m_mesh_component->get_mesh();
			const auto& l_mesh_collider = l_mesh.get_mesh_collider();

			core::bc_stack_frame<_bc_ragdoll_collider_entry> l_colliders_stack;
			m_colliders_map.reserve(l_mesh_collider.get_colliders().size());

			bool l_dummy;
			l_mesh.iterate_over_nodes(l_dummy, [&](const bc_mesh_node& p_node, bool)
			{
				if (l_colliders_stack.empty())
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

				_bc_ragdoll_collider_entry::affected_node l_affected_node;
				l_affected_node.m_node_index = p_node.get_index();
				l_affected_node.m_collider_offset_transform = physics::bc_transform::identity();

				l_last_collider->m_affected_nodes.push_back(l_affected_node);

				return false;
			},
			[&](const bc_mesh_node& p_node, const bool)
			{
				if (l_colliders_stack.empty())
				{
					return;
				}

				if (p_node.get_index() == l_colliders_stack.top().m_attached_node_index)
				{
					m_colliders_map.push_back(std::move(l_colliders_stack.top()));
					l_colliders_stack.pop();
				}
			});

			for (auto& l_entry : m_colliders_map)
			{
				auto l_collider = l_mesh_collider.find_mesh_collider(l_entry.m_attached_node_name);

				// Ignore collider rotation in bind pose transform
				const auto l_collider_inv_bind_pose_transform = l_collider.front().m_absolute_transform.as_position().get_inverse();
				const auto l_collider_inv_bind_pose_rotation = l_collider.front().m_absolute_transform.as_rotation().get_inverse();

				for (auto& l_affected_node : l_entry.m_affected_nodes)
				{
					const auto& l_affected_mesh_node = *l_mesh.find_node(l_affected_node.m_node_index);
					auto l_affected_node_bind_pose = l_mesh.get_node_bind_pose_transform(l_affected_mesh_node);
					l_affected_node_bind_pose.set_translation(l_affected_node_bind_pose.get_translation() * l_mesh.get_mesh_scale()); // TODO mesh scale
					const auto l_affected_node_bind_pose_transform = physics::bc_transform(l_affected_node_bind_pose);

					l_affected_node.m_collider_offset_transform = l_collider_inv_bind_pose_transform * l_affected_node_bind_pose_transform;
					l_affected_node.m_collider_inv_bind_pose_rotation = l_collider_inv_bind_pose_rotation;
				}
			}
		}

		void bc_human_ragdoll_component::_create_physics_joints()
		{
			auto& l_physics = m_physics_system->get_physics();
			const auto& l_mesh = m_mesh_component->get_mesh();
			const auto& l_mesh_colliders = l_mesh.get_mesh_collider();

			const auto l_rigid_body = m_rigid_body_component->get_body();
			const auto l_rigid_body_global_transform = l_rigid_body.get_global_pose();

			core::bc_vector_frame<physics::bc_shape> l_rigid_body_shapes(l_rigid_body.get_shape_count());
			l_rigid_body.get_shapes(l_rigid_body_shapes.data(), l_rigid_body_shapes.size());

			for (auto& l_rigid_body_shape : l_rigid_body_shapes)
			{
				const auto l_shape_global_transform = l_rigid_body_global_transform * l_rigid_body_shape.get_local_pose();
				auto l_px_joint_actor = l_physics.create_rigid_dynamic(l_shape_global_transform);
				auto l_px_joint_shape = bc_copy_shape(l_physics, l_rigid_body_shape);

				l_px_joint_actor->attach_shape(l_px_joint_shape.get());
				m_physics_system->set_game_actor(l_px_joint_actor.get(), bc_actor());

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

				m_px_scene->add_actor(l_px_joint_actor.get());
				l_joint_ite->second = std::move(l_px_joint_actor);
			}

			m_joints.reserve(l_mesh_colliders.get_collider_joints().size());

			auto l_create_spherical_joint = [&](std::string_view p_collider1, physics::bc_rigid_actor& p_actor1, std::string_view p_collider2, physics::bc_rigid_actor& p_actor2)
			{
				const auto* l_colliders_joint = l_mesh_colliders.find_joint(p_collider1, p_collider2);
				if (!l_colliders_joint)
				{
					const auto l_message = core::bc_estring_stream_frame() << bcL("cannot find joint for colliders ") << p_collider1 << bcL(" and ") << p_collider2;
					core::bc_log(core::bc_log_type::error, l_message.str().c_str());
					return physics::bc_spherical_joint_ref();
				}

				const auto l_collider1_colliders = l_mesh_colliders.find_mesh_collider(p_collider1);
				const auto l_collider2_colliders = l_mesh_colliders.find_mesh_collider(p_collider2);

				const auto& l_collider1_inv_bind_post = l_collider1_colliders.front().m_local_transform.get_inverse();
				const auto& l_collider2_inv_bind_post = l_collider2_colliders.front().m_local_transform.get_inverse();
				const auto l_collider1_local_frame = physics::bc_transform(l_collider1_inv_bind_post) * *l_colliders_joint;
				const auto l_collider2_local_frame = physics::bc_transform(l_collider2_inv_bind_post) * *l_colliders_joint;

				auto l_joint = l_physics.create_spherical_joint
				(
					&p_actor1,
					l_collider1_local_frame,
					&p_actor2,
					l_collider2_local_frame
				);
				l_joint->set_visualization(true);
				l_joint->enable_limit(physics::bc_joint_cone_limit(core::bc_to_radian(90), core::bc_to_radian(90)));

				return l_joint;
			};

			auto l_head_joint = l_create_spherical_joint
			(
				m_joint_actors[s_body_index].first,
				m_joint_actors[s_body_index].second.get(),
				m_joint_actors[s_head_index].first,
				m_joint_actors[s_head_index].second.get()
			);
			auto l_left_arm_joint = l_create_spherical_joint
			(
				m_joint_actors[s_body_index].first,
				m_joint_actors[s_body_index].second.get(),
				m_joint_actors[s_left_arm_index].first,
				m_joint_actors[s_left_arm_index].second.get()
			);
			auto l_left_fore_arm_joint = l_create_spherical_joint
			(
				m_joint_actors[s_left_arm_index].first,
				m_joint_actors[s_left_arm_index].second.get(),
				m_joint_actors[s_left_fore_arm_index].first,
				m_joint_actors[s_left_fore_arm_index].second.get()
			);
			auto l_left_hand_joint = l_create_spherical_joint
			(
				m_joint_actors[s_left_fore_arm_index].first,
				m_joint_actors[s_left_fore_arm_index].second.get(),
				m_joint_actors[s_left_hand_index].first,
				m_joint_actors[s_left_hand_index].second.get()
			);
			auto l_right_arm_joint = l_create_spherical_joint
			(
				m_joint_actors[s_body_index].first,
				m_joint_actors[s_body_index].second.get(),
				m_joint_actors[s_right_arm_index].first,
				m_joint_actors[s_right_arm_index].second.get()
			);
			auto l_right_fore_arm_joint = l_create_spherical_joint
			(
				m_joint_actors[s_right_arm_index].first,
				m_joint_actors[s_right_arm_index].second.get(),
				m_joint_actors[s_right_fore_arm_index].first,
				m_joint_actors[s_right_fore_arm_index].second.get()
			);
			auto l_right_hand_joint = l_create_spherical_joint
			(
				m_joint_actors[s_right_fore_arm_index].first,
				m_joint_actors[s_right_fore_arm_index].second.get(),
				m_joint_actors[s_right_hand_index].first,
				m_joint_actors[s_right_hand_index].second.get()
			);
			auto l_left_up_leg_joint = l_create_spherical_joint
			(
				m_joint_actors[s_body_index].first,
				m_joint_actors[s_body_index].second.get(),
				m_joint_actors[s_left_up_leg_index].first,
				m_joint_actors[s_left_up_leg_index].second.get()
			);
			auto l_left_leg_joint = l_create_spherical_joint
			(
				m_joint_actors[s_left_up_leg_index].first,
				m_joint_actors[s_left_up_leg_index].second.get(),
				m_joint_actors[s_left_leg_index].first,
				m_joint_actors[s_left_leg_index].second.get()
			);
			auto l_left_foot_joint = l_create_spherical_joint
			(
				m_joint_actors[s_left_leg_index].first,
				m_joint_actors[s_left_leg_index].second.get(),
				m_joint_actors[s_left_foot_index].first,
				m_joint_actors[s_left_foot_index].second.get()
			);
			auto l_right_up_leg_joint = l_create_spherical_joint
			(
				m_joint_actors[s_body_index].first,
				m_joint_actors[s_body_index].second.get(),
				m_joint_actors[s_right_up_leg_index].first,
				m_joint_actors[s_right_up_leg_index].second.get()
			);
			auto l_right_leg_joint = l_create_spherical_joint
			(
				m_joint_actors[s_right_up_leg_index].first,
				m_joint_actors[s_right_up_leg_index].second.get(),
				m_joint_actors[s_right_leg_index].first,
				m_joint_actors[s_right_leg_index].second.get()
			);
			auto l_right_foot_joint = l_create_spherical_joint
			(
				m_joint_actors[s_right_leg_index].first,
				m_joint_actors[s_right_leg_index].second.get(),
				m_joint_actors[s_right_foot_index].first,
				m_joint_actors[s_right_foot_index].second.get()
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

			// Capture the vector which transform body part origin into local origin
			const auto l_body_transform = m_joint_actors[s_body_index].second->get_global_pose();
			m_body_to_origin_vector = get_actor().get_component<bc_mediate_component>()->get_position() - l_body_transform.get_position();
			m_body_to_origin_vector = l_body_transform.as_rotation().get_inverse().transform(m_body_to_origin_vector);
		}

		void bc_human_ragdoll_component::_destroy_physics_joints()
		{
			if (m_joints.empty())
			{
				return;
			}

			m_joints.clear();

			for (auto& [l_joint_name, l_joint_actor] : m_joint_actors)
			{
				m_px_scene->remove_actor(*l_joint_actor);
				l_joint_actor.reset();
			}
		}
	}
}