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
			bc_sequence_animation_job(core::bc_span<core::bc_unique_ptr< bci_animation_job >>& p_animations);
			
			bc_sequence_animation_job(core::bc_span<core::bc_unique_ptr< bci_animation_job >>& p_animations, execute_callback p_callback);

			bc_sequence_animation_job(bc_sequence_animation_job&&) noexcept;

			~bc_sequence_animation_job();

			bc_sequence_animation_job& operator=(bc_sequence_animation_job&&) noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;
		
		private:
			core::bc_vector< core::bc_unique_ptr< bci_animation_job > > m_jobs;
			execute_callback m_callback;
		};

		inline bc_sequence_animation_job::bc_sequence_animation_job(core::bc_span<core::bc_unique_ptr< bci_animation_job >>& p_animations)
			: bci_animation_job((*p_animations.begin())->get_skeleton()),
			m_jobs(std::make_move_iterator(std::begin(p_animations)), std::make_move_iterator(std::end(p_animations)))
		{
		}

		inline bc_sequence_animation_job::bc_sequence_animation_job(core::bc_span<core::bc_unique_ptr< bci_animation_job >>& p_animations, execute_callback p_callback)
			: bci_animation_job((*p_animations.begin())->get_skeleton()),
			m_jobs(std::make_move_iterator(std::begin(p_animations)), std::make_move_iterator(std::end(p_animations))),
			m_callback(std::move(p_callback))
		{
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