// [12/31/2020 MRB]

#pragma once

#include "Game/Object/Animation/bcAnimationSkeleton.h"

namespace black_cat
{
	namespace game
	{
		class bci_animation_job
		{
		public:
			virtual ~bci_animation_job() = 0;
			
			virtual void run() const = 0;

		protected:
			explicit bci_animation_job(bc_animation_skeleton& p_skeleton);

			bci_animation_job(bci_animation_job&&) noexcept = default;

			bci_animation_job& operator=(bci_animation_job&&) noexcept = default;

			const bc_animation_skeleton* get_skeleton() const noexcept;
			
		private:
			bc_animation_skeleton* m_skeleton;
		};

		inline bci_animation_job::~bci_animation_job() = default;

		inline bci_animation_job::bci_animation_job(bc_animation_skeleton& p_skeleton)
			: m_skeleton(&p_skeleton)
		{
		}

		inline const bc_animation_skeleton* bci_animation_job::get_skeleton() const noexcept
		{
			return m_skeleton;
		}
	}	
}