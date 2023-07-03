// [31/03/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcString.h"
#include "Core/Utility/bcLogger.h"
#include "Game/System/Animation/Job/bcAdditiveBlendingAnimationJob.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/blending_job.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/skeleton_utils.h"

namespace black_cat
{
	namespace game
	{
		bc_additive_blending_animation_job::bc_additive_blending_animation_job(bc_animation_skeleton& p_skeleton,
			core::bc_shared_ptr<bci_local_transform_animation_job> p_main_layer,
			core::bc_shared_ptr<bc_sampling_animation_job> p_additive_layer,
			const bcCHAR* p_additive_layer_root_joint)
			: bci_local_transform_animation_job(p_skeleton),
			m_main_layer_weight(1),
			m_additive_layer_weight(1),
			m_main_layer(std::move(p_main_layer)),
			m_additive_layer(std::move(p_additive_layer)),
			m_additive_layer_weights(m_skeleton->get_native_handle().num_soa_joints(), ozz::math::simd_float4::one()),
			m_locals(m_skeleton->get_native_handle().num_soa_joints())
		{
			if(p_additive_layer_root_joint)
			{
				const auto l_joint = m_skeleton->find_joint_by_name(p_additive_layer_root_joint);

				if (!l_joint.second)
				{
					const auto l_msg = bcL("No joint were found with name ") + core::bc_to_estring_frame(p_additive_layer_root_joint);
					core::bc_get_service<core::bc_logger>()->log_debug(l_msg.c_str());
					return;
				}

				auto l_weight_setter = [](decltype(m_additive_layer_weights)& p_weights, bcFLOAT p_weight_value)
				{
					return [&p_weights, p_weight_value](bcINT p_joint, bcINT p_parent)
					{
						auto& l_soa_weight = p_weights[p_joint / 4];
						const auto l_weight_value = ozz::math::simd_float4::Load1(p_weight_value);
						l_soa_weight = ozz::math::SetI(l_soa_weight, l_weight_value, p_joint % 4);
					};
				};

				ozz::animation::IterateJointsDF(m_skeleton->get_native_handle(), l_weight_setter(m_additive_layer_weights, 1.f), l_joint.first);
			}
		}

		bool bc_additive_blending_animation_job::run(const platform::bc_clock::update_param& p_clock)
		{
			if (!m_enabled)
			{
				return true;
			}

			const bool l_is_valid = m_additive_layer->run(p_clock);
			if (!l_is_valid)
			{
				return false;
			}

			ozz::animation::BlendingJob::Layer l_layers[1];
			l_layers[0].transform = ozz::make_span(m_main_layer->get_local_transforms());
			l_layers[0].weight = m_main_layer_weight;

			ozz::animation::BlendingJob::Layer l_additive_layers[1];
			l_additive_layers[0].transform = ozz::make_span(m_additive_layer->get_local_transforms());
			l_additive_layers[0].weight = m_additive_layer_weight;
			l_additive_layers[0].joint_weights = ozz::make_span(m_additive_layer_weights);
			
			ozz::animation::BlendingJob l_job;
			l_job.layers = ozz::make_span(l_layers);
			l_job.additive_layers = ozz::make_span(l_additive_layers);
			l_job.rest_pose = m_skeleton->get_native_handle().joint_rest_poses();
			l_job.output = ozz::make_span(m_locals);

			return l_job.Run();
		}
	}
}