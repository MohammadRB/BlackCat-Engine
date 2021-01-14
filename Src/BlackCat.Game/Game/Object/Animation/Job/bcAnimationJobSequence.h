// [01/04/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/Object/Animation/bcAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		class bc_animation_job_sequence : public bci_animation_job
		{
		public:
			bc_animation_job_sequence(bci_animation_job** p_begin, bcSIZE p_count);

			bc_animation_job_sequence(bc_animation_job_sequence&&) noexcept;

			~bc_animation_job_sequence();

			bc_animation_job_sequence& operator=(bc_animation_job_sequence&&) noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;
		
		private:
			core::bc_vector_movable<bci_animation_job*> m_jobs;
		};

		inline bc_animation_job_sequence::bc_animation_job_sequence(bci_animation_job** p_begin, bcSIZE p_count)
			: bci_animation_job((*p_begin)->get_skeleton())
		{
			BC_ASSERT(p_count > 0);
			
			m_jobs.resize(p_count);
			std::memcpy(m_jobs.data(), p_begin, sizeof(bci_animation_job**) * p_count);
		}

		inline bc_animation_job_sequence::bc_animation_job_sequence(bc_animation_job_sequence&&) noexcept = default;

		inline bc_animation_job_sequence::~bc_animation_job_sequence() = default;

		inline bc_animation_job_sequence& bc_animation_job_sequence::operator=(bc_animation_job_sequence&&) noexcept = default;

		inline bool bc_animation_job_sequence::run(const core_platform::bc_clock::update_param& p_clock)
		{
			for(auto& l_job : m_jobs)
			{
				if(!l_job->run(p_clock))
				{
					break;
				}
			}

			return true;
		}
	}	
}