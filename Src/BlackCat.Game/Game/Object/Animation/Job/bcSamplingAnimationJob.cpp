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
			m_sampling_cache(core::bc_make_unique< ozz::animation::SamplingCache >()),
			m_locals(m_skeleton->get_native_handle().num_soa_joints())
		{
			m_sampling_cache->Resize(m_skeleton->get_num_joints());
		}

		bool bc_sampling_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
		{
			m_local_time += p_clock.m_elapsed_second;
			auto l_local_time = m_local_time / m_animation->get_duration();
			l_local_time = l_local_time - std::floorf(l_local_time);

			ozz::animation::SamplingJob l_sampling_job;
			l_sampling_job.animation = &m_animation->get_native_handle();
			l_sampling_job.cache = m_sampling_cache.get();
			l_sampling_job.ratio = l_local_time;
			l_sampling_job.output = { (&*m_locals.begin()), m_locals.size() };

			return l_sampling_job.Run();
		}
	}
}