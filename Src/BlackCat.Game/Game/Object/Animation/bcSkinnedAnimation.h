// [12/29/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Content/bcContent.h"
#include "Game/bcExport.h"

#include "3rdParty/Ozz/Include/ozz/animation/runtime/skeleton.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/animation.h"

namespace black_cat
{
	namespace game
	{
		class bc_skinned_animation : public core::bc_icontent
		{
			BC_CONTENT(anim)
			
		public:
			bc_skinned_animation(core::bc_string p_skeleton_name,
				core::bc_unique_ptr<ozz::animation::Skeleton> p_skeleton,
				core::bc_vector_frame<core::bc_unique_ptr<ozz::animation::Animation>> p_animations) noexcept;

			bc_skinned_animation(bc_skinned_animation&&) noexcept;

			~bc_skinned_animation();

			bc_skinned_animation& operator=(bc_skinned_animation&&) noexcept;
			
			const bcCHAR* get_skeleton_name() const noexcept;

		private:
			core::bc_string m_skeleton_name;
			core::bc_unique_ptr<ozz::animation::Skeleton> m_skeleton;
			core::bc_vector<core::bc_unique_ptr<ozz::animation::Animation>> m_animations;
		};

		using bc_skinned_animation_ptr = core::bc_content_ptr<bc_skinned_animation>;
	}
}