// [03/11/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Animation/Job/bcBlendingAnimationJob.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/blending_job.h"

namespace black_cat
{
	namespace game
	{
		bc_blending_animation_job::bc_blending_animation_job(bc_animation_skeleton& p_skeleton, std::initializer_list<const bci_local_transform_animation_job*> p_layers)
			: bci_local_transform_animation_job(p_skeleton),
			m_layers(p_layers.size()),
			m_locals(m_skeleton->get_native_handle().num_soa_joints())
		{
			std::transform
			(
				std::begin(p_layers),
				std::end(p_layers),
				std::begin(m_layers),
				[](const bci_local_transform_animation_job* p_sample)
				{
					return std::make_pair(p_sample, 0.f);
				}
			);
		}

		void bc_blending_animation_job::set_weights(const bcFLOAT* p_weights) noexcept
		{
			for (bcUINT32 l_i = 0; l_i < m_layers.size(); ++l_i)
			{
				m_layers[l_i].second = p_weights[l_i];
			}
		}

		bool bc_blending_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
		{
			core::bc_vector_frame<ozz::animation::BlendingJob::Layer> l_layers(m_layers.size());

			for(bcUINT32 l_i = 0; l_i < m_layers.size(); ++l_i)
			{
				auto& l_layer = m_layers[l_i];
				l_layers[l_i].transform = { &*l_layer.first->get_local_transforms().begin(), l_layer.first->get_local_transforms().size() };
				l_layers[l_i].weight = l_layer.second;
			}

			ozz::animation::BlendingJob l_job;
			l_job.layers = ozz::make_span(l_layers);
			l_job.bind_pose = m_skeleton->get_native_handle().joint_bind_poses();
			l_job.output = { &*m_locals.begin(), m_locals.size() };

			return l_job.Run();
		}
	}	
}