// [12/30/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"

#include "3rdParty/Ozz/Include/ozz/animation/runtime/skeleton.h"

namespace black_cat
{
	namespace game
	{
		class bc_animation_skeleton
		{
		public:
			bc_animation_skeleton();

			bc_animation_skeleton(bc_animation_skeleton&& p_other) noexcept;

			~bc_animation_skeleton();

			bc_animation_skeleton& operator=(bc_animation_skeleton&& p_other) noexcept;

			bcUINT32 get_num_joints() const noexcept;
			
			ozz::animation::Skeleton* get_native_handle() noexcept;
			
		private:
			core::bc_unique_ptr<ozz::animation::Skeleton> m_animation;
		};

		inline bc_animation_skeleton::bc_animation_skeleton()
		{
			m_animation = core::bc_make_unique< ozz::animation::Skeleton >();
		}

		inline bc_animation_skeleton::bc_animation_skeleton(bc_animation_skeleton&& p_other) noexcept = default;
		
		inline bc_animation_skeleton::~bc_animation_skeleton() = default;
		
		inline bc_animation_skeleton& bc_animation_skeleton::operator=(bc_animation_skeleton&& p_other) noexcept = default;

		inline bcUINT32 bc_animation_skeleton::get_num_joints() const noexcept
		{
			return m_animation->num_joints();
		}

		inline ozz::animation::Skeleton* bc_animation_skeleton::get_native_handle() noexcept
		{
			return m_animation.get();
		}
	}	
}