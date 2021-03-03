// [01/04/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/Object/Animation/bcAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		class bc_sequence_animation_job : public bci_animation_job
		{
		public:
			using execute_callback = core::bc_delegate< void() >;
			
		public:
			bc_sequence_animation_job(bci_animation_job** p_begin, bcSIZE p_count);
			
			bc_sequence_animation_job(bci_animation_job** p_begin, bcSIZE p_count, execute_callback p_callback);

			bc_sequence_animation_job(bc_sequence_animation_job&&) noexcept;

			~bc_sequence_animation_job();

			bc_sequence_animation_job& operator=(bc_sequence_animation_job&&) noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;
		
		private:
			core::bc_vector_movable<bci_animation_job*> m_jobs;
			execute_callback m_callback;
		};

		inline bc_sequence_animation_job::bc_sequence_animation_job(bci_animation_job** p_begin, bcSIZE p_count)
			: bc_sequence_animation_job(p_begin, p_count, execute_callback())
		{
		}

		inline bc_sequence_animation_job::bc_sequence_animation_job(bci_animation_job** p_begin, bcSIZE p_count, execute_callback p_callback)
			: bci_animation_job((*p_begin)->get_skeleton()),
			m_callback(std::move(p_callback))
		{
			BC_ASSERT(p_count > 0);

			m_jobs.resize(p_count);
			std::memcpy(m_jobs.data(), p_begin, sizeof(bci_animation_job**) * p_count);
		}

		inline bc_sequence_animation_job::bc_sequence_animation_job(bc_sequence_animation_job&&) noexcept = default;

		inline bc_sequence_animation_job::~bc_sequence_animation_job() = default;

		inline bc_sequence_animation_job& bc_sequence_animation_job::operator=(bc_sequence_animation_job&&) noexcept = default;

		inline bool bc_sequence_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
		{
			for(auto& l_job : m_jobs)
			{
				if(!l_job->run(p_clock))
				{
					break;
				}
			}

			if(m_callback.is_valid())
			{
				m_callback();
			}

			return true;
		}
	}	
}