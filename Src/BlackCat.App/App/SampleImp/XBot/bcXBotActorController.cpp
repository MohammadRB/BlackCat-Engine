// [01/04/2021 MRB]

#include "App/AppPCH.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Math/bcCoordinate.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "Core/bcUtility.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "PhysicsImp/Body/bcRigidDynamic.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidControllerComponent.h"
#include "Game/Object/Scene/Component/bcHumanRagdollComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcWeaponComponent.h"
#include "Game/Object/Scene/Component/bcSoundComponent.h"
#include "Game/Object/Scene/Component/bcCheckPointComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/System/Animation/bcAnimationSkeleton.h"
#include "Game/System/Animation/bcSkeletonAnimation.h"
#include "Game/System/Animation/bcAnimationJobBuilder.h"
#include "Game/System/Animation/bcAnimationJobHelper.h"
#include "Game/System/Animation/Job/bcSamplingAnimationJob.h"
#include "Game/System/Animation/Job/bcBlendingAnimationJob.h"
#include "Game/System/Animation/Job/bcAimAnimationJob.h"
#include "Game/System/Animation/Job/bcPartialBlendingAnimationJob.h"
#include "Game/System/Animation/Job/bcTwoBoneIKAnimationJob.h"
#include "Game/System/Animation/Job/bcAdditiveBlendingAnimationJob.h"
#include "Game/System/Animation/Job/bcActorUpdateAnimationJob.h"
#include "Game/System/Animation/Job/bcExecuteOneAnimationJob.h"
#include "Game/System/Animation/Job/bcExecuteOnceAnimationJob.h"
#include "Game/System/Animation/Job/bcExecuteOneAnimationJob.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcJsonParse.h"
#include "Game/bcUtility.h"
#include "App/SampleImp/XBot/bcXBotActorController.h"
#include "App/SampleImp/XBot/bcXBotUpdateAnimationJob.h"
#include "App/SampleImp/XBot/bcXBotWeaponIKAnimationJob.h"

namespace black_cat
{
	bc_xbot_actor_controller::bc_xbot_actor_controller() noexcept
		: m_physics_system(nullptr),
		m_scene(nullptr),
		m_skinned_mesh_component(nullptr),
		m_rigid_controller_component(nullptr),
		m_human_ragdoll_component(nullptr),
		m_grenade_anim_attach_time(0),
		m_grenade_anim_throw_time(0),
		m_bound_box_max_side_length(0),
		m_look_delta_x(0),
		m_look_velocity(0, 1, 0.50f),
		m_forward_velocity(0, 1, 0.35f),
		m_backward_velocity(m_forward_velocity),
		m_right_velocity(m_forward_velocity),
		m_left_velocity(m_forward_velocity),
		m_walk_velocity(m_forward_velocity),
		m_ragdoll_enabled(false),
		m_weapon(nullptr)
	{
	}

	void bc_xbot_actor_controller::update_attachment_transforms()
	{
		if (m_grenade.has_value())
		{
			if (m_state_machine->m_state.m_grenade_throw_time >= m_grenade_anim_attach_time &&
				m_state_machine->m_state.m_grenade_throw_time <= m_grenade_anim_throw_time)
			{
				if (m_grenade->m_actor == nullptr)
				{
					m_grenade->m_actor = get_scene()->create_actor(m_grenade->m_entity_name, core::bc_matrix4f::translation_matrix(m_position));

					auto* l_rigid_dynamic_component = m_grenade->m_actor.get_component<game::bc_rigid_dynamic_component>();
					if (l_rigid_dynamic_component)
					{
						l_rigid_dynamic_component->set_enable(false);
						bc_actor_unmark_for_checkpoint(m_grenade->m_actor);
					}
				}

				const auto l_grenade_transform = _calculate_attachment_transform
				(
					m_right_hand_chain[2].first,
					m_right_hand_weapon_up,
					m_right_hand_weapon_forward,
					m_right_hand_attachment_offset,
					m_grenade->m_local_up,
					m_grenade->m_local_forward,
					core::bc_vector3f(0)
				);

				m_grenade->m_actor.add_event(game::bc_world_transform_actor_event(l_grenade_transform));
				m_grenade->m_actor.mark_for_double_update();
			}
			else if (m_grenade->m_actor != nullptr)
			{
				throw_grenade(m_grenade->m_actor);
				m_grenade.reset();
			}
		}

		if (m_weapon.has_value())
		{
			const auto* l_weapon_ik_job = game::bc_animation_job_helper::find_job<bc_xbot_weapon_ik_animation_job>(*m_state_machine->get_active_animation());
			const auto l_weapon_ik_weight = l_weapon_ik_job->get_weight();
			core::bc_matrix4f l_weapon_transform;

			if (l_weapon_ik_weight >= 1)
			{
				l_weapon_transform = _calculate_weapon_aim_transform();
			}
			else
			{
				const auto l_weapon_aim_transform = _calculate_weapon_aim_transform();
				const auto l_weapon_attached_transform = _calculate_attachment_transform
				(
					m_left_hand_chain[2].first,
					m_left_hand_weapon_up,
					m_left_hand_weapon_forward,
					m_left_hand_attachment_offset,
					m_weapon->m_local_up,
					m_weapon->m_local_forward,
					m_weapon->m_second_hand_offset
				);
				l_weapon_transform = (l_weapon_aim_transform * l_weapon_ik_weight) + (l_weapon_attached_transform * (1 - l_weapon_ik_weight));
			}

			m_weapon->m_actor.add_event(game::bc_world_transform_actor_event(l_weapon_transform));
			m_weapon->m_actor.mark_for_double_update();
		}
	}

	void bc_xbot_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		try
		{
			bc_actor_network_controller::initialize(p_context);
		}
		catch (...)
		{
			// If no network component is presented network controller throws exception
		}

		m_physics_system = &p_context.m_game_system.get_physics_system();
		m_actor = p_context.m_actor;
		m_skinned_mesh_component = p_context.m_actor.get_component<game::bc_skinned_mesh_component>();
		m_rigid_controller_component = p_context.m_actor.get_component<game::bc_rigid_controller_component>();
		m_human_ragdoll_component = p_context.m_actor.get_component<game::bc_human_ragdoll_component>();

		if (!m_skinned_mesh_component)
		{
			throw bc_invalid_operation_exception("xbot actor must have skinned components");
		}

		json_parse::bc_load_throw(p_context.m_parameters, "origin_ls", m_local_origin);
		json_parse::bc_load_throw(p_context.m_parameters, "forward_ls", m_local_forward);
		json_parse::bc_load_throw(p_context.m_parameters, "right_hand_weapon_up", m_right_hand_weapon_up);
		json_parse::bc_load_throw(p_context.m_parameters, "right_hand_weapon_forward", m_right_hand_weapon_forward);
		json_parse::bc_load_throw(p_context.m_parameters, "right_hand_attachment_offset", m_right_hand_attachment_offset);
		json_parse::bc_load_throw(p_context.m_parameters, "left_hand_weapon_up", m_left_hand_weapon_up);
		json_parse::bc_load_throw(p_context.m_parameters, "left_hand_weapon_forward", m_left_hand_weapon_forward);
		json_parse::bc_load_throw(p_context.m_parameters, "left_hand_attachment_offset", m_left_hand_attachment_offset);
		json_parse::bc_load_throw(p_context.m_parameters, "rifle_joint_offset", m_rifle_joint_offset);
		json_parse::bc_load_throw(p_context.m_parameters, "grenade_anim_attach_time", m_grenade_anim_attach_time);
		json_parse::bc_load_throw(p_context.m_parameters, "grenade_anim_throw_time", m_grenade_anim_throw_time);

		const core::bc_vector<core::bc_any>* l_upper_body_chain_value = nullptr;
		const core::bc_vector<core::bc_any>* l_right_hand_chain_value = nullptr;
		const core::bc_vector<core::bc_any>* l_left_hand_chain_value = nullptr;
		const core::bc_string* l_rifle_joint_value = nullptr;

		json_parse::bc_load_throw(p_context.m_parameters, "upper_body_chain", l_upper_body_chain_value);
		json_parse::bc_load_throw(p_context.m_parameters, "right_hand_chain", l_right_hand_chain_value);
		json_parse::bc_load_throw(p_context.m_parameters, "left_hand_chain", l_left_hand_chain_value);
		json_parse::bc_load_throw(p_context.m_parameters, "rifle_joint", l_rifle_joint_value);

		std::transform
		(
			std::begin(*l_upper_body_chain_value),
			std::end(*l_upper_body_chain_value),
			std::back_inserter(m_upper_body_chain),
			[](const core::bc_any& p_any)
			{
				return p_any.as_throw<core::bc_string>();
			}
		);
		std::transform
		(
			std::begin(*l_right_hand_chain_value),
			std::end(*l_right_hand_chain_value),
			std::begin(m_right_hand_chain),
			[this](const core::bc_any& p_any)
			{
				const auto l_joint = m_skinned_mesh_component->get_skeleton()->find_joint_by_name(p_any.as_throw<core::bc_string>().c_str());
				if (!l_joint.second)
				{
					throw bc_invalid_argument_exception("Right hand and/or left hand were not found");
				}

				return l_joint;
			}
		);
		std::transform
		(
			std::begin(*l_left_hand_chain_value),
			std::end(*l_left_hand_chain_value),
			std::begin(m_left_hand_chain),
			[this](const core::bc_any& p_any)
			{
				const auto l_joint = m_skinned_mesh_component->get_skeleton()->find_joint_by_name(p_any.as_throw<core::bc_string>().c_str());
				if (!l_joint.second)
				{
					throw bc_invalid_argument_exception("Right hand and/or left hand were not found");
				}

				return l_joint;
			}
		);
		m_rifle_joint = m_skinned_mesh_component->get_skeleton()->find_joint_by_name(l_rifle_joint_value->c_str());

		m_animation_pipeline = _create_animation_pipeline
		(
			"NeutralIdle",
			"LeftTurn",
			"RightTurn",
			"RifleIdleAiming",
			"RifleIdleAimingLeftTurn",
			"RifleIdleAimingRightTurn",
			"Walking",
			"WalkingBackward",
			"Running",
			"RunningBackward",
			"RifleWalking",
			"RifleWalkingBackward",
			"RifleRunning",
			"RifleRunningBackward",
			"GrenadeThrow",
			"FiringRifle"
		);
	}

	void bc_xbot_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		m_scene = &p_scene;
		const auto* l_mediate_component = p_context.m_actor.get_component<game::bc_mediate_component>();
		const auto l_bound_box_extends = l_mediate_component->get_bound_box().get_half_extends();
		m_bound_box_max_side_length = std::max({ l_bound_box_extends.x, l_bound_box_extends.y, l_bound_box_extends.z }) * 2;

		m_px_controller_material = p_context.m_game_system.get_render_system().get_material_manager().get_default_collider_material().m_px_material;
		m_rigid_controller_component->reset_controller(create_px_controller());

		auto l_mass = 0.f;

		{
			game::bc_rigid_component_shared_lock l_lock(*m_rigid_controller_component);
			l_mass = m_rigid_controller_component->get_body().get_mass();
		}

		m_state_machine = core::bc_make_unique<bc_xbot_state_machine>(bc_xbot_state_machine
		(
			m_local_forward,
			7.f,
			m_bound_box_max_side_length * 1.3f,
			m_bound_box_max_side_length * 0.6f,
			l_mass,
			*m_animation_pipeline
		));

		if(const auto* l_sound_component = p_context.m_actor.get_component<game::bc_sound_component>())
		{
			m_running_sound = l_sound_component->play_sound("running");
			if(m_running_sound.is_valid())
			{
				m_running_sound.set_mode(core::bc_enum::mask_or({ sound::bc_sound_mode::loop, m_running_sound.get_mode() }));
				m_running_sound.set_volume(0);
			}
		}
	}
	
	void bc_xbot_actor_controller::update(const bc_xbot_input_update_context& p_context)
	{
		if (!m_scene)
		{
			return;
		}

		m_look_delta_x = p_context.m_look_delta_x;
		if (p_context.m_look_delta_x != 0)
		{
			m_look_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_look_velocity.release(p_context.m_clock.m_elapsed_second);
		}

		if (p_context.m_forward_pressed)
		{
			m_forward_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_forward_velocity.release(p_context.m_clock.m_elapsed_second);
		}

		if (p_context.m_right_pressed)
		{
			m_right_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_right_velocity.release(p_context.m_clock.m_elapsed_second);
		}

		if (p_context.m_left_pressed)
		{
			m_left_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_left_velocity.release(p_context.m_clock.m_elapsed_second);
		}

		const bool l_is_moving_forward = m_forward_velocity.get_value() > 0.f || m_right_velocity.get_value() > 0.f || m_left_velocity.get_value() > 0.f;
		const bool l_is_moving_backward = m_backward_velocity.get_value() > 0.f;

		if (p_context.m_backward_pressed && (!l_is_moving_forward || l_is_moving_backward))
		{
			m_backward_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_backward_velocity.release(p_context.m_clock.m_elapsed_second);
		}

		if (p_context.m_walk_pressed)
		{
			m_walk_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_walk_velocity.release(p_context.m_clock.m_elapsed_second);
		}
		
		m_state_machine->update
		(
			bc_xbot_state_update_params
			{
				p_context.m_clock,
				m_position,
				m_look_delta_x,
				m_look_velocity.get_value(),
				m_forward_velocity.get_value(),
				m_backward_velocity.get_value(),
				m_right_velocity.get_value(),
				m_left_velocity.get_value(),
				m_walk_velocity.get_value()
			}
		);

		if (!m_ragdoll_enabled)
		{
			_update_px_move(p_context.m_clock, m_state_machine->m_state.m_move_direction * m_state_machine->m_state.m_move_amount);
			_update_world_transform();
			_update_sounds();

			m_skinned_mesh_component->add_animation_job(m_state_machine->get_active_animation());
		}
	}

	void bc_xbot_actor_controller::update(const bc_xbot_input_update_context1& p_context)
	{
		if (!m_scene)
		{
			return;
		}

		m_look_delta_x = 0;

		if (p_context.m_forward_pressed)
		{
			m_forward_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_forward_velocity.release(p_context.m_clock.m_elapsed_second);
		}

		if (p_context.m_backward_pressed)
		{
			m_backward_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_backward_velocity.release(p_context.m_clock.m_elapsed_second);
		}

		if (p_context.m_right_pressed)
		{
			m_right_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_right_velocity.release(p_context.m_clock.m_elapsed_second);
		}

		if (p_context.m_left_pressed)
		{
			m_left_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_left_velocity.release(p_context.m_clock.m_elapsed_second);
		}

		if (p_context.m_walk_pressed)
		{
			m_walk_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_walk_velocity.release(p_context.m_clock.m_elapsed_second);
		}

		m_state_machine->update
		(
			bc_xbot_state_update_params1
			{
				p_context.m_clock,
				p_context.m_position,
				p_context.m_look_direction,
				p_context.m_look_side,
				p_context.m_look_velocity,
				m_forward_velocity.get_value(),
				m_backward_velocity.get_value(),
				m_right_velocity.get_value(),
				m_left_velocity.get_value(),
				m_walk_velocity.get_value()
			}
		);

		if(!m_ragdoll_enabled)
		{
			_update_px_position(p_context.m_position);
			_update_world_transform();
			_update_sounds();

			m_skinned_mesh_component->add_animation_job(m_state_machine->get_active_animation());
		}
	}

	void bc_xbot_actor_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		{
			physics::bc_scene_lock l_lock(&m_scene->get_px_scene());

			auto* l_rigid_controller_component = p_context.m_actor.get_component<game::bc_rigid_controller_component>();
			l_rigid_controller_component->reset_controller();
		}

		if (m_weapon.has_value())
		{
			m_scene->remove_actor(m_weapon->m_actor);
			m_weapon.reset();
		}

		if(m_grenade.has_value())
		{
			m_scene->remove_actor(m_grenade->m_actor);
			m_grenade.reset();
		}

		m_scene = nullptr;
		m_state_machine = nullptr;
	}

	void bc_xbot_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		const auto* l_world_transform_event = core::bci_message::as<game::bc_world_transform_actor_event>(p_context.m_event);
		if (l_world_transform_event && l_world_transform_event->get_transform_type() != game::bc_transform_event_type::physics)
		{
			const auto& l_world_transform = l_world_transform_event->get_transform();
			m_position = l_world_transform.get_translation() - m_local_origin;

			// In case we have transform event before actor added to scene
			auto l_px_controller = m_rigid_controller_component->get_controller();
			if (l_px_controller.is_valid())
			{
				{
					physics::bc_scene_lock l_lock(&m_scene->get_px_scene());
					l_px_controller.set_foot_position(m_position - m_local_origin);
				}
			}
		}
	}

	physics::bc_ccontroller_ref bc_xbot_actor_controller::create_px_controller()
	{
		const auto l_px_controller_desc = physics::bc_ccontroller_desc
		                                  (
			                                  core::bc_vector3f(m_position + core::bc_vector3f::up() * m_bound_box_max_side_length / 2),
			                                  m_bound_box_max_side_length * .65f,
			                                  m_bound_box_max_side_length * .2f,
			                                  m_px_controller_material
		                                  )
		                                  .with_contact_offset(m_bound_box_max_side_length * .02f)
		                                  .with_step_offset(m_bound_box_max_side_length * 0.f)
		                                  .with_hit_callback(this);

		{
			physics::bc_scene_lock l_lock(&m_scene->get_px_scene());

			auto l_px_controller = m_scene->get_px_scene().create_ccontroller(l_px_controller_desc);
			l_px_controller->set_foot_position(m_position - m_local_origin);

			auto l_px_controller_actor = l_px_controller->get_actor();
			physics::bc_shape l_px_controller_shape;

			l_px_controller_actor.get_shapes(&l_px_controller_shape, 1);

			m_physics_system->set_game_actor(l_px_controller_actor, game::bc_actor());
			l_px_controller_shape.set_flag(physics::bc_shape_flag::query, false);

			return l_px_controller;
		}
	}

	bool bc_xbot_actor_controller::start_grenade_throw(const bcCHAR* p_entity_name) noexcept
	{
		const bool l_threw = m_state_machine->throw_grenade();
		if (!l_threw)
		{
			return false;
		}

		m_grenade.reset(bc_xbot_grenade{ p_entity_name });
		return true;
	}

	void bc_xbot_actor_controller::attach_weapon(game::bc_actor& p_weapon) noexcept
	{
		auto* l_weapon_component = p_weapon.get_component<game::bc_weapon_component>();
		if (!l_weapon_component)
		{
			core::bc_log(core::bc_log_type::error) << "Passed actor does not have weapon component" << core::bc_lend;
			return;
		}

		auto l_weapon_mass = 0.f;
		if (auto* l_rigid_dynamic_component = p_weapon.get_component<game::bc_rigid_dynamic_component>())
		{
			l_rigid_dynamic_component->set_kinematic(true);

			{
				game::bc_rigid_component_shared_lock l_lock(*l_rigid_dynamic_component);
				l_weapon_mass = l_rigid_dynamic_component->get_body().get_mass();
			}

			game::bc_actor_unmark_for_checkpoint(p_weapon);
		}

		bc_xbot_weapon l_weapon;
		l_weapon.m_actor = p_weapon;
		l_weapon.m_component = l_weapon_component;
		l_weapon.m_class = l_weapon_component->get_class();
		l_weapon.m_local_up = l_weapon_component->get_up_ls();
		l_weapon.m_local_forward = l_weapon_component->get_forward_ls();
		l_weapon.m_local_right = l_weapon_component->get_right_ls();
		l_weapon.m_main_hand_offset = l_weapon_component->get_main_hand_offset_ls();
		l_weapon.m_second_hand_offset = l_weapon_component->get_second_hand_offset_ls();
		l_weapon.m_rate_of_fire_seconds = l_weapon_component->get_rate_of_fire_seconds();
		l_weapon.m_mass = l_weapon_mass;

		m_weapon.reset(l_weapon);
		m_state_machine->attach_weapon(*m_weapon, m_rifle_joint.second, m_rifle_joint_offset);
	}

	void bc_xbot_actor_controller::detach_weapon() noexcept
	{
		m_state_machine->detach_weapon();

		if (auto* l_rigid_dynamic_component = m_weapon->m_actor.get_component<game::bc_rigid_dynamic_component>())
		{
			l_rigid_dynamic_component->set_kinematic(false);
			l_rigid_dynamic_component->set_enable(false);

			{
				game::bc_rigid_component_lock l_lock(*l_rigid_dynamic_component);

				auto l_rigid_dynamic = l_rigid_dynamic_component->get_dynamic_body();
				const auto l_rigid_dynamic_pose = l_rigid_dynamic.get_global_pose();
				l_rigid_dynamic.set_global_pose(physics::bc_transform(l_rigid_dynamic_pose.get_position() + get_look_direction(), l_rigid_dynamic_pose.get_matrix3()));
			}
		}

		m_scene->remove_actor(m_weapon->m_actor);
		m_weapon.reset();
	}

	void bc_xbot_actor_controller::drop_weapon() noexcept
	{
		m_state_machine->detach_weapon();

		if (auto* l_rigid_dynamic_component = m_weapon->m_actor.get_component<game::bc_rigid_dynamic_component>())
		{
			l_rigid_dynamic_component->set_kinematic(false);

			{
				game::bc_rigid_component_lock l_lock(*l_rigid_dynamic_component);
				
				auto l_rigid_dynamic = l_rigid_dynamic_component->get_dynamic_body();
				const auto l_rigid_dynamic_pose = l_rigid_dynamic.get_global_pose();
				l_rigid_dynamic.set_global_pose(physics::bc_transform(l_rigid_dynamic_pose.get_position() + get_look_direction(), l_rigid_dynamic_pose.get_matrix3()));
				l_rigid_dynamic.set_linear_velocity(get_look_direction() * 2);
			}

			game::bc_actor_mark_for_checkpoint(m_weapon->m_actor);
		}

		m_weapon.reset();
	}

	bool bc_xbot_actor_controller::shoot_weapon(game::bc_network_client_id p_player_id) noexcept
	{
		if (!m_weapon.has_value())
		{
			return false;
		}

		const bool l_shoot = m_state_machine->shoot_weapon();
		if (!l_shoot)
		{
			return false;
		}

		const auto l_bullet = m_weapon->m_component->shoot(m_state_machine->m_state.m_look_direction, p_player_id);
		m_scene->add_bullet(l_bullet);

		return true;
	}

	void bc_xbot_actor_controller::set_weapon_rotation(bcFLOAT p_rotation) noexcept
	{
		m_state_machine->set_weapon_rotation(p_rotation);
	}

	void bc_xbot_actor_controller::enable_ragdoll()
	{
		if(m_grenade.has_value())
		{
			throw_grenade(m_grenade->m_actor);
			m_grenade.reset();
		}

		if(m_weapon.has_value())
		{
			drop_weapon();
		}

		m_state_machine->transfer_state<bc_xbot_ragdoll_state>();
		m_rigid_controller_component->reset_controller();
		m_human_ragdoll_component->set_enable(true);

		m_ragdoll_enabled = true;
	}

	void bc_xbot_actor_controller::enable_ragdoll(core::bc_string_view p_body_part_force, const core::bc_vector3f& p_force)
	{
		enable_ragdoll();
		m_human_ragdoll_component->add_force(p_body_part_force, p_force, true);
	}

	void bc_xbot_actor_controller::disable_ragdoll()
	{
		m_state_machine->transfer_state<bc_xbot_idle_state>();
		m_rigid_controller_component->reset_controller(create_px_controller());
		m_human_ragdoll_component->set_enable(false);

		m_ragdoll_enabled = false;
	}

	void bc_xbot_actor_controller::on_shape_hit(const physics::bc_ccontroller_shape_hit& p_hit)
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

		const auto l_actor_mass = m_rigid_controller_component->get_body().get_mass();
		const auto l_multiplier = m_state_machine->m_state.m_move_amount / m_state_machine->m_state.m_move_speed;
		l_rigid_dynamic.add_force(m_state_machine->m_state.m_move_direction * l_multiplier * l_actor_mass);
	}

	void bc_xbot_actor_controller::on_ccontroller_hit(const physics::bc_ccontroller_controller_hit& p_hit)
	{
	}

	core::bc_shared_ptr<game::bci_animation_job> bc_xbot_actor_controller::_create_animation_pipeline(const bcCHAR* p_idle_animation,
		const bcCHAR* p_idle_left_turn_animation,
		const bcCHAR* p_idle_right_turn_animation,
		const bcCHAR* p_weapon_idle_aiming_animation,
		const bcCHAR* p_weapon_idle_aiming_left_turn_animation,
		const bcCHAR* p_weapon_idle_aiming_right_turn_animation,
		const bcCHAR* p_walking_animation,
		const bcCHAR* p_walking_backward_animation,
		const bcCHAR* p_running_animation,
		const bcCHAR* p_running_backward_animation,
		const bcCHAR* p_weapon_walking_animation,
		const bcCHAR* p_weapon_walking_backward_animation,
		const bcCHAR* p_weapon_running_animation,
		const bcCHAR* p_weapon_running_backward_animation,
		const bcCHAR* p_grenade_throw_animation,
		const bcCHAR* p_weapon_shoot_animation)
	{
		auto& l_skeleton = *m_skinned_mesh_component->get_skeleton();
		auto& l_idle_animation = m_skinned_mesh_component->find_animation_throw(p_idle_animation);
		auto& l_idle_left_turn_animation = m_skinned_mesh_component->find_animation_throw(p_idle_left_turn_animation);
		auto& l_idle_right_turn_animation = m_skinned_mesh_component->find_animation_throw(p_idle_right_turn_animation);
		auto& l_weapon_idle_aiming_animation = m_skinned_mesh_component->find_animation_throw(p_weapon_idle_aiming_animation);
		auto& l_weapon_idle_aiming_left_turn_animation = m_skinned_mesh_component->find_animation_throw(p_weapon_idle_aiming_left_turn_animation);
		auto& l_weapon_idle_aiming_right_turn_animation = m_skinned_mesh_component->find_animation_throw(p_weapon_idle_aiming_right_turn_animation);
		auto& l_walking_animation = m_skinned_mesh_component->find_animation_throw(p_walking_animation);
		auto& l_walking_backward_animation = m_skinned_mesh_component->find_animation_throw(p_walking_backward_animation);
		auto& l_running_animation = m_skinned_mesh_component->find_animation_throw(p_running_animation);
		auto& l_running_backward_animation = m_skinned_mesh_component->find_animation_throw(p_running_backward_animation);
		auto& l_weapon_walking_animation = m_skinned_mesh_component->find_animation_throw(p_weapon_walking_animation);
		auto& l_weapon_walking_backward_animation = m_skinned_mesh_component->find_animation_throw(p_weapon_walking_backward_animation);
		auto& l_weapon_running_animation = m_skinned_mesh_component->find_animation_throw(p_weapon_running_animation);
		auto& l_weapon_running_backward_animation = m_skinned_mesh_component->find_animation_throw(p_weapon_running_backward_animation);
		auto& l_grenade_throw_animation = m_skinned_mesh_component->find_animation_throw(p_grenade_throw_animation);
		auto& l_weapon_shoot_animation = m_skinned_mesh_component->find_animation_throw(p_weapon_shoot_animation);

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
		auto l_idle_left_turn_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_idle_left_turn_animation));
		auto l_idle_right_turn_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_idle_right_turn_animation));
		auto l_weapon_idle_aiming_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_weapon_idle_aiming_animation));
		auto l_weapon_idle_aiming_left_turn_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_weapon_idle_aiming_left_turn_animation));
		auto l_weapon_idle_aiming_right_turn_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_weapon_idle_aiming_right_turn_animation));
		auto l_walking_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_walking_animation));
		auto l_walking_backward_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_walking_backward_animation));
		auto l_running_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_running_animation));
		auto l_running_backward_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_running_backward_animation));
		auto l_weapon_walking_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_weapon_walking_animation));
		auto l_weapon_walking_backward_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_weapon_walking_backward_animation));
		auto l_weapon_running_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_weapon_running_animation));
		auto l_weapon_running_backward_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_weapon_running_backward_animation));
		auto l_grenade_throw_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_grenade_throw_animation));
		auto l_weapon_shoot_sample_job = core::bc_make_shared<game::bc_sampling_animation_job>(game::bc_sampling_animation_job(l_skeleton, l_weapon_shoot_animation));
		
		auto l_idle_blending_job = core::bc_make_shared<game::bc_blending_animation_job>
		(
			game::bc_blending_animation_job
			(
				game::bc_blending_animation_job
				(
					l_skeleton, 
					{ 
						std::move(l_idle_left_turn_sample_job),
						std::move(l_idle_sample_job),
						std::move(l_idle_right_turn_sample_job)
					}
				)
			)
		);
		auto l_running_blending_job = core::bc_make_shared<game::bc_blending_animation_job>
		(
			game::bc_blending_animation_job
			(
				l_skeleton,
				{
					std::move(l_walking_sample_job),
					std::move(l_walking_backward_sample_job),
					std::move(l_running_sample_job),
					std::move(l_running_backward_sample_job)
				}
			)
		);
		auto l_weapon_idle_aiming_blending_job = core::bc_make_shared<game::bc_blending_animation_job>
		(
			game::bc_blending_animation_job
			(
				game::bc_blending_animation_job
				(
					l_skeleton, 
					{
						std::move(l_weapon_idle_aiming_left_turn_sample_job),
						std::move(l_weapon_idle_aiming_sample_job),
						std::move(l_weapon_idle_aiming_right_turn_sample_job)
					}
				)
			)
		);
		auto l_weapon_running_blending_job = core::bc_make_shared<game::bc_blending_animation_job>
		(
			game::bc_blending_animation_job
			(
				l_skeleton,
				{
					std::move(l_weapon_walking_sample_job),
					std::move(l_weapon_walking_backward_sample_job),
					std::move(l_weapon_running_sample_job),
					std::move(l_weapon_running_backward_sample_job)
				}
			)
		);
		auto l_without_weapon_blending_job = core::bc_make_shared<game::bc_blending_animation_job>
		(
			game::bc_blending_animation_job(l_skeleton, { l_idle_blending_job, l_running_blending_job })
		);
		auto l_with_weapon_blending_job = core::bc_make_shared<game::bc_blending_animation_job>
		(
			game::bc_blending_animation_job(l_skeleton, { l_weapon_idle_aiming_blending_job, l_weapon_running_blending_job })
		);
		auto l_main_local_job = core::bc_make_shared<game::bc_execute_once_animation_job<game::bc_execute_one_animation_job>>
		(
			game::bc_execute_once_animation_job(game::bc_execute_one_animation_job(l_skeleton, { l_without_weapon_blending_job, l_with_weapon_blending_job }))
		);
		auto l_grenade_throw_partial_blending_job = core::bc_make_shared<game::bc_partial_blending_animation_job>
		(
			game::bc_partial_blending_animation_job(l_skeleton, l_main_local_job, std::move(l_grenade_throw_sample_job), m_upper_body_chain[0].c_str())
		);
		auto l_grenade_throw_blending_job = core::bc_make_shared<game::bc_blending_animation_job>
		(
			game::bc_blending_animation_job(l_skeleton, { l_main_local_job, l_grenade_throw_partial_blending_job })
		);
		auto l_weapon_shoot_job = core::bc_make_shared<game::bc_additive_blending_animation_job>
		(
			game::bc_additive_blending_animation_job(l_skeleton, l_grenade_throw_blending_job, l_weapon_shoot_sample_job, nullptr)
		);
		auto l_local_to_model_job = core::bc_make_shared<game::bc_local_to_model_animation_job>
		(
			game::bc_local_to_model_animation_job(l_weapon_shoot_job, m_skinned_mesh_component->get_mesh(), m_skinned_mesh_component->get_model_transforms())
		);
		auto l_model_to_skinning_job = core::bc_make_shared<game::bc_model_to_skinning_animation_job>
		(
			game::bc_model_to_skinning_animation_job(l_local_to_model_job, m_skinned_mesh_component->get_world_transforms())
		);
		auto l_weapon_ik_job = core::bc_make_shared<bc_xbot_weapon_ik_animation_job>
		(
			bc_xbot_weapon_ik_animation_job
			(
				m_local_forward,
				l_weapon_shoot_job,
				l_local_to_model_job,
				m_right_hand_chain[0].second,
				m_right_hand_chain[1].second,
				m_right_hand_chain[2].second,
				m_left_hand_chain[0].second,
				m_left_hand_chain[1].second,
				m_left_hand_chain[2].second,
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
			bc_xbot_update_animation_job(m_actor, *this, *m_skinned_mesh_component, l_model_to_skinning_job)
		);

		bcFLOAT l_grenade_throw_blending_weights[] = { 1,0 };
		l_grenade_throw_blending_job->set_weights(&l_grenade_throw_blending_weights[0]);
		l_grenade_throw_partial_blending_job->get_layer2()->set_play_mode(game::bc_animation_play_mode::once_disable);
		l_grenade_throw_partial_blending_job->set_enabled(false);
		l_weapon_shoot_sample_job->set_play_mode(game::bc_animation_play_mode::once_disable);
		l_weapon_shoot_job->set_enabled(false);

		return game::bc_animation_job_builder()
			.start_with(game::bc_tag_animation_job(std::move(l_idle_blending_job)), "idle_blending")
			.then(game::bc_tag_animation_job(std::move(l_running_blending_job)), "running_blending")
			.then(game::bc_tag_animation_job(std::move(l_weapon_idle_aiming_blending_job)), "weapon_idle_aiming_blending")
			.then(game::bc_tag_animation_job(std::move(l_weapon_running_blending_job)), "weapon_running_blending")
			.then(game::bc_tag_animation_job(std::move(l_without_weapon_blending_job)), "without_weapon_blending")
			.then(game::bc_tag_animation_job(std::move(l_with_weapon_blending_job)), "with_weapon_blending")
			.then(std::move(l_main_local_job), "main_local")
			.then(std::move(l_grenade_throw_blending_job), "grenade_throw_blending")
			.then(std::move(l_weapon_shoot_job), "weapon_shot")
			.then(std::move(l_local_to_model_job))
			.then(std::move(l_weapon_ik_job), "weapon_ik")
			.then(std::move(l_aim_job), "aim")
			.then(std::move(l_model_to_skinning_job))
			.end_with(std::move(l_actor_update_job))
			.build();
	}

	void bc_xbot_actor_controller::_update_px_move(const platform::bc_clock::update_param& p_clock, const core::bc_vector3f& p_move_vector)
	{
		auto l_px_controller_pre_filter = physics::bc_scene_query_pre_filter_callback
		(
			[this](const physics::bc_scene_query_pre_filter_data& p_filter_data)
			{
				const auto l_actor = m_physics_system->get_game_actor(p_filter_data.m_actor);
				if
				(
					l_actor == m_actor || 
					(m_weapon.has_value() && l_actor == m_weapon->m_actor) ||
					(m_grenade.has_value() && l_actor == m_grenade->m_actor)
				)
				{
					return physics::bc_query_hit_type::none;
				}

				return physics::bc_query_hit_type::block;
			}
		);

		auto& l_px_scene = m_scene->get_px_scene();
		auto l_px_controller = m_rigid_controller_component->get_controller();

		{
			physics::bc_scene_lock l_lock(&l_px_scene);

			const auto l_displacement = (p_move_vector + (l_px_scene.get_gravity() * m_bound_box_max_side_length)) * p_clock.m_elapsed_second;
			l_px_controller.move(l_displacement, p_clock, &l_px_controller_pre_filter);
			m_position = l_px_controller.get_foot_position();
		}
	}

	void bc_xbot_actor_controller::_update_px_position(const core::bc_vector3f& p_position)
	{
		auto& l_px_scene = m_scene->get_px_scene();
		auto l_px_controller = m_rigid_controller_component->get_controller();
		
		{
			physics::bc_scene_lock l_lock(&l_px_scene);

			m_position = p_position - m_local_origin;
			l_px_controller.set_foot_position(m_position);
		}
	}
	
	void bc_xbot_actor_controller::_update_world_transform()
	{
		core::bc_matrix4f l_world_transform;
		core::bc_matrix3f l_rotation;

		const auto l_move_direction_sign = m_state_machine->m_state.m_look_direction.dot(m_state_machine->m_state.m_move_direction) >= -0.01f ? 1.f : -1.f;
		if constexpr (graphic::bc_render_api_info::use_left_handed())
		{
			l_rotation.rotation_between_two_vector_lh(get_local_forward(), m_state_machine->m_state.m_move_direction * l_move_direction_sign);
		}
		else
		{
			l_rotation.rotation_between_two_vector_rh(get_local_forward(), m_state_machine->m_state.m_move_direction * l_move_direction_sign);
		}
		
		l_world_transform.make_identity();
		l_world_transform.set_rotation(l_rotation);
		l_world_transform.set_translation(m_position);

		game::bc_animation_job_helper::set_skinning_world_transform(*m_animation_pipeline, l_world_transform);
		
		m_actor.add_event(game::bc_world_transform_actor_event(l_world_transform, game::bc_transform_event_type::physics));
		m_actor.mark_for_double_update();
	}

	void bc_xbot_actor_controller::_update_sounds()
	{
		if(m_running_sound.is_valid())
		{
			auto l_move_velocity = std::max
			({
				m_forward_velocity.get_value(),
				m_backward_velocity.get_value(),
				m_left_velocity.get_value(),
				m_right_velocity.get_value()
			});
			l_move_velocity = std::max(0.f, l_move_velocity - m_walk_velocity.get_value());

			m_running_sound.set_volume(l_move_velocity);
		}
	}

	core::bc_matrix4f bc_xbot_actor_controller::_calculate_weapon_aim_transform()
	{
		const auto* l_model_job = game::bc_animation_job_helper::find_job<game::bc_local_to_model_animation_job>(*m_state_machine->get_active_animation());
		const auto* l_skinning_job = game::bc_animation_job_helper::find_job<game::bc_model_to_skinning_animation_job>(*m_state_machine->get_active_animation());
		
		const auto l_weapon_up_rotation_ms = bc_matrix3f_rotation_between_two_vector_checked(m_weapon->m_local_up, m_right_hand_weapon_up);
		const auto l_weapon_forward_rotation_ms = bc_matrix3f_rotation_between_two_vector_checked(m_weapon->m_local_forward, m_right_hand_weapon_forward);
		const auto l_weapon_aim_up_rotation_ms = bc_matrix3f_rotation_between_two_vector_checked(m_weapon->m_local_up, core::bc_vector3f::up());
		const auto l_weapon_aim_forward_rotation_ms = bc_matrix3f_rotation_between_two_vector_checked(m_weapon->m_local_forward, m_state_machine->m_state.m_look_direction);
		const auto l_weapon_aim_right_rotation_ms = bc_matrix3f_rotation_euler(m_weapon->m_local_right, core::bc_to_radian(m_state_machine->m_state.m_weapon_rotation));

		auto l_weapon_transform_ms = core::bc_matrix4f::identity();
		l_weapon_transform_ms.set_rotation(l_weapon_up_rotation_ms * l_weapon_forward_rotation_ms);
		const auto l_weapon_main_hand_offset_ms = l_weapon_transform_ms.get_rotation() * m_weapon->m_main_hand_offset;
		l_weapon_transform_ms.set_translation(l_weapon_main_hand_offset_ms);
		
		auto l_main_hand_transform_ms = l_model_job->get_transforms()[m_right_hand_chain[2].first];
		l_main_hand_transform_ms.make_neutralize_scale();
		const auto l_main_hand_offset_ms = l_main_hand_transform_ms.get_rotation() * m_right_hand_attachment_offset;
		l_main_hand_transform_ms.set_translation(l_main_hand_transform_ms.get_translation() + l_main_hand_offset_ms);
		
		auto l_main_hand_transform_ws = l_weapon_transform_ms * l_main_hand_transform_ms * l_skinning_job->get_world();

		constexpr auto l_aim_lock = .7f;
		const auto l_weapon_aim_rotation = l_weapon_aim_right_rotation_ms * l_weapon_aim_up_rotation_ms * l_weapon_aim_forward_rotation_ms;
		const auto l_main_hand_final_rotation = l_main_hand_transform_ws.get_rotation() * (1 - l_aim_lock) + l_weapon_aim_rotation * l_aim_lock;

		l_main_hand_transform_ws.set_rotation(l_main_hand_final_rotation);

		return l_main_hand_transform_ws;
	}

	core::bc_matrix4f bc_xbot_actor_controller::_calculate_attachment_transform(bcUINT32 p_attached_node_index,
		const core::bc_vector3f& p_attached_node_local_up,
		const core::bc_vector3f& p_attached_node_local_forward,
		const core::bc_vector3f& p_attached_node_local_offset,
		const core::bc_vector3f& p_attachment_local_up,
		const core::bc_vector3f& p_attachment_local_forward,
		const core::bc_vector3f& p_attachment_local_offset)
	{
		const auto* l_model_job = game::bc_animation_job_helper::find_job<game::bc_local_to_model_animation_job>(*m_state_machine->get_active_animation());
		const auto* l_skinning_job = game::bc_animation_job_helper::find_job<game::bc_model_to_skinning_animation_job>(*m_state_machine->get_active_animation());
		
		auto l_attached_node_transform_ms = l_model_job->get_transforms()[p_attached_node_index];
		l_attached_node_transform_ms.make_neutralize_scale();
		const auto l_attached_node_offset_ms = l_attached_node_transform_ms.get_rotation() * p_attached_node_local_offset;
		l_attached_node_transform_ms.set_translation(l_attached_node_transform_ms.get_translation() + l_attached_node_offset_ms);

		const auto l_attachment_up_rotation_ms = bc_matrix3f_rotation_between_two_vector(p_attachment_local_up, p_attached_node_local_up);
		const auto l_attachment_forward_rotation_ms = bc_matrix3f_rotation_between_two_vector(p_attachment_local_forward, p_attached_node_local_forward);

		auto l_attachment_transform_ms = core::bc_matrix4f::identity();
		l_attachment_transform_ms.set_rotation(l_attachment_up_rotation_ms * l_attachment_forward_rotation_ms);
		const auto l_attachment_transform_offset_ms = l_attachment_transform_ms.get_rotation() * p_attachment_local_offset;
		l_attachment_transform_ms.set_translation(l_attachment_transform_offset_ms);
		
		auto l_attachment_transform_ws = l_attachment_transform_ms * l_attached_node_transform_ms * l_skinning_job->get_world();
		
		return l_attachment_transform_ws;
	}
}