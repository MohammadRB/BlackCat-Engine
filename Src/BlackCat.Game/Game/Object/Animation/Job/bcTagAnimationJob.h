// [10/31/2021 MRB]

#pragma once

#include "Game/Object/Animation/bcAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		class bc_tag_animation_job
		{
		public:
			explicit bc_tag_animation_job(core::bc_shared_ptr<bci_animation_job> p_job)
				: m_job(std::move(p_job))
			{
			}

			bc_tag_animation_job(bc_tag_animation_job&&) noexcept = default;

			~bc_tag_animation_job() = default;

			bc_tag_animation_job& operator=(bc_tag_animation_job&&) noexcept = default;

			core::bc_shared_ptr<bci_animation_job> m_job;
		};
	}
}