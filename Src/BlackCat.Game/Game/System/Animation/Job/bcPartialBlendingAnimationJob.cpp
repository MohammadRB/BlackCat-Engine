// [03/12/2021 MRB]

#include "Game/GamePCH.h"

#include <cstring>
#include "Core/Container/bcString.h"
#include "Core/Utility/bcLogger.h"
#include "Game/System/Animation/Job/bcPartialBlendingAnimationJob.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/blending_job.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/skeleton_utils.h"

namespace black_cat
{
	namespace game
	{
		bc_partial_blending_animation_job::bc_partial_blending_animation_job(bc_animation_skeleton& p_skeleton,
			core::bc_shared_ptr<bci_local_transform_animation_job> p_layer1,
			core::bc_shared_ptr<bc_sampling_animation_job> p_layer2,
			const bcCHAR* p_layer_2_root_joint)
			: bci_local_transform_animation_job(p_skeleton),
			m_layer1_weight(1),
			m_layer2_weight(1),
			m_layer1(std::move(p_layer1)),
			m_layer2(std::move(p_layer2)),
			m_layer1_weights(m_skeleton->get_native_handle().num_soa_joints(), ozz::math::simd_float4::one()),
			m_layer2_weights(m_skeleton->get_native_handle().num_soa_joints(), ozz::math::simd_float4::zero()),
			m_locals(m_skeleton->get_native_handle().num_soa_joints())
		{
			const auto l_joint = m_skeleton->find_joint_by_name(p_layer_2_root_joint);

			if (!l_joint.second)
			{
				const auto l_msg = bcL("No joint were found with name ") + core::bc_to_estring_frame
						(p_layer_2_root_joint);
				core::bc_get_service<core::bc_logger>()->log_debug(l_msg.c_str());
				return;
			}

			auto l_weight_setter = [](decltype(m_layer1_weights)& p_weights, bcFLOAT p_weight_value)
			{
				return [&p_weights, p_weight_value](bcINT p_joint, bcINT p_parent)
				{
					auto& l_soa_weight = p_weights[p_joint / 4];
					const auto l_weight_value = ozz::math::simd_float4::Load1(p_weight_value);
					l_soa_weight = ozz::math::SetI(l_soa_weight, l_weight_value, p_joint % 4);
				};
			};

			ozz::animation::IterateJointsDF(m_skeleton->get_native_handle(), l_weight_setter(m_layer1_weights, 0.f), l_joint.first);
			ozz::animation::IterateJointsDF(m_skeleton->get_native_handle(), l_weight_setter(m_layer2_weights, 1.f), l_joint.first);
		}

		bool bc_partial_blending_animation_job::run(const platform::bc_clock::update_param& p_clock)
		{
			if(!m_enabled)
			{
				return true;
			}
			
			ozz::animation::BlendingJob::Layer l_layers[2];
			l_layers[0].transform = ozz::make_span(m_layer1->get_local_transforms());
			l_layers[0].weight = m_layer1_weight;
			l_layers[0].joint_weights = ozz::make_span(m_layer1_weights);
			l_layers[1].transform = ozz::make_span(m_layer2->get_local_transforms());
			l_layers[1].weight = m_layer2_weight;
			l_layers[1].joint_weights = ozz::make_span(m_layer2_weights);

			const bool l_is_valid = m_layer2->run(p_clock);
			if(!l_is_valid)
			{
				return false;
			}
			
			ozz::animation::BlendingJob l_job;
			l_job.layers = ozz::make_span(l_layers);
			l_job.rest_pose = m_skeleton->get_native_handle().joint_rest_poses();
			l_job.output = ozz::make_span(m_locals);

			return l_job.Run();
		}
	}	
}