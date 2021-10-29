// [01/04/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcSpan.h"
#include "Game/Object/Animation/bcAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		struct bc_sequence_animation
		{
			const bcCHAR* m_name;
			core::bc_shared_ptr<bci_animation_job> m_animation;
		};
				
		class bc_sequence_animation_job : public bci_animation_job
		{
		public:
			explicit bc_sequence_animation_job(core::bc_span<bc_sequence_animation>& p_animations);
			
			bc_sequence_animation_job(bc_sequence_animation_job&&) noexcept;

			~bc_sequence_animation_job() override;

			bc_sequence_animation_job& operator=(bc_sequence_animation_job&&) noexcept;

			std::tuple<const bcCHAR*, bci_animation_job*> at(bcUINT32 p_position) noexcept;
			
			std::tuple<const bcCHAR*, const bci_animation_job*> at(bcUINT32 p_position) const noexcept;

			bcSIZE size() const noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;
		
		private:
			core::bc_vector<bc_sequence_animation> m_jobs;
		};

		inline bc_sequence_animation_job::bc_sequence_animation_job(core::bc_span<bc_sequence_animation>& p_animations)
			: bci_animation_job((*p_animations.begin()).m_animation->get_skeleton()),
			m_jobs(std::make_move_iterator(std::begin(p_animations)), std::make_move_iterator(std::end(p_animations)))
		{
		}

		inline bc_sequence_animation_job::bc_sequence_animation_job(bc_sequence_animation_job&& p_other) noexcept = default;

		inline bc_sequence_animation_job::~bc_sequence_animation_job() = default;

		inline bc_sequence_animation_job& bc_sequence_animation_job::operator=(bc_sequence_animation_job&&) noexcept = default;

		inline std::tuple<const bcCHAR*, bci_animation_job*> bc_sequence_animation_job::at(bcUINT32 p_position) noexcept
		{
			auto& l_job = m_jobs[p_position];
			return std::make_tuple(l_job.m_name, l_job.m_animation.get());
		}

		inline std::tuple<const bcCHAR*, const bci_animation_job*> bc_sequence_animation_job::at(bcUINT32 p_position) const noexcept
		{
			auto& l_job = m_jobs[p_position];
			return std::make_tuple(l_job.m_name, l_job.m_animation.get());
		}

		inline bcSIZE bc_sequence_animation_job::size() const noexcept
		{
			return m_jobs.size();
		}

		inline bool bc_sequence_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
		{
			if (!m_enabled)
			{
				return true;
			}
			
			for(auto& l_job : m_jobs)
			{
				if(!l_job.m_animation->run(p_clock))
				{
					break;
				}
			}

			return true;
		}
	}
}