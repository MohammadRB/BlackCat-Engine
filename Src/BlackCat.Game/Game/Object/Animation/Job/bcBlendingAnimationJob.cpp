// [03/11/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Animation/Job/bcBlendingAnimationJob.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/blending_job.h"

namespace black_cat
{
	namespace game
	{
		bc_blending_animation_job::bc_blending_animation_job(bc_animation_skeleton& p_skeleton, std::initializer_list<core::bc_shared_ptr<bc_sampling_animation_job>> p_layers)
			: bci_local_transform_animation_job(p_skeleton),
			m_layers(p_layers.size()),
			m_locals(m_skeleton->get_native_handle().num_soa_joints())
		{
			std::transform
			(
				std::begin(p_layers),
				std::end(p_layers),
				std::begin(m_layers),
				[](const core::bc_shared_ptr<bc_sampling_animation_job>& p_sample)
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

		void bc_blending_animation_job::set_local_times(const bcFLOAT* p_time) noexcept
		{
			for (bcUINT32 l_i = 0; l_i < m_layers.size(); ++l_i)
			{
				if(p_time[l_i] < 0)
				{
					continue;
				}
				
				m_layers[l_i].first->set_local_time(p_time[l_i]);
			}
		}

		bool bc_blending_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
		{
			if (!m_enabled)
			{
				return true;
			}
			
			core::bc_vector_frame<ozz::animation::BlendingJob::Layer> l_layers(m_layers.size());
			bool l_is_valid = true;
			
			for(bcUINT32 l_i = 0; l_i < m_layers.size(); ++l_i)
			{
				auto& l_layer_job = m_layers[l_i];
				auto& l_layer = l_layers[l_i];
				l_layer.transform = ozz::make_span(l_layer_job.first->get_local_transforms());
				l_layer.weight = l_layer_job.second;

				if(l_layer.weight > 0.f)
				{
					l_is_valid = l_is_valid && l_layer_job.first->run(p_clock);
					
					if(!l_is_valid)
					{
						return false;
					}
				}
			}

			ozz::animation::BlendingJob l_job;
			l_job.layers = ozz::make_span(l_layers);
			l_job.bind_pose = m_skeleton->get_native_handle().joint_bind_poses();
			l_job.output = ozz::make_span(m_locals);

			return l_job.Run();
		}
	}	
}