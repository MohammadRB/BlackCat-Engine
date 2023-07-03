// [31/10/2021 MRB]

#pragma once

#include "Game/System/Animation/bcAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		/**
		 * \brief Tag animations will be stored in animation pipeline to be accessible but wont be executed.
		 */
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