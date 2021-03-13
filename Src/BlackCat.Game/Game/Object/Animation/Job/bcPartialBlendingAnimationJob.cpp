// [03/12/2021 MRB]

#include "Game/GamePCH.h"

#include <cstring>
#include "Core/Container/bcString.h"
#include "Core/Utility/bcLogger.h"
#include "Game/Object/Animation/Job/bcPartialBlendingAnimationJob.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/blending_job.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/skeleton_utils.h"

namespace black_cat
{
	namespace game
	{
		bc_partial_blending_animation_job::bc_partial_blending_animation_job(bc_animation_skeleton& p_skeleton,
			const bci_local_transform_animation_job& p_layer1,
			const bci_local_transform_animation_job& p_layer2,
			const bcCHAR* p_layer_2_root_joint)
			: bci_local_transform_animation_job(p_skeleton),
			m_layer1(&p_layer1),
			m_layer2(&p_layer2),
			m_layer1_weights(m_skeleton->get_native_handle().num_soa_joints(), ozz::math::simd_float4::one()),
			m_layer2_weights(m_skeleton->get_native_handle().num_soa_joints(), ozz::math::simd_float4::zero())
		{
			bool l_joint_found = false;
			bcINT32 l_joint_index = 0;
			
			for(auto& l_joint_name : m_skeleton->get_bone_names())
			{
				if(std::strstr(l_joint_name, p_layer_2_root_joint))
				{
					l_joint_found = true;
					break;
				}

				++l_joint_index;
			}

			if(!l_joint_found)
			{
				const auto l_msg = bcL("No joint were found with name ") + core::bc_to_estring_frame(p_layer_2_root_joint);
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
			
			ozz::animation::IterateJointsDF(m_skeleton->get_native_handle(), l_weight_setter(m_layer1_weights, 0.f), l_joint_index);
			ozz::animation::IterateJointsDF(m_skeleton->get_native_handle(), l_weight_setter(m_layer2_weights, 1.f), l_joint_index);
		}

		bool bc_partial_blending_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
		{
			ozz::animation::BlendingJob::Layer l_layers[2];
			l_layers[0].transform = { &*m_layer1->get_local_transforms().begin(), m_layer1->get_local_transforms().size() };
			l_layers[0].weight = 1;
			l_layers[0].joint_weights = ozz::make_span(m_layer1_weights);
			l_layers[1].transform = { &*m_layer2->get_local_transforms().begin(), m_layer2->get_local_transforms().size() };
			l_layers[1].weight = 1;
			l_layers[1].joint_weights = ozz::make_span(m_layer2_weights);
			
			ozz::animation::BlendingJob l_job;
			l_job.layers = ozz::make_span(l_layers);
			l_job.bind_pose = m_skeleton->get_native_handle().joint_bind_poses();
			l_job.output = { &*m_locals.begin(), m_locals.size() };

			return l_job.Run();
		}
	}	
}