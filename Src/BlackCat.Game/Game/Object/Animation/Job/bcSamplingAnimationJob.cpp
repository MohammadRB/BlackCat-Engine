// [01/05/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Animation/Job/bcSamplingAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		bc_sampling_animation_job::bc_sampling_animation_job(bc_animation_skeleton& p_skeleton, bc_skeleton_animation& p_animation)
			: bci_local_transform_animation_job(p_skeleton),
			m_animation(&p_animation),
			m_play_mode(bc_animation_play_mode::loop),
			m_local_time(0),
			m_speed(1),
			m_sampling_cache(core::bc_make_unique<ozz::animation::SamplingCache>(core::bc_alloc_type::unknown, 16)),
			m_locals(m_skeleton->get_native_handle().num_soa_joints())
		{
			m_sampling_cache->Resize(m_skeleton->get_num_joints());
		}

		bcFLOAT bc_sampling_animation_job::advance_time(const platform::bc_clock::update_param& p_clock)
		{
			m_local_time += p_clock.m_elapsed_second * m_speed;
			const auto l_animation_duration = m_animation->get_duration();

			switch (m_play_mode)
			{
			case bc_animation_play_mode::once_disable:
			{
				if (m_local_time > l_animation_duration)
				{
					m_local_time = 0;
					m_enabled = false;
				}
				break;
			}
			case bc_animation_play_mode::loop:
			{
				const auto l_local_time = m_local_time / l_animation_duration;
				m_local_time = (l_local_time - std::floorf(l_local_time)) * l_animation_duration;
				break;
			}
			default:
			{
				BC_ASSERT(false);
				break;
			}
			}

			return m_local_time / l_animation_duration;
		}
		
		bool bc_sampling_animation_job::run(const platform::bc_clock::update_param& p_clock)
		{
			if(!m_enabled)
			{
				return true;
			}
			
			const auto l_local_time = advance_time(p_clock);

			ozz::animation::SamplingJob l_sampling_job;
			l_sampling_job.animation = &m_animation->get_native_handle();
			l_sampling_job.cache = m_sampling_cache.get();
			l_sampling_job.ratio = l_local_time;
			l_sampling_job.output = { (&*m_locals.begin()), m_locals.size() };

			return l_sampling_job.Run();
		}
	}
}