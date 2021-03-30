// [01/04/2021 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Math/bcCoordinate.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcJsonParse.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/ActorController/bcXBotController.h"
#include "Game/Object/Scene/ActorController/bcXBotUpdateAnimationJob.h"
#include "Game/Object/Scene/ActorController/bcXBotWeaponIKAnimationJob.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/Component/bcWeaponComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Animation/bcAnimationSkeleton.h"
#include "Game/Object/Animation/bcSkeletonAnimation.h"
#include "Game/Object/Animation/bcAnimationJobBuilder.h"
#include "Game/Object/Animation/bcAnimationJobHelper.h"
#include "Game/Object/Animation/Job/bcBlendingAnimationJob.h"
#include "Game/Object/Animation/Job/bcAimAnimationJob.h"
#include "Game/Object/Animation/Job/bcPartialBlendingAnimationJob.h"
#include "Game/Object/Animation/Job/bcActorUpdateAnimationJob.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/bcGameSystem.h"

namespace black_cat
{
	namespace game
	{
		bc_xbot_controller::bc_xbot_controller() noexcept
			: m_physics_system(nullptr),
			m_scene(nullptr),
			m_skinned_component(nullptr),
			m_active_job(nullptr),
			m_look_speed(0),
			m_run_speed(0),
			m_walk_speed(0),
			m_move_speed(0),
			m_look_delta_x(0),
			m_forward_pressed(false),
			m_backward_pressed(false),
			m_right_pressed(false),
			m_left_pressed(false),
			m_walk_pressed(false),
			m_look_velocity(0, 1, 0.25),
			m_forward_velocity(0, 1, 0.25),
			m_backward_velocity(m_forward_velocity),
			m_right_velocity(m_forward_velocity),
			m_left_velocity(m_forward_velocity),
			m_walk_velocity(m_forward_velocity),
			m_position(),
			m_look_direction(),
			m_move_direction(),
			m_move_amount(0)
		{
		}

		void bc_xbot_controller::initialize(const bc_actor_component_initialize_context& p_context)
		{
			m_physics_system = &p_context.m_game_system.get_physics_system();
			m_actor = p_context.m_actor;
			m_skinned_component = p_context.m_actor.get_component<bc_skinned_mesh_component>();

			if (!m_skinned_component)
			{
				throw bc_invalid_operation_exception("xbot actor must have skinned components");
			}

			const auto& l_upper_body_chain_value = p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>("upper_body_chain");
			const auto& l_main_hand_chain_value = p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>("main_hand_chain");
			const auto& l_second_hand_chain_value = p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>("second_hand_chain");
			const auto& l_rifle_joint_value = p_context.m_parameters.get_value_throw<core::bc_string>("rifle_joint");
			const auto& l_rifle_joint_offset_value = p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>("rifle_joint_offset");

			std::transform
			(
				std::begin(l_upper_body_chain_value),
				std::end(l_upper_body_chain_value),
				std::back_inserter(m_upper_body_chain),
				[](const core::bc_any& p_any)
				{
					return p_any.as_throw<core::bc_string>();
				}
			);
			std::transform
			(
				std::begin(l_main_hand_chain_value),
				std::end(l_main_hand_chain_value),
				std::begin(m_main_hand_chain),
				[this](const core::bc_any& p_any)
				{
					const auto l_joint = m_skinned_component->get_skeleton()->find_joint_by_name(p_any.as_throw<core::bc_string>().c_str());
					if(!l_joint.second)
					{
						throw bc_invalid_argument_exception("Right hand and/or left hand were not found");
					}
					
					return l_joint;
				}
			);
			std::transform
			(
				std::begin(l_second_hand_chain_value),
				std::end(l_second_hand_chain_value),
				std::begin(m_second_hand_chain),
				[this](const core::bc_any& p_any)
				{
					const auto l_joint = m_skinned_component->get_skeleton()->find_joint_by_name(p_any.as_throw<core::bc_string>().c_str());
					if (!l_joint.second)
					{
						throw bc_invalid_argument_exception("Right hand and/or left hand were not found");
					}
					
					return l_joint;
				}
			);
			m_rifle_joint = m_skinned_component->get_skeleton()->find_joint_by_name(l_rifle_joint_value.c_str());
			json_parse::bc_load(l_rifle_joint_offset_value, m_rifle_joint_offset);
			m_local_origin = core::bc_vector3f(0);
			m_local_forward = core::bc_vector3f(0, 0, -1);
			m_look_direction = m_local_forward;
			m_move_direction = m_local_forward;

			m_idle_job = _create_idle_animation
			(
				"NeutralIdle", 
				"LeftTurn", 
				"RightTurn",
				m_idle_sample_job
			);
			m_running_job = _create_running_animation
			(
				m_idle_sample_job,
				"Walking",
				"WalkingBackward",
				"Running",
				"RunningBackward",
				m_walking_sample_job,
				m_walking_backward_sample_job,
				m_running_sample_job,
				m_running_backward_sample_job
			);
			m_rifle_idle_job = _create_idle_animation
			(
				"RifleAimingIdle",
				"RifleLeftTurn",
				"RifleRightTurn",
				m_rifle_aiming_idle_sample_job
			);
			m_rifle_running_job = _create_running_animation
			(
				m_rifle_aiming_idle_sample_job,
				"RifleWalking",
				"RifleWalkingBackward",
				"RifleRunning",
				"RifleRunningBackward",
				m_walking_sample_job,
				m_walking_backward_sample_job,
				m_running_sample_job,
				m_running_backward_sample_job
			);
		}

		void bc_xbot_controller::added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene)
		{
			m_scene = &p_scene;
			
			const auto* l_mediate_component = p_context.m_actor.get_component<bc_mediate_component>();
			const auto l_bound_box_extends = l_mediate_component->get_bound_box().get_half_extends();
			const auto l_max_side_length = std::max({ l_bound_box_extends.x, l_bound_box_extends.y, l_bound_box_extends.z }) * 2;
			const auto l_px_material = core::bc_get_service<bc_game_system>()->get_render_system().get_material_manager().get_default_collider_material().m_px_material;
			const auto l_px_controller_desc = physics::bc_ccontroller_desc
			                                  (
				                                  core::bc_vector3f(m_position),
				                                  l_max_side_length * 0.8f,
				                                  l_max_side_length * 0.2f,
				                                  l_px_material
			                                  )
											  .with_contact_offset(l_max_side_length * 0.2f)
			                                  .with_step_offset(l_max_side_length * 0.f)
			                                  .with_hit_callback(this);
			
			{
				physics::bc_scene_lock l_lock(&m_scene->get_px_scene());
				
				m_px_controller = m_scene->get_px_scene().create_ccontroller(l_px_controller_desc);
				m_px_controller->set_foot_position(m_position - m_local_origin);

				physics::bc_shape l_px_controller_shape;
				m_px_controller->get_actor().get_shapes(&l_px_controller_shape, 1);
				l_px_controller_shape.set_flag(physics::bc_shape_flag::query, false);
			}
			
			m_look_speed = 10.0f;
			m_run_speed = l_max_side_length * 1.3f;
			m_walk_speed = l_max_side_length * 0.6f;
			
			m_active_job = m_idle_job.get();
		}

		void bc_xbot_controller::update(const bc_actor_component_update_content& p_context)
		{
			if (!m_scene) // Has not added to scene yet
			{
				return;
			}
			
			_update_input(p_context.m_clock);
			_update_direction(p_context.m_clock);
			_update_active_animation(p_context.m_clock);
			_update_world_transform(p_context.m_clock);

			if(m_active_job)
			{
				m_skinned_component->add_animation_job(*m_active_job);
			}
		}

		void bc_xbot_controller::removed_from_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene)
		{
			{
				physics::bc_scene_lock l_lock(&m_scene->get_px_scene());
				m_px_controller.reset();
			}
			
			m_scene = nullptr;
			m_active_job = nullptr;
		}

		void bc_xbot_controller::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if(l_world_transform_event && !l_world_transform_event->is_px_simulation_transform())
			{
				const auto& l_world_transform = l_world_transform_event->get_transform();
				m_position = l_world_transform.get_translation() - m_local_origin;

				// In case we have transform event before actor added to scene
				if(m_px_controller->is_valid())
				{
					{
						physics::bc_scene_lock l_lock(&m_scene->get_px_scene());
						m_px_controller->set_foot_position(m_position - m_local_origin);
					}
				}
			}
		}

		void bc_xbot_controller::set_look_delta(bcINT32 p_x_change) noexcept
		{
			m_look_delta_x = p_x_change;
			core_platform::atomic_thread_fence(core_platform::bc_memory_order::release);
		}

		void bc_xbot_controller::set_move_forward(bool p_value) noexcept
		{
			m_forward_pressed = p_value;
			core_platform::atomic_thread_fence(core_platform::bc_memory_order::release);
		}

		void bc_xbot_controller::set_move_backward(bool p_value) noexcept
		{
			m_backward_pressed = p_value;
			core_platform::atomic_thread_fence(core_platform::bc_memory_order::release);
		}

		void bc_xbot_controller::set_move_right(bool p_value) noexcept
		{
			m_right_pressed = p_value;
			core_platform::atomic_thread_fence(core_platform::bc_memory_order::release);
		}

		void bc_xbot_controller::set_move_left(bool p_value) noexcept
		{
			m_left_pressed = p_value;
			core_platform::atomic_thread_fence(core_platform::bc_memory_order::release);
		}

		void bc_xbot_controller::set_walk(bool p_value) noexcept
		{
			m_walk_pressed = p_value;
			core_platform::atomic_thread_fence(core_platform::bc_memory_order::release);
		}

		void bc_xbot_controller::attach_weapon(const bc_actor& p_weapon) noexcept
		{
			const auto* l_weapon_component = p_weapon.get_component<bc_weapon_component>();
			const auto* l_mesh_component = p_weapon.get_component<bc_mesh_component>();
			if(!l_weapon_component)
			{
				throw bc_invalid_argument_exception("Passed actor has not weapon component");
			}
			if (!l_mesh_component)
			{
				throw bc_invalid_argument_exception("Passed actor has not mesh component");
			}

			const auto& l_weapon_mesh = l_mesh_component->get_mesh();
			const auto* l_main_hand_node = l_weapon_mesh.find_node(l_weapon_component->get_main_hand_node_name());
			const auto* l_second_hand_node = l_weapon_mesh.find_node(l_weapon_component->get_second_hand_node_name());

			physics::bc_bound_box l_weapon_bound_box;
			bc_sub_mesh_mat4_transform l_weapon_mesh_transforms(*l_weapon_mesh.get_root_node());
			l_weapon_mesh.calculate_absolute_transforms
			(
				core::bc_matrix4f::identity(),
				l_weapon_mesh_transforms,
				l_weapon_bound_box
			);
			
			bc_xbot_weapon l_weapon;
			l_weapon.m_actor = p_weapon;
			l_weapon.m_class = l_weapon_component->get_class();
			l_weapon.m_main_hand_node = l_main_hand_node;
			l_weapon.m_second_hand_node = l_second_hand_node;
			l_weapon.m_main_hand_offset = l_weapon_mesh_transforms.get_node_transform(*l_main_hand_node).get_translation() + l_weapon_component->get_main_hand_local_offset();
			l_weapon.m_second_hand_offset = l_weapon_mesh_transforms.get_node_transform(*l_second_hand_node).get_translation() + l_weapon_component->get_second_hand_local_offset();
			l_weapon.m_local_forward = l_weapon_component->get_local_forward();

			m_weapon.reset(l_weapon);

			auto* l_rifle_idle_ik_job = bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<bc_sequence_animation_job&>(*m_rifle_idle_job));
			auto* l_rifle_running_ik_job = bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<bc_sequence_animation_job&>(*m_rifle_running_job));

			l_rifle_idle_ik_job->set_offset_joint(m_rifle_joint.second);
			l_rifle_idle_ik_job->set_offset(m_rifle_joint_offset);
			l_rifle_running_ik_job->set_offset_joint(m_rifle_joint.second);
			l_rifle_running_ik_job->set_offset(m_rifle_joint_offset);
		}

		void bc_xbot_controller::detach_weapon() noexcept
		{
			m_weapon.reset();

			/*bc_animation_job_helper::find_job<bc_partial_blending_animation_job>(static_cast<bc_sequence_animation_job&>(*m_idle_job))->set_enabled(false);
			bc_animation_job_helper::find_job<bc_partial_blending_animation_job>(static_cast<bc_sequence_animation_job&>(*m_running_job))->set_enabled(false);*/
		}

		void bc_xbot_controller::on_shape_hit(const physics::bc_ccontroller_shape_hit& p_hit)
		{
		}

		void bc_xbot_controller::on_ccontroller_hit(const physics::bc_ccontroller_controller_hit& p_hit)
		{
		}

		core::bc_shared_ptr<bci_animation_job> bc_xbot_controller::_create_idle_animation(const bcCHAR* p_idle_animation,
			const bcCHAR* p_left_turn_animation,
			const bcCHAR* p_right_turn_animation,
			core::bc_shared_ptr<bc_sampling_animation_job>& p_idle_sample_job)
		{
			auto& l_skeleton = *m_skinned_component->get_skeleton();
			auto& l_idle_animation = *m_skinned_component->find_animation(p_idle_animation);
			auto& l_left_turn_animation = *m_skinned_component->find_animation(p_left_turn_animation);
			auto& l_right_turn_animation = *m_skinned_component->find_animation(p_right_turn_animation);
			core::bc_vector<const bcCHAR*> l_aim_bone_chain(m_upper_body_chain.size());

			std::transform
			(
				std::begin(m_upper_body_chain),
				std::end(m_upper_body_chain),
				std::begin(l_aim_bone_chain),
				[](const core::bc_string& p_bone_name)
				{
					return p_bone_name.c_str();
				}
			);
			
			p_idle_sample_job = core::bc_make_shared<bc_sampling_animation_job>(bc_sampling_animation_job(l_skeleton, l_idle_animation));
			auto l_left_turn_sample_job = core::bc_make_shared<bc_sampling_animation_job>(bc_sampling_animation_job(l_skeleton, l_left_turn_animation));
			auto l_right_turn_sample_job = core::bc_make_shared<bc_sampling_animation_job>(bc_sampling_animation_job(l_skeleton, l_right_turn_animation));

			auto l_idle_blending_job = core::bc_make_shared<bc_blending_animation_job>
			(
				bc_blending_animation_job(l_skeleton, { std::move(l_left_turn_sample_job), p_idle_sample_job, std::move(l_right_turn_sample_job) })
			);
			auto l_local_to_model_job = core::bc_make_shared<bc_local_to_model_animation_job>
			(
				bc_local_to_model_animation_job(l_idle_blending_job, m_skinned_component->get_mesh(), m_skinned_component->get_model_transforms())
			);
			auto l_model_to_skinning_job = core::bc_make_shared<bc_model_to_skinned_animation_job>
			(
				bc_model_to_skinned_animation_job(l_local_to_model_job, m_skinned_component->get_world_transforms())
			);
			auto l_aim_job = core::bc_make_shared<bc_aim_animation_job>
			(
				bc_aim_animation_job(l_idle_blending_job, l_local_to_model_job, l_model_to_skinning_job, core::bc_make_span(l_aim_bone_chain), m_local_forward)
			);
			auto l_weapon_ik_job = core::bc_make_shared<bc_xbot_weapon_ik_animation_job>
			(
				bc_xbot_weapon_ik_animation_job
				(
					m_local_forward,
					l_idle_blending_job, 
					l_local_to_model_job,
					m_main_hand_chain[0].second,
					m_main_hand_chain[1].second,
					m_main_hand_chain[2].second,
					m_second_hand_chain[0].second,
					m_second_hand_chain[1].second,
					m_second_hand_chain[2].second,
					m_rifle_joint.second,
					m_rifle_joint_offset
				)
			);
			auto l_actor_update_job = core::bc_make_shared<bc_xbot_update_animation_job>
			(
				bc_xbot_update_animation_job(m_actor, *this, *m_skinned_component, l_model_to_skinning_job)
			);
			
			bcFLOAT l_blending_weights[] = { 0,1,0 };
			l_idle_blending_job->set_weights(&l_blending_weights[0]);
			//l_aim_job->set_enabled(false);

			return bc_animation_job_builder()
				.start_with(std::move(l_idle_blending_job))
				.then(std::move(l_local_to_model_job))
				.then(std::move(l_aim_job))
				.then(std::move(l_weapon_ik_job))
				.then(std::move(l_model_to_skinning_job))
				.end_with(std::move(l_actor_update_job))
				.build();
		}

		core::bc_shared_ptr<bci_animation_job> bc_xbot_controller::_create_running_animation(core::bc_shared_ptr<bc_sampling_animation_job> p_idle_sample_job,
			const bcCHAR* p_walking_animation,
			const bcCHAR* p_walking_backward_animation,
			const bcCHAR* p_running_animation,
			const bcCHAR* p_running_backward_animation,
			core::bc_shared_ptr<bc_sampling_animation_job>& p_walking_sample_job,
			core::bc_shared_ptr<bc_sampling_animation_job>& p_walking_backward_sample_job,
			core::bc_shared_ptr<bc_sampling_animation_job>& p_running_sample_job,
			core::bc_shared_ptr<bc_sampling_animation_job>& p_running_backward_sample_job)
		{
			auto& l_skeleton = *m_skinned_component->get_skeleton();
			auto& l_walking_animation = *m_skinned_component->find_animation(p_walking_animation);
			auto& l_walking_backward_animation = *m_skinned_component->find_animation(p_walking_backward_animation);
			auto& l_running_animation = *m_skinned_component->find_animation(p_running_animation);
			auto& l_running_backward_animation = *m_skinned_component->find_animation(p_running_backward_animation);
			core::bc_vector<const bcCHAR*> l_aim_bone_chain(m_upper_body_chain.size());

			std::transform
			(
				std::begin(m_upper_body_chain),
				std::end(m_upper_body_chain),
				std::begin(l_aim_bone_chain),
				[](const core::bc_string& p_bone_name)
				{
					return p_bone_name.c_str();
				}
			);

			p_walking_sample_job = core::bc_make_shared<bc_sampling_animation_job>(bc_sampling_animation_job(l_skeleton, l_walking_animation));
			p_walking_backward_sample_job = core::bc_make_shared<bc_sampling_animation_job>(bc_sampling_animation_job(l_skeleton, l_walking_backward_animation));
			p_running_sample_job = core::bc_make_shared<bc_sampling_animation_job>(bc_sampling_animation_job(l_skeleton, l_running_animation));
			p_running_backward_sample_job = core::bc_make_shared<bc_sampling_animation_job>(bc_sampling_animation_job(l_skeleton, l_running_backward_animation));
			
			auto l_running_blending_job = core::bc_make_shared<bc_blending_animation_job>
			(
				bc_blending_animation_job
				(
					l_skeleton,
					{
						std::move(p_idle_sample_job),
						p_walking_sample_job,
						p_walking_backward_sample_job,
						p_running_sample_job,
						p_running_backward_sample_job
					}
				)
			);
			auto l_local_to_model_job = core::bc_make_shared<bc_local_to_model_animation_job>
			(
				bc_local_to_model_animation_job(l_running_blending_job, m_skinned_component->get_mesh(), m_skinned_component->get_model_transforms())
			);
			auto l_model_to_skinning_job = core::bc_make_shared<bc_model_to_skinned_animation_job>
			(
				bc_model_to_skinned_animation_job(l_local_to_model_job, m_skinned_component->get_world_transforms())
			);
			auto l_aim_job = core::bc_make_shared<bc_aim_animation_job>
			(
				bc_aim_animation_job(l_running_blending_job, l_local_to_model_job, l_model_to_skinning_job, core::bc_make_span(l_aim_bone_chain), m_local_forward)
			);
			auto l_weapon_ik_job = core::bc_make_shared<bc_xbot_weapon_ik_animation_job>
			(
				bc_xbot_weapon_ik_animation_job
				(
					m_local_forward,
					l_running_blending_job,
					l_local_to_model_job,
					m_main_hand_chain[0].second,
					m_main_hand_chain[1].second,
					m_main_hand_chain[2].second,
					m_second_hand_chain[0].second,
					m_second_hand_chain[1].second,
					m_second_hand_chain[2].second,
					m_rifle_joint.second,
					m_rifle_joint_offset
				)
			);
			auto l_actor_update_job = core::bc_make_shared<bc_xbot_update_animation_job>
			(
				bc_xbot_update_animation_job(m_actor, *this, *m_skinned_component, l_model_to_skinning_job)
			);

			//l_aim_job->set_enabled(false);
			
			return bc_animation_job_builder()
				.start_with(std::move(l_running_blending_job))
				.then(std::move(l_local_to_model_job))
				.then(std::move(l_aim_job))
				.then(std::move(l_weapon_ik_job))
				.then(std::move(l_model_to_skinning_job))
				.end_with(std::move(l_actor_update_job))
				.build();
		}

		void bc_xbot_controller::_update_input(const core_platform::bc_clock::update_param& p_clock)
		{
			core_platform::atomic_thread_fence(core_platform::bc_memory_order::acquire);

			if (m_look_delta_x != 0)
			{
				m_look_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_look_velocity.release(p_clock.m_elapsed_second);
			}

			if (m_forward_pressed)
			{
				m_forward_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_forward_velocity.release(p_clock.m_elapsed_second);
			}

			if (m_right_pressed)
			{
				m_right_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_right_velocity.release(p_clock.m_elapsed_second);
			}

			if (m_left_pressed)
			{
				m_left_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_left_velocity.release(p_clock.m_elapsed_second);
			}

			const bool l_is_moving_forward = m_forward_velocity.get_value() || m_right_velocity.get_value() || m_left_velocity.get_value();
			const bool l_is_moving_backward = m_backward_velocity.get_value();

			if (m_backward_pressed && (!l_is_moving_forward || l_is_moving_backward))
			{
				m_backward_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_backward_velocity.release(p_clock.m_elapsed_second);
			}

			if (m_walk_pressed)
			{
				m_walk_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_walk_velocity.release(p_clock.m_elapsed_second);
			}
		}

		void bc_xbot_controller::_update_direction(const core_platform::bc_clock::update_param& p_clock)
		{
			const auto l_look_velocity = m_look_velocity.get_value();
			const auto l_forward_velocity = m_forward_velocity.get_value();
			const auto l_backward_velocity = m_backward_velocity.get_value();
			const auto l_right_velocity = m_right_velocity.get_value();
			const auto l_left_velocity = m_left_velocity.get_value();
			const auto l_walk_velocity = m_walk_velocity.get_value();

			/*if(l_forward_velocity > 0)
			{
				core::bc_log(core::bc_log_type::debug, core::bc_to_estring_frame(l_forward_velocity).c_str());
			}*/
			
			core::bc_matrix3f l_look_rotation;
			core::bc_vector3f l_right_vector;
			core::bc_vector3f l_left_vector;

			if (graphic::bc_render_api_info::use_left_handed())
			{
				l_look_rotation.rotation_y_lh(core::bc_to_radian(m_look_delta_x * l_look_velocity * m_look_speed * p_clock.m_elapsed_second));
			}
			else
			{
				l_look_rotation.rotation_y_rh(core::bc_to_radian(m_look_delta_x * l_look_velocity * m_look_speed * p_clock.m_elapsed_second));
			}

			m_look_direction = core::bc_vector3f::normalize(l_look_rotation * m_look_direction);

			if (graphic::bc_render_api_info::use_left_handed())
			{
				core::bc_matrix3f l_right_left_direction;

				l_right_left_direction.rotation_y_lh(core::bc_to_radian(l_right_velocity * 90));
				l_right_vector = core::bc_vector3f::normalize(l_right_left_direction * m_look_direction);

				l_right_left_direction.rotation_y_lh(core::bc_to_radian(l_left_velocity * -90));
				l_left_vector = core::bc_vector3f::normalize(l_right_left_direction * m_look_direction);
			}
			else
			{
				core::bc_matrix3f l_right_left_direction;

				l_right_left_direction.rotation_y_rh(core::bc_to_radian(l_right_velocity * 90));
				l_right_vector = core::bc_vector3f::normalize(l_right_left_direction * m_look_direction);

				l_right_left_direction.rotation_y_rh(core::bc_to_radian(l_left_velocity * -90));
				l_left_vector = core::bc_vector3f::normalize(l_right_left_direction * m_look_direction);
			}

			m_move_speed = (1 - l_walk_velocity) * m_run_speed + l_walk_velocity * m_walk_speed;
			m_move_direction = m_look_direction * l_forward_velocity +
				m_look_direction * -l_backward_velocity +
				l_right_vector * l_right_velocity +
				l_left_vector * l_left_velocity;
			m_move_amount = std::max({ l_forward_velocity, l_backward_velocity, l_right_velocity, l_left_velocity }) * m_move_speed * p_clock.m_elapsed_second;

			if (core::bc_vector3f::length_sq(m_move_direction) == 0)
			{
				m_move_amount = 0;
			}
			
			if (m_move_amount <= 0)
			{
				m_move_direction = m_look_direction;
			}

			m_move_direction.normalize();
			
			// if two vectors are exactly opposite of each other 'rotation_between_two_vector' method in matrix3x3 will have undefined result
			if (m_move_direction.dot(m_look_direction) == -1.f)
			{
				m_move_direction.x -= 0.01f;
				m_move_direction.normalize();
			}
		}

		void bc_xbot_controller::_update_active_animation(const core_platform::bc_clock::update_param& p_clock)
		{
			m_active_job = nullptr;

			if (m_move_amount <= 0)
			{
				if(m_weapon.is_set())
				{
					switch (m_weapon->m_class)
					{
					case bc_weapon_class::rifle:
						m_active_job = get_rifle_idle_animation();
						break;
					default: 
						BC_ASSERT(false);
					}
				}
				else
				{
					m_active_job = get_idle_animation();
				}

				_blend_idle_animation(p_clock, *m_active_job);
				_weapon_ik_animation(m_weapon.get(), *m_active_job);
			}
			else
			{
				if (m_weapon.is_set())
				{
					switch (m_weapon->m_class)
					{
					case bc_weapon_class::rifle:
						m_active_job = get_rifle_running_animation();
						break;
					default:
						BC_ASSERT(false);
					}
				}
				else
				{
					m_active_job = get_running_animation();
				}

				_blend_running_animation(p_clock, *m_active_job);
				_weapon_ik_animation(m_weapon.get(), *m_active_job);
			}
		}

		void bc_xbot_controller::_blend_idle_animation(const core_platform::bc_clock::update_param& p_clock, bci_animation_job& p_idle_animation)
		{
			const auto l_look_velocity = m_look_velocity.get_value();
			bcFLOAT l_weights[3] = { l_look_velocity, 1 - l_look_velocity, l_look_velocity };

			auto* l_blending_job = bc_animation_job_helper::find_job<bc_blending_animation_job>(static_cast<bc_sequence_animation_job&>(p_idle_animation));
			l_blending_job->set_weights(&l_weights[0]);

			if (l_look_velocity == 0.f)
			{
				bcFLOAT l_times[3] = { 0, -1, 0 };
				l_blending_job->set_local_times(&l_times[0]);
			}
			else
			{
				bcFLOAT l_times[3] = { -1, 0, -1 };
				l_blending_job->set_local_times(&l_times[0]);
			}

			auto* l_aim_job = bc_animation_job_helper::find_job<bc_aim_animation_job>(static_cast<bc_sequence_animation_job&>(*m_active_job));
			l_aim_job->set_world_target(m_position + m_look_direction * 1000);
		}

		void bc_xbot_controller::_blend_running_animation(const core_platform::bc_clock::update_param& p_clock, bci_animation_job& p_running_animation)
		{
			const auto l_run_weight = (m_move_speed - m_walk_speed) / (m_run_speed - m_walk_speed);
			const auto l_walk_weight = 1 - l_run_weight;
			const auto l_move_speed_normalize = m_move_amount / (m_move_speed * p_clock.m_elapsed_second);
			bcFLOAT l_weights[] = 
			{
				1 - l_move_speed_normalize,
				l_move_speed_normalize * l_walk_weight,
				l_move_speed_normalize * l_walk_weight,
				l_move_speed_normalize * l_run_weight,
				l_move_speed_normalize * l_run_weight,
			};

			const auto l_move_direction_dot = m_look_direction.dot(m_move_direction);
			if (l_move_direction_dot >= -0.01f)
			{
				l_weights[2] = 0;
				l_weights[4] = 0;
			}
			else
			{
				l_weights[1] = 0;
				l_weights[3] = 0;
			}

			auto* l_blending_job = bc_animation_job_helper::find_job<bc_blending_animation_job>(static_cast<bc_sequence_animation_job&>(p_running_animation));
			l_blending_job->set_weights(&l_weights[0]);

			auto* l_aim_job = bc_animation_job_helper::find_job<bc_aim_animation_job>(static_cast<bc_sequence_animation_job&>(*m_active_job));
			l_aim_job->set_world_target(m_position + m_look_direction * 1000);
		}

		void bc_xbot_controller::_weapon_ik_animation(bc_xbot_weapon* p_weapon, bci_animation_job& p_main_animation)
		{
			auto* l_weapon_ik_job = bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<bc_sequence_animation_job&>(p_main_animation));
			if(!l_weapon_ik_job)
			{
				return;
			}

			l_weapon_ik_job->set_weapon(p_weapon);
		}

		void bc_xbot_controller::_update_world_transform(const core_platform::bc_clock::update_param& p_clock)
		{
			auto l_px_controller_pre_filter = physics::bc_scene_query_pre_filter_callback
			(
				[this](const physics::bc_scene_query_pre_filter_data& p_filter_data)
				{
					const auto l_actor = m_physics_system->get_game_actor(p_filter_data.m_actor);
					if(l_actor == m_actor)
					{
						return physics::bc_query_hit_type::none;
					}
					
					return physics::bc_query_hit_type::block;
				}
			);

			auto& l_px_scene = m_scene->get_px_scene();
			
			{
				physics::bc_scene_lock l_lock(&l_px_scene);
				const auto l_dis = m_move_direction * m_move_amount + l_px_scene.get_gravity() * p_clock.m_elapsed_second;
				
				m_px_controller->move(l_dis, p_clock, &l_px_controller_pre_filter);
				m_position = m_px_controller->get_foot_position();
			}
			
			core::bc_matrix4f l_world_transform;
			core::bc_matrix3f l_rotation;

			auto l_move_direction_sign = m_look_direction.dot(m_move_direction) >= -0.01f ? 1 : -1;

			if (graphic::bc_render_api_info::use_left_handed())
			{
				l_rotation.rotation_between_two_vector_lh(get_local_forward(), m_move_direction * l_move_direction_sign);
			}
			else
			{
				l_rotation.rotation_between_two_vector_rh(get_local_forward(), m_move_direction * l_move_direction_sign);
			}
			
			l_world_transform.make_identity();
			l_world_transform.set_rotation(l_rotation);
			l_world_transform.set_translation(m_position);

			bc_animation_job_helper::set_skinning_world_transform(static_cast<bc_sequence_animation_job&>(*m_idle_job), l_world_transform);
			bc_animation_job_helper::set_skinning_world_transform(static_cast<bc_sequence_animation_job&>(*m_running_job), l_world_transform);
			bc_animation_job_helper::set_skinning_world_transform(static_cast<bc_sequence_animation_job&>(*m_rifle_idle_job), l_world_transform);
			bc_animation_job_helper::set_skinning_world_transform(static_cast<bc_sequence_animation_job&>(*m_rifle_running_job), l_world_transform);
			
			m_actor.add_event(bc_world_transform_actor_event(l_world_transform, true));
		}

		void bc_xbot_controller::_update_weapon(const core_platform::bc_clock::update_param& p_clock)
		{
			if(!m_weapon.is_set())
			{
				return;
			}

			auto* l_model_job = bc_animation_job_helper::find_job<bc_local_to_model_animation_job>(static_cast<bc_sequence_animation_job&>(*m_active_job));
			auto* l_skinning_job = bc_animation_job_helper::find_job<bc_model_to_skinned_animation_job>(static_cast<bc_sequence_animation_job&>(*m_active_job));
			auto l_main_hand_translation = l_model_job->get_transforms()[m_main_hand_chain[2].first].get_translation();
			l_main_hand_translation = (l_skinning_job->get_world() * core::bc_vector4f(l_main_hand_translation, 1)).xyz();
			
			core::bc_matrix4f l_world_transform;
			core::bc_matrix3f l_rotation;

			if(graphic::bc_render_api_info::use_left_handed())
			{
				l_rotation.rotation_between_two_vector_lh(m_weapon->m_local_forward, m_look_direction);
			}
			else
			{
				l_rotation.rotation_between_two_vector_rh(m_weapon->m_local_forward, m_look_direction);
			}

			l_world_transform.make_identity();
			l_world_transform.set_rotation(l_rotation);
			l_world_transform.set_translation(l_main_hand_translation + l_rotation * m_weapon->m_main_hand_offset);
			
			m_weapon->m_actor.add_event(bc_world_transform_actor_event(l_world_transform));
		}
	}
}