// [12/31/2020 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcAnimationJobSampling.h"
#include "Game/Object/Animation/Job/bcAnimationJobLocalToModelTransform.h"
#include "Game/Object/Animation/Job/bcAnimationJobModelToSkinnedTransform.h"
#include "Game/Object/Animation/Job/bcAnimationJobSequence.h"

namespace black_cat
{
	namespace game
	{
		class bc_animation_job_builder1
		{
		public:
			bc_animation_job_builder1(core::bc_vector_movable<bci_animation_job*> p_animations);

			bc_animation_job_builder1(bc_animation_job_builder1&&) noexcept;

			~bc_animation_job_builder1();

			bc_animation_job_builder1& operator=(bc_animation_job_builder1&&) noexcept;

			bc_animation_job_builder1& then(bci_animation_job& p_job);

			core::bc_unique_ptr<bci_animation_job> end_with(bci_animation_job& p_job);

		private:
			core::bc_vector_movable<bci_animation_job*> m_animations;
		};
		
		class bc_animation_job_builder
		{
		public:
			bc_animation_job_builder();

			bc_animation_job_builder(bc_animation_job_builder&&) noexcept;

			~bc_animation_job_builder();

			bc_animation_job_builder& operator=(bc_animation_job_builder&&) noexcept;
			
			bc_animation_job_builder1 start_with(bci_animation_job& p_job);
		
		private:
			core::bc_vector_movable<bci_animation_job*> m_animations;
		};

		inline bc_animation_job_builder1::bc_animation_job_builder1(core::bc_vector_movable<bci_animation_job*> p_animations)
			: m_animations(std::move(p_animations))
		{
		}

		inline bc_animation_job_builder1::bc_animation_job_builder1(bc_animation_job_builder1&&) noexcept = default;

		inline bc_animation_job_builder1::~bc_animation_job_builder1() = default;

		inline bc_animation_job_builder1& bc_animation_job_builder1::operator=(bc_animation_job_builder1&&) noexcept = default;

		inline bc_animation_job_builder1& bc_animation_job_builder1::then(bci_animation_job& p_job)
		{
			m_animations.push_back(&p_job);
			return *this;
		}

		inline core::bc_unique_ptr<bci_animation_job> bc_animation_job_builder1::end_with(bci_animation_job& p_job)
		{
			m_animations.push_back(&p_job);
			return core::bc_make_unique<bc_animation_job_sequence>(bc_animation_job_sequence(m_animations.data(), m_animations.size()));
		}
		
		inline bc_animation_job_builder::bc_animation_job_builder() = default;

		inline bc_animation_job_builder::bc_animation_job_builder(bc_animation_job_builder&&) noexcept = default;

		inline bc_animation_job_builder::~bc_animation_job_builder() = default;

		inline bc_animation_job_builder& bc_animation_job_builder::operator=(bc_animation_job_builder&&) noexcept = default;

		inline bc_animation_job_builder1 bc_animation_job_builder::start_with(bci_animation_job& p_job)
		{
			m_animations.reserve(8);
			m_animations.push_back(&p_job);

			return bc_animation_job_builder1(std::move(m_animations));
		}
	}	
}