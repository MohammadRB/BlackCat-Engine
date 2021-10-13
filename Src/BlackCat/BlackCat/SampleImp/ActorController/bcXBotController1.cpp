// [01/04/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Math/bcCoordinate.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "PhysicsImp/Body/bcRigidDynamic.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidControllerComponent.h"
#include "Game/Object/Scene/Component/bcWeaponComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Animation/bcAnimationSkeleton.h"
#include "Game/Object/Animation/bcSkeletonAnimation.h"
#include "Game/Object/Animation/bcAnimationJobBuilder.h"
#include "Game/Object/Animation/bcAnimationJobHelper.h"
#include "Game/Object/Animation/Job/bcBlendingAnimationJob.h"
#include "Game/Object/Animation/Job/bcAimAnimationJob.h"
#include "Game/Object/Animation/Job/bcAdditiveBlendingAnimationJob.h"
#include "Game/Object/Animation/Job/bcActorUpdateAnimationJob.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/bcGameSystem.h"
#include "BlackCat/SampleImp/ActorController/bcXBotController.h"
#include "BlackCat/SampleImp/ActorController/bcXBotUpdateAnimationJob.h"
#include "BlackCat/SampleImp/ActorController/bcXBotWeaponIKAnimationJob.h"

namespace black_cat
{
	bc_xbot_controller::bc_xbot_controller() noexcept
		: m_physics_system(nullptr),
		m_scene(nullptr),
		m_skinned_component(nullptr),
		m_bound_box_max_side_length(0),
		m_active_job(nullptr),
		m_look_speed(0),
		m_run_speed(0),
		m_walk_speed(0),
		m_move_speed(0),
		m_clock(0, 0, 0),
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
		m_move_amount(0),
		m_weapon(nullptr),
		m_weapon_shoot_time(-1)
	{
	}

	void bc_xbot_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		m_physics_system = &p_context.m_game_system.get_physics_system();
		m_actor = p_context.m_actor;
		m_skinned_component = p_context.m_actor.get_component<game::bc_skinned_mesh_component>();

		if (!m_skinned_component)
		{
			throw bc_invalid_operation_exception("xbot actor must have skinned components");
		}

		const auto& l_upper_body_chain_value = p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>("upper_body_chain");
		const auto& l_main_hand_chain_value = p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>("main_hand_chain");
		const auto& l_second_hand_chain_value = p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>("second_hand_chain");
		const auto& l_rifle_joint_value = p_context.m_parameters.get_value_throw<core::bc_string>("rifle_joint");
		m_rifle_joint_offset = p_context.m_parameters.get_value_vector3f_throw("rifle_joint_offset");

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
		m_local_origin = core::bc_vector3f(0);
		m_local_forward = core::bc_vector3f(0, 0, -1);
		m_look_direction = m_local_forward;
		m_move_direction = m_local_forward;

		m_idle_job = _create_idle_animation
		(
			"NeutralIdle", 
			"LeftTurn", 
			"RightTurn",
			nullptr,
			&m_idle_sample_job
		);
		m_running_job = _create_running_animation
		(
			m_idle_sample_job,
			"Walking",
			"WalkingBackward",
			"Running",
			"RunningBackward",
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		);
		m_rifle_idle_job = _create_idle_animation
		(
			"RifleAimingIdle",
			"RifleLeftTurn",
			"RifleRightTurn",
			"FiringRifle",
			&m_rifle_aiming_idle_sample_job
		);
		m_rifle_running_job = _create_running_animation
		(
			m_rifle_aiming_idle_sample_job,
			"RifleWalking",
			"RifleWalkingBackward",
			"RifleRunning",
			"RifleRunningBackward",
			"FiringRifle",
			nullptr,
			nullptr,
			nullptr,
			nullptr
		);
	}

	void bc_xbot_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		m_scene = &p_scene;
		
		const auto* l_mediate_component = p_context.m_actor.get_component<game::bc_mediate_component>();
		auto* l_rigid_controller_component = p_context.m_actor.get_component<game::bc_rigid_controller_component>();
		const auto l_bound_box_extends = l_mediate_component->get_bound_box().get_half_extends();
		m_bound_box_max_side_length = std::max({ l_bound_box_extends.x, l_bound_box_extends.y, l_bound_box_extends.z }) * 2;
		const auto l_px_material = p_context.m_game_system.get_render_system().get_material_manager().get_default_collider_material().m_px_material;
		const auto l_px_controller_desc = physics::bc_ccontroller_desc
		                                  (
			                                  core::bc_vector3f(m_position + core::bc_vector3f::up() * m_bound_box_max_side_length / 2),
											  m_bound_box_max_side_length * .65f,
			                                  m_bound_box_max_side_length * .2f,
			                                  l_px_material
		                                  )
										  .with_contact_offset(m_bound_box_max_side_length * .02f)
		                                  .with_step_offset(m_bound_box_max_side_length * 0.f)
		                                  .with_hit_callback(this);

		{
			physics::bc_scene_lock l_lock(&m_scene->get_px_scene());

			auto l_px_controller = m_scene->get_px_scene().create_ccontroller(l_px_controller_desc);
			l_px_controller->set_foot_position(m_position - m_local_origin);

			physics::bc_rigid_dynamic l_px_controller_actor = l_px_controller->get_actor();
			physics::bc_shape l_px_controller_shape;

			l_px_controller_actor.get_shapes(&l_px_controller_shape, 1);

			m_physics_system->set_game_actor(l_px_controller_actor, game::bc_actor());
			l_px_controller_shape.set_flag(physics::bc_shape_flag::query, false);
			
			l_rigid_controller_component->reset_controller(std::move(l_px_controller));
		}
		
		m_px_controller = l_rigid_controller_component->get_controller();
		
		m_look_speed = 10.0f;
		m_run_speed = m_bound_box_max_side_length * 1.3f;
		m_walk_speed = m_bound_box_max_side_length * 0.6f;
		
		m_active_job = m_idle_job.get();
	}

	void bc_xbot_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		if (!m_scene) // Has not added to scene yet
		{
			return;
		}

		m_clock = p_context.m_clock;
		_update_input();
		_update_direction();
		_update_active_animation();
		_update_world_transform();

		if(m_active_job)
		{
			m_skinned_component->add_animation_job(m_active_job);
		}
	}

	void bc_xbot_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		{
			physics::bc_scene_lock l_lock(&m_scene->get_px_scene());
			
			auto* l_rigid_controller_component = p_context.m_actor.get_component<game::bc_rigid_controller_component>();
			l_rigid_controller_component->reset_controller();
		}
		
		m_scene = nullptr;
		m_active_job = nullptr;
		
		if(m_weapon.has_value())
		{
			m_weapon.reset();
		}
	}

	void bc_xbot_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		const auto* l_world_transform_event = core::bci_message::as<game::bc_world_transform_actor_event>(p_context.m_event);
		if(l_world_transform_event && l_world_transform_event->get_transform_type() != game::bc_transform_event_type::physics)
		{
			const auto& l_world_transform = l_world_transform_event->get_transform();
			m_position = l_world_transform.get_translation() - m_local_origin;

			// In case we have transform event before actor added to scene
			if(m_px_controller.is_valid())
			{
				{
					physics::bc_scene_lock l_lock(&m_scene->get_px_scene());
					m_px_controller.set_foot_position(m_position - m_local_origin);
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

	void bc_xbot_controller::attach_weapon(game::bc_actor& p_weapon) noexcept
	{
		auto* l_weapon_component = p_weapon.get_component<game::bc_weapon_component>();
		if(!l_weapon_component)
		{
			core::bc_log(core::bc_log_type::error) << "Passed actor does not have weapon component" << core::bc_lend;
			return;
		}

		bc_xbot_weapon l_weapon;
		l_weapon.m_actor = p_weapon;
		l_weapon.m_component = l_weapon_component;
		l_weapon.m_class = l_weapon_component->get_class();
		l_weapon.m_local_up = l_weapon_component->get_up_ls();
		l_weapon.m_local_forward = l_weapon_component->get_forward_ls();
		l_weapon.m_main_hand_offset = l_weapon_component->get_main_hand_offset_ls();
		l_weapon.m_second_hand_offset = l_weapon_component->get_second_hand_offset_ls();
		l_weapon.m_rate_of_fire_seconds = l_weapon_component->get_rate_of_fire_seconds();

		m_weapon.reset(l_weapon);
		m_weapon_shoot_time = 0;

		auto* l_rifle_idle_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_rifle_idle_job));
		auto* l_rifle_running_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_rifle_running_job));

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

	void bc_xbot_controller::shoot_weapon() noexcept
	{
		if(!m_weapon.has_value())
		{
			return;
		}
		
		auto* l_additive_blend_job = game::bc_animation_job_helper::find_job<game::bc_additive_blending_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_active_job));
		if(!l_additive_blend_job || !l_additive_blend_job->get_additive_layer())
		{
			return;
		}

		if(m_weapon_shoot_time >= 0)
		{
			return;
		}

		m_weapon_shoot_time = 0;
		l_additive_blend_job->get_additive_layer()->set_local_time(0);
		l_additive_blend_job->get_additive_layer()->set_enabled(true);
		l_additive_blend_job->set_enabled(true);

		m_scene->add_bullet(m_weapon->m_component->shoot(m_look_direction));
	}

	void bc_xbot_controller::on_shape_hit(const physics::bc_ccontroller_shape_hit& p_hit)
	{
		const auto l_rigid_actor = p_hit.get_actor();
		const auto l_is_rigid_dynamic = l_rigid_actor.get_type() == physics::bc_actor_type::rigid_dynamic;
		if(!l_is_rigid_dynamic)
		{
			return;
		}

		auto l_rigid_dynamic = l_rigid_actor.is_rigid_dynamic();
		const auto l_is_rigid_kinematic = core::bc_enum::has(l_rigid_dynamic.get_rigid_body_flags(), physics::bc_rigid_body_flag::kinematic);
		if(l_is_rigid_kinematic)
		{
			return;
		}
		
		const auto l_ccontroller_actor = p_hit.get_ccontroller().get_actor().is_rigid_body();
		const auto l_multiplier = (m_move_amount / m_run_speed);
		l_rigid_dynamic.add_force(m_move_direction * l_multiplier * l_ccontroller_actor.get_mass());
	}

	void bc_xbot_controller::on_ccontroller_hit(const physics::bc_ccontroller_controller_hit& p_hit)
	{
	}

	core::bc_shared_ptr<game::bci_animation_job> bc_xbot_controller::_create_idle_animation(const bcCHAR* p_idle_animation,
		const bcCHAR* p_left_turn_animation,
		const bcCHAR* p_right_turn_animation,
		const bcCHAR* p_weapon_shoot_animation,
		core::bc_shared_ptr<game::bc_sampling_animation_job>* p_idle_sample_job)
	{
		auto& l_skeleton = *m_skinned_component->get_skeleton();
		auto& l_idle_animation = *m_skinned_component->find_animation(p_idle_animation);
		auto& l_left_turn_animation = *m_skinned_component->find_animation(p_left_turn_animation);
		auto& l_right_turn_animation = *m_skinned_component->find_animation(p_right_turn_animation);
		auto* l_weapon_shoot_animation = m_skinned_component->find_animation(p_weapon_shoot_animation);
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
		
		auto l_idle_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_idle_animation));
		auto l_left_turn_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_left_turn_animation));
		auto l_right_turn_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_right_turn_animation));
		core::bc_shared_ptr<game::bc_sampling_animation_job> l_weapon_shoot_sample_job;

		if(l_weapon_shoot_animation)
		{
			l_weapon_shoot_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, *l_weapon_shoot_animation));
			l_weapon_shoot_sample_job->set_play_mode(game::bc_animation_play_mode::once_disable);
		}
		
		auto l_idle_blending_job = core::bc_make_shared<game::bc_blending_animation_job>
		(
			game::bc_blending_animation_job(l_skeleton, { std::move(l_left_turn_sample_job), l_idle_sample_job, std::move(l_right_turn_sample_job) })
		);
		auto l_weapon_shoot_job = core::bc_make_shared<game::bc_additive_blending_animation_job>
		(
			game::bc_additive_blending_animation_job(l_skeleton, l_idle_blending_job, l_weapon_shoot_sample_job, nullptr)
		);
		auto l_local_to_model_job = core::bc_make_shared<game::bc_local_to_model_animation_job>
		(
			game::bc_local_to_model_animation_job(l_weapon_shoot_job, m_skinned_component->get_mesh(), m_skinned_component->get_model_transforms())
		);
		auto l_model_to_skinning_job = core::bc_make_shared<game::bc_model_to_skinned_animation_job>
		(
			game::bc_model_to_skinned_animation_job(l_local_to_model_job, m_skinned_component->get_world_transforms())
		);
		auto l_weapon_ik_job = core::bc_make_shared<bc_xbot_weapon_ik_animation_job>
		(
			bc_xbot_weapon_ik_animation_job
			(
				m_local_forward,
				l_weapon_shoot_job,
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
		auto l_aim_job = core::bc_make_shared<game::bc_aim_animation_job>
		(
			game::bc_aim_animation_job(l_weapon_shoot_job, l_local_to_model_job, l_model_to_skinning_job, core::bc_make_span(l_aim_bone_chain), m_local_forward)
		);
		auto l_actor_update_job = core::bc_make_shared<bc_xbot_update_animation_job>
		(
			bc_xbot_update_animation_job(m_actor, *this, *m_skinned_component, l_model_to_skinning_job)
		);
		
		bcFLOAT l_blending_weights[] = { 0,1,0 };
		l_idle_blending_job->set_weights(&l_blending_weights[0]);
		l_weapon_shoot_job->set_enabled(false);
		
		if(p_idle_sample_job)
		{
			*p_idle_sample_job = l_idle_sample_job;
		}
		
		return game::bc_animation_job_builder()
			.start_with(std::move(l_idle_blending_job))
			.then(l_weapon_shoot_job)
			.then(std::move(l_local_to_model_job))
			.then(std::move(l_weapon_ik_job))
			.then(std::move(l_aim_job))
			.then(std::move(l_model_to_skinning_job))
			.end_with(std::move(l_actor_update_job))
			.build();
	}

	core::bc_shared_ptr<game::bci_animation_job> bc_xbot_controller::_create_running_animation(core::bc_shared_ptr<game::bc_sampling_animation_job> p_idle_sample_job,
		const bcCHAR* p_walking_animation,
		const bcCHAR* p_walking_backward_animation,
		const bcCHAR* p_running_animation,
		const bcCHAR* p_running_backward_animation,
		const bcCHAR* p_weapon_shoot_animation,
		core::bc_shared_ptr<game::bc_sampling_animation_job>* p_walking_sample_job,
		core::bc_shared_ptr<game::bc_sampling_animation_job>* p_walking_backward_sample_job,
		core::bc_shared_ptr<game::bc_sampling_animation_job>* p_running_sample_job,
		core::bc_shared_ptr<game::bc_sampling_animation_job>* p_running_backward_sample_job)
	{
		auto& l_skeleton = *m_skinned_component->get_skeleton();
		auto& l_walking_animation = *m_skinned_component->find_animation(p_walking_animation);
		auto& l_walking_backward_animation = *m_skinned_component->find_animation(p_walking_backward_animation);
		auto& l_running_animation = *m_skinned_component->find_animation(p_running_animation);
		auto& l_running_backward_animation = *m_skinned_component->find_animation(p_running_backward_animation);
		auto* l_weapon_shoot_animation = m_skinned_component->find_animation(p_weapon_shoot_animation);
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

		auto l_walking_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_walking_animation));
		auto l_walking_backward_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_walking_backward_animation));
		auto l_running_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_running_animation));
		auto l_running_backward_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_running_backward_animation));
		core::bc_shared_ptr<game::bc_sampling_animation_job> l_weapon_shoot_sample_job;

		if (l_weapon_shoot_animation)
		{
			l_weapon_shoot_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, *l_weapon_shoot_animation));
			l_weapon_shoot_sample_job->set_play_mode(game::bc_animation_play_mode::once_disable);
		}
		
		auto l_running_blending_job = core::bc_make_shared<game::bc_blending_animation_job>
		(
			game::bc_blending_animation_job
			(
				l_skeleton,
				{
					std::move(p_idle_sample_job),
					l_walking_sample_job,
					l_walking_backward_sample_job,
					l_running_sample_job,
					l_running_backward_sample_job
				}
			)
		);
		auto l_weapon_shoot_job = core::bc_make_shared<game::bc_additive_blending_animation_job>
		(
			game::bc_additive_blending_animation_job(l_skeleton, l_running_blending_job, l_weapon_shoot_sample_job, nullptr)
		);
		auto l_local_to_model_job = core::bc_make_shared<game::bc_local_to_model_animation_job>
		(
			game::bc_local_to_model_animation_job(l_weapon_shoot_job, m_skinned_component->get_mesh(), m_skinned_component->get_model_transforms())
		);
		auto l_model_to_skinning_job = core::bc_make_shared<game::bc_model_to_skinned_animation_job>
		(
			game::bc_model_to_skinned_animation_job(l_local_to_model_job, m_skinned_component->get_world_transforms())
		);
		auto l_weapon_ik_job = core::bc_make_shared<bc_xbot_weapon_ik_animation_job>
		(
			bc_xbot_weapon_ik_animation_job
			(
				m_local_forward,
				l_weapon_shoot_job,
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
		auto l_aim_job = core::bc_make_shared<game::bc_aim_animation_job>
		(
			game::bc_aim_animation_job(l_weapon_shoot_job, l_local_to_model_job, l_model_to_skinning_job, core::bc_make_span(l_aim_bone_chain), m_local_forward)
		);
		auto l_actor_update_job = core::bc_make_shared<bc_xbot_update_animation_job>
		(
			bc_xbot_update_animation_job(m_actor, *this, *m_skinned_component, l_model_to_skinning_job)
		);

		l_weapon_shoot_job->set_enabled(false);

		if(p_walking_sample_job)
		{
			*p_walking_sample_job = l_walking_sample_job;
		}
		if (p_walking_backward_sample_job)
		{
			*p_walking_backward_sample_job = l_walking_backward_sample_job;
		}
		if (p_running_sample_job)
		{
			*p_running_sample_job = l_running_sample_job;
		}
		if (p_running_backward_sample_job)
		{
			*p_running_backward_sample_job = l_running_backward_sample_job;
		}
		
		return game::bc_animation_job_builder()
			.start_with(std::move(l_running_blending_job))
			.then(l_weapon_shoot_job)
			.then(std::move(l_local_to_model_job))
			.then(std::move(l_weapon_ik_job))
			.then(std::move(l_aim_job))
			.then(std::move(l_model_to_skinning_job))
			.end_with(std::move(l_actor_update_job))
			.build();
	}

	void bc_xbot_controller::_update_input()
	{
		core_platform::atomic_thread_fence(core_platform::bc_memory_order::acquire);

		if (m_look_delta_x != 0)
		{
			m_look_velocity.push(m_clock.m_elapsed_second);
		}
		else
		{
			m_look_velocity.release(m_clock.m_elapsed_second);
		}

		if (m_forward_pressed)
		{
			m_forward_velocity.push(m_clock.m_elapsed_second);
		}
		else
		{
			m_forward_velocity.release(m_clock.m_elapsed_second);
		}

		if (m_right_pressed)
		{
			m_right_velocity.push(m_clock.m_elapsed_second);
		}
		else
		{
			m_right_velocity.release(m_clock.m_elapsed_second);
		}

		if (m_left_pressed)
		{
			m_left_velocity.push(m_clock.m_elapsed_second);
		}
		else
		{
			m_left_velocity.release(m_clock.m_elapsed_second);
		}

		const bool l_is_moving_forward = m_forward_velocity.get_value() || m_right_velocity.get_value() || m_left_velocity.get_value();
		const bool l_is_moving_backward = m_backward_velocity.get_value();

		if (m_backward_pressed && (!l_is_moving_forward || l_is_moving_backward))
		{
			m_backward_velocity.push(m_clock.m_elapsed_second);
		}
		else
		{
			m_backward_velocity.release(m_clock.m_elapsed_second);
		}

		if (m_walk_pressed)
		{
			m_walk_velocity.push(m_clock.m_elapsed_second);
		}
		else
		{
			m_walk_velocity.release(m_clock.m_elapsed_second);
		}
	}

	void bc_xbot_controller::_update_direction()
	{
		const auto l_look_velocity = m_look_velocity.get_value();
		const auto l_forward_velocity = m_forward_velocity.get_value();
		const auto l_backward_velocity = m_backward_velocity.get_value();
		const auto l_right_velocity = m_right_velocity.get_value();
		const auto l_left_velocity = m_left_velocity.get_value();
		const auto l_walk_velocity = m_walk_velocity.get_value();
				
		core::bc_matrix3f l_look_rotation;
		core::bc_vector3f l_right_vector;
		core::bc_vector3f l_left_vector;

		if constexpr (graphic::bc_render_api_info::use_left_handed())
		{
			l_look_rotation.rotation_y_lh(core::bc_to_radian(m_look_delta_x * l_look_velocity * m_look_speed * m_clock.m_elapsed_second));
		}
		else
		{
			l_look_rotation.rotation_y_rh(core::bc_to_radian(m_look_delta_x * l_look_velocity * m_look_speed * m_clock.m_elapsed_second));
		}

		m_look_direction = core::bc_vector3f::normalize(l_look_rotation * m_look_direction);

		if constexpr (graphic::bc_render_api_info::use_left_handed())
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
		m_move_amount = std::max({ l_forward_velocity, l_backward_velocity, l_right_velocity, l_left_velocity }) * m_move_speed;

		if (core::bc_vector3f::length_sq(m_move_direction) <= 0)
		{
			m_move_amount = 0;
		}
		
		if (m_move_amount <= 0)
		{
			m_move_direction = m_look_direction;
		}

		m_move_direction.normalize();
		
		// if two vectors are exactly opposite of each other 'rotation_between_two_vector' method in matrix3x3 will have undefined result
		if (m_move_direction.dot(m_look_direction) >= -1.f)
		{
			m_move_direction.x -= 0.01f;
			m_move_direction.normalize();
		}
	}

	void bc_xbot_controller::_update_active_animation()
	{
		m_active_job = nullptr;

		if (m_move_amount <= 0)
		{
			if(m_weapon.has_value())
			{
				switch (m_weapon->m_class)
				{
				case game::bc_weapon_class::rifle:
					m_active_job = get_rifle_idle_animation();
					break;
				default:
					core::bc_log(core::bc_log_type::error) << "xbot weapon class is unknown" << core::bc_lend;
					BC_ASSERT(false);
				}
			}
			else
			{
				m_active_job = get_idle_animation();
			}

			_blend_idle_animation(*m_active_job);
			_weapon_ik_animation(m_weapon.get(), *m_active_job);
		}
		else
		{
			if (m_weapon.has_value())
			{
				switch (m_weapon->m_class)
				{
				case game::bc_weapon_class::rifle:
					m_active_job = get_rifle_running_animation();
					break;
				default:
					core::bc_log(core::bc_log_type::error) << "xbot weapon class is unknown" << core::bc_lend;
					BC_ASSERT(false);
				}
			}
			else
			{
				m_active_job = get_running_animation();
			}

			_blend_running_animation(*m_active_job);
			_weapon_ik_animation(m_weapon.get(), *m_active_job);
		}

		_blend_weapon_shoot_animation();
	}

	void bc_xbot_controller::_blend_idle_animation(game::bci_animation_job& p_idle_animation)
	{
		const auto l_look_velocity = m_look_velocity.get_value();
		bcFLOAT l_weights[3] = { l_look_velocity, 1 - l_look_velocity, l_look_velocity };

		auto* l_blending_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_idle_animation));
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

		auto* l_aim_job = game::bc_animation_job_helper::find_job<game::bc_aim_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_active_job));
		l_aim_job->set_world_target(m_position + m_look_direction * 1000);
	}

	void bc_xbot_controller::_blend_running_animation(game::bci_animation_job& p_running_animation)
	{
		const auto l_run_weight = (m_move_speed - m_walk_speed) / (m_run_speed - m_walk_speed);
		const auto l_walk_weight = 1 - l_run_weight;
		const auto l_move_speed_normalize = m_move_amount / m_move_speed;
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

		auto* l_blending_job = game::bc_animation_job_helper::find_job<game::bc_blending_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_running_animation));
		l_blending_job->set_weights(&l_weights[0]);

		auto* l_aim_job = game::bc_animation_job_helper::find_job<game::bc_aim_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_active_job));
		l_aim_job->set_world_target(m_position + m_look_direction * 1000);
	}

	void bc_xbot_controller::_blend_weapon_shoot_animation()
	{
		if (!m_weapon.has_value())
		{
			return;
		}
		
		auto* l_additive_blend_job = game::bc_animation_job_helper::find_job<game::bc_additive_blending_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_active_job));
		if (!l_additive_blend_job)
		{
			return;
		}

		auto* l_additive_layer = l_additive_blend_job->get_additive_layer().get();
		if(!l_additive_layer)
		{
			return;
		}

		if (m_weapon_shoot_time >= 0)
		{
			m_weapon_shoot_time += m_clock.m_elapsed_second;

			if (m_weapon_shoot_time >= m_weapon->m_rate_of_fire_seconds)
			{
				m_weapon_shoot_time = -1;
			}
		}
		
		l_additive_blend_job->set_weights(1, m_move_amount ? std::max((m_move_amount / m_run_speed) * 2, 1.4f) : 1); // Increase weapon shoot impact in running mode

		// Once weapon shoot animation stopped disable additive blend
		if(!l_additive_layer->get_enabled())
		{
			l_additive_layer->set_local_time(0);
			l_additive_blend_job->set_enabled(false);
		}
	}

	void bc_xbot_controller::_weapon_ik_animation(bc_xbot_weapon* p_weapon, game::bci_animation_job& p_main_animation)
	{
		auto* l_weapon_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(static_cast<game::bc_sequence_animation_job&>(p_main_animation));
		if(!l_weapon_ik_job)
		{
			return;
		}

		l_weapon_ik_job->set_weapon(p_weapon);
	}

	void bc_xbot_controller::_update_world_transform()
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
			const auto l_displacement = (m_move_direction * m_move_amount + l_px_scene.get_gravity() * m_bound_box_max_side_length) * m_clock.m_elapsed_second;
			
			m_px_controller.move(l_displacement, m_clock, &l_px_controller_pre_filter);
			m_position = m_px_controller.get_foot_position();
		}
		
		core::bc_matrix4f l_world_transform;
		core::bc_matrix3f l_rotation;

		auto l_move_direction_sign = m_look_direction.dot(m_move_direction) >= -0.01f ? 1 : -1;

		if constexpr (graphic::bc_render_api_info::use_left_handed())
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

		game::bc_animation_job_helper::set_skinning_world_transform(static_cast<game::bc_sequence_animation_job&>(*m_idle_job), l_world_transform);
		game::bc_animation_job_helper::set_skinning_world_transform(static_cast<game::bc_sequence_animation_job&>(*m_running_job), l_world_transform);
		game::bc_animation_job_helper::set_skinning_world_transform(static_cast<game::bc_sequence_animation_job&>(*m_rifle_idle_job), l_world_transform);
		game::bc_animation_job_helper::set_skinning_world_transform(static_cast<game::bc_sequence_animation_job&>(*m_rifle_running_job), l_world_transform);
		
		m_actor.add_event(game::bc_world_transform_actor_event(l_world_transform, game::bc_transform_event_type::physics));
	}

	void bc_xbot_controller::_update_weapon_transform()
	{
		if(!m_weapon.has_value())
		{
			return;
		}

		auto* l_model_job = game::bc_animation_job_helper::find_job<game::bc_local_to_model_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_active_job));
		auto* l_skinning_job = game::bc_animation_job_helper::find_job<game::bc_model_to_skinned_animation_job>(static_cast<game::bc_sequence_animation_job&>(*m_active_job));
		auto l_main_hand_transform = l_model_job->get_transforms()[m_main_hand_chain[2].first];
		l_main_hand_transform.make_neutralize_scale();

		core::bc_matrix3f l_aim_target_rotation;
		core::bc_matrix3f l_weapon_forward_rotation;
		core::bc_matrix3f l_weapon_up_rotation;

		if constexpr (graphic::bc_render_api_info::use_left_handed())
		{
			l_aim_target_rotation.rotation_between_two_vector_lh(m_local_forward, m_look_direction);
			l_weapon_forward_rotation.rotation_between_two_vector_lh(m_weapon->m_local_forward, m_look_direction);
			l_weapon_up_rotation.rotation_between_two_vector_lh(m_weapon->m_local_up, core::bc_vector3f::up());
		}
		else
		{
			l_aim_target_rotation.rotation_between_two_vector_rh(m_local_forward, m_look_direction);
			l_weapon_forward_rotation.rotation_between_two_vector_rh(m_weapon->m_local_forward, m_look_direction);
			l_weapon_up_rotation.rotation_between_two_vector_rh(m_weapon->m_local_up, core::bc_vector3f::up());
		}

		const auto l_main_hand_offset = l_aim_target_rotation * m_weapon->m_main_hand_offset;
		auto l_main_hand_world_transform = l_main_hand_transform * l_skinning_job->get_world();
		l_main_hand_world_transform.set_translation(l_main_hand_world_transform.get_translation() + l_main_hand_offset);

		const auto l_aim_lock = .7f;
		const auto l_main_hand_final_rotation = l_main_hand_world_transform.get_rotation() * (1 - l_aim_lock) + (l_weapon_up_rotation * l_weapon_forward_rotation) * l_aim_lock;
		
		l_main_hand_world_transform.set_rotation(l_main_hand_final_rotation);

		m_weapon->m_actor.add_event(game::bc_world_transform_actor_event(l_main_hand_world_transform));
		m_weapon->m_actor.mark_for_double_update();
	}
}