// [12/29/2020 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcIteratorAdapter.h"
#include "Core/Content/bcContent.h"
#include "Game/Object/Animation/bcAnimationSkeleton.h"
#include "Game/Object/Animation/bcSkeletonAnimation.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_skinned_animation : public core::bci_content
		{
			BC_CONTENT(anim)
			
		public:
			using animation_iterator = core::bc_const_iterator_adapter< core::bc_vector< bc_skeleton_animation > >;
			
		public:
			bc_skinned_animation(bc_animation_skeleton p_skeleton, core::bc_vector<bc_skeleton_animation> p_animations) noexcept;

			bc_skinned_animation(bc_skinned_animation&&) noexcept;

			~bc_skinned_animation();

			bc_skinned_animation& operator=(bc_skinned_animation&&) noexcept;

			bc_animation_skeleton& get_skeleton() noexcept;
			
			const bc_animation_skeleton& get_skeleton() const noexcept;

			animation_iterator get_animations() const noexcept;
			
		private:
			bc_animation_skeleton m_skeleton;
			core::bc_vector< bc_skeleton_animation > m_animations;
		};

		using bc_skinned_animation_ptr = core::bc_content_ptr<bc_skinned_animation>;

		inline bc_skinned_animation::bc_skinned_animation(bc_animation_skeleton p_skeleton, core::bc_vector<bc_skeleton_animation> p_animations) noexcept
			: m_skeleton(std::move(p_skeleton)),
			m_animations(std::move(p_animations))
		{
		}

		inline bc_skinned_animation::bc_skinned_animation(bc_skinned_animation&&) noexcept = default;

		inline bc_skinned_animation::~bc_skinned_animation() = default;

		inline bc_skinned_animation& bc_skinned_animation::operator=(bc_skinned_animation&&) noexcept = default;

		inline bc_animation_skeleton& bc_skinned_animation::get_skeleton() noexcept
		{
			return m_skeleton;
		}

		inline const bc_animation_skeleton& bc_skinned_animation::get_skeleton() const noexcept
		{
			return m_skeleton;
		}

		inline bc_skinned_animation::animation_iterator bc_skinned_animation::get_animations() const noexcept
		{
			return animation_iterator(m_animations);
		}
	}
}