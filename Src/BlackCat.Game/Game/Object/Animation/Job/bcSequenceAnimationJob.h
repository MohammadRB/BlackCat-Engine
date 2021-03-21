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
		class bc_sequence_animation_job : public bci_animation_job
		{
		public:
			using execute_callback = core::bc_delegate< void() >;
			
		public:
			explicit bc_sequence_animation_job(core::bc_span<core::bc_shared_ptr< bci_animation_job >>& p_animations);
			
			bc_sequence_animation_job(bc_sequence_animation_job&&) noexcept;

			~bc_sequence_animation_job() override;

			bc_sequence_animation_job& operator=(bc_sequence_animation_job&&) noexcept;

			bci_animation_job* at(bcUINT32 p_position) noexcept;
			
			const bci_animation_job* at(bcUINT32 p_position) const noexcept;

			bcSIZE size() const noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;
		
		private:
			core::bc_vector< core::bc_shared_ptr< bci_animation_job > > m_jobs;
			execute_callback m_callback;
		};

		inline bc_sequence_animation_job::bc_sequence_animation_job(core::bc_span<core::bc_shared_ptr< bci_animation_job >>& p_animations)
			: bci_animation_job((*p_animations.begin())->get_skeleton()),
			m_jobs(std::make_move_iterator(std::begin(p_animations)), std::make_move_iterator(std::end(p_animations)))
		{
		}

		inline bc_sequence_animation_job::bc_sequence_animation_job(bc_sequence_animation_job&& p_other) noexcept = default;

		inline bc_sequence_animation_job::~bc_sequence_animation_job() = default;

		inline bc_sequence_animation_job& bc_sequence_animation_job::operator=(bc_sequence_animation_job&&) noexcept = default;

		inline bci_animation_job* bc_sequence_animation_job::at(bcUINT32 p_position) noexcept
		{
			return m_jobs[p_position].get();
		}

		inline const bci_animation_job* bc_sequence_animation_job::at(bcUINT32 p_position) const noexcept
		{
			return const_cast<bc_sequence_animation_job&>(*this).at(p_position);
		}

		inline bcSIZE bc_sequence_animation_job::size() const noexcept
		{
			return m_jobs.size();
		}

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