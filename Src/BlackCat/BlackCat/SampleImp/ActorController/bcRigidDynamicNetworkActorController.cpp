// [09/30/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/bcUtility.h"
#include "BlackCat/SampleImp/ActorController/bcRigidDynamicNetworkActorController.h"

namespace black_cat
{
	core::bc_logger_output_stream& operator<<(core::bc_logger_output_stream& p_stream, const core::bc_vector3f& p_vector)
	{
		return p_stream << p_vector.x << " " << p_vector.y << " " << p_vector.z;
	}
	
	void bc_rigid_dynamic_network_actor_controller::load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
		//core::bc_log(core::bc_log_type::debug) << core::bc_only_file << "load origin: " << core::bc_lend;
	}

	void bc_rigid_dynamic_network_actor_controller::load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
		json_parse::bc_load(p_context.m_parameters, "pos", m_network_position);
		json_parse::bc_load(p_context.m_parameters, "rot", m_network_euler_rotation);

		m_network_component->add_extrapolating_value("pos", m_network_position);
		m_network_component->add_extrapolating_value("rot", m_network_euler_rotation);

		// Advance properties based on extrapolated values
		/*const auto l_extrapolated_pos = m_network_component->get_extrapolated_value("pos").second;
		const auto l_extrapolated_rot = m_network_component->get_extrapolated_value("rot").second;
		m_network_position += l_extrapolated_pos;
		m_network_euler_rotation += l_extrapolated_rot;*/

		//core::bc_log(core::bc_log_type::debug) << "load pos: " << m_network_position.x << " " << m_network_position.y << " " << m_network_position.z << core::bc_lend;

		auto l_transform = core::bc_matrix4f::identity();
		l_transform.set_translation(m_network_position);
		l_transform.set_rotation(bc_matrix3f_rotation_zyx(m_network_euler_rotation));

		p_context.m_actor.add_event(game::bc_world_transform_actor_event(l_transform, game::bc_transform_event_type::network));

		//core::bc_log(core::bc_log_type::debug) << core::bc_only_file << "load replicated: " << m_network_position << core::bc_lend;
	}

	void bc_rigid_dynamic_network_actor_controller::write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		auto* l_mediate_component = p_context.m_actor.get_component<game::bc_mediate_component>();
		const auto& l_world_transform = l_mediate_component->get_world_transform();

		json_parse::bc_write(p_context.m_parameters, "pos", l_world_transform.get_translation());
		json_parse::bc_write(p_context.m_parameters, "rot", bc_matrix4f_decompose_to_euler_angles(l_world_transform));

		const auto l_world_translation = l_world_transform.get_translation();
		//core::bc_log(core::bc_log_type::debug) << core::bc_only_file << "write origin: " << l_world_translation << core::bc_lend;
	}

	void bc_rigid_dynamic_network_actor_controller::write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		json_parse::bc_write(p_context.m_parameters, "pos", m_network_position);
		json_parse::bc_write(p_context.m_parameters, "rot", m_network_euler_rotation);

		//core::bc_log(core::bc_log_type::debug) << core::bc_only_file << "write replicated: " << m_network_position << core::bc_lend;
	}

	void bc_rigid_dynamic_network_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
	}

	void bc_rigid_dynamic_network_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
	}

	void bc_rigid_dynamic_network_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
		if (!m_added_to_scene || p_context.m_is_double_update)
		{
			return;
		}

		const auto l_extrapolated_pos = m_network_component->get_extrapolated_value("pos", p_context.m_clock).second;
		//const auto l_extrapolated_rot = m_network_component->get_extrapolated_value("rot", p_context.m_clock).second;
		m_network_position += l_extrapolated_pos;
		//m_network_euler_rotation += l_extrapolated_rot;

		/*if(l_extrapolated_pos.magnitude() > 0.0)
		{
			core::bc_log(core::bc_log_type::debug) << core::bc_only_file << "update extrapolated position: " << l_extrapolated_pos << core::bc_lend;
		}*/
		
		auto l_transform = core::bc_matrix4f::identity();
		l_transform.set_translation(m_network_position);
		l_transform.set_rotation(bc_matrix3f_rotation_zyx(m_network_euler_rotation));

		p_context.m_actor.add_event(game::bc_world_transform_actor_event(l_transform, game::bc_transform_event_type::network));
		p_context.m_actor.mark_for_double_update();
	}

	void bc_rigid_dynamic_network_actor_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		m_added_to_scene = false;
	}
	
	void bc_rigid_dynamic_network_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
	}
}