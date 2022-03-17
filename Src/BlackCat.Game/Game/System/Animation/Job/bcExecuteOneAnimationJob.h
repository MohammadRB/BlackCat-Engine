// [10/31/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/System/Animation/bcAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		class bc_execute_one_animation_job : public bci_local_transform_animation_job
		{
		public:
			bc_execute_one_animation_job(bc_animation_skeleton& p_skeleton, 
				const std::initializer_list<core::bc_shared_ptr<bci_local_transform_animation_job>>& p_jobs, 
				bcUINT32 p_active_index = 0)
				: bci_local_transform_animation_job(p_skeleton),
				m_jobs(p_jobs),
				m_active_index(p_active_index)
			{
			}

			bc_execute_one_animation_job(bc_execute_one_animation_job&&) noexcept = default;

			~bc_execute_one_animation_job() override = default;

			bc_execute_one_animation_job& operator=(bc_execute_one_animation_job&&) noexcept = default;

			bci_local_transform_animation_job& get_active_job() const noexcept
			{
				return *m_jobs[m_active_index];
			}
			
			void set_active_index(bcUINT32 p_active_index) noexcept
			{
				m_active_index = p_active_index;
			}

			bc_animation_local_transform& get_local_transforms() noexcept override
			{
				return get_active_job().get_local_transforms();
			}
			
			const bc_animation_local_transform& get_local_transforms() const noexcept override
			{
				return get_active_job().get_local_transforms();
			}
			
			bool run(const platform::bc_clock::update_param& p_clock) override
			{
				return m_jobs[m_active_index]->run(p_clock);
			}

		private:
			core::bc_vector<core::bc_shared_ptr<bci_local_transform_animation_job>> m_jobs;
			bcUINT32 m_active_index;
		};
	}	
}