// [30/12/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"

#include "3rdParty/Ozz/Include/ozz/animation/runtime/animation.h"

namespace black_cat
{
	namespace game
	{
		class bc_skeleton_animation
		{
		public:
			bc_skeleton_animation();

			bc_skeleton_animation(bc_skeleton_animation&& p_other) noexcept;

			~bc_skeleton_animation();

			bc_skeleton_animation& operator=(bc_skeleton_animation&& p_other) noexcept;

			const bcCHAR* get_name() const noexcept;

			bcFLOAT get_duration() const noexcept;

			bcUINT32 get_num_tracks() const noexcept;
			
			ozz::animation::Animation& get_native_handle() noexcept;
			
		private:
			core::bc_unique_ptr<ozz::animation::Animation> m_animation;
		};

		inline bc_skeleton_animation::bc_skeleton_animation()
		{
			m_animation = core::bc_make_unique<ozz::animation::Animation>();
		}

		inline bc_skeleton_animation::bc_skeleton_animation(bc_skeleton_animation&& p_other) noexcept = default;
		
		inline bc_skeleton_animation::~bc_skeleton_animation() = default;
		
		inline bc_skeleton_animation& bc_skeleton_animation::operator=(bc_skeleton_animation&& p_other) noexcept = default;

		inline const bcCHAR* bc_skeleton_animation::get_name() const noexcept
		{
			return m_animation->name();
		}

		inline bcFLOAT bc_skeleton_animation::get_duration() const noexcept
		{
			return m_animation->duration();
		}

		inline bcUINT32 bc_skeleton_animation::get_num_tracks() const noexcept
		{
			return m_animation->num_tracks();
		}

		inline ozz::animation::Animation& bc_skeleton_animation::get_native_handle() noexcept
		{
			return *m_animation;
		}
	}	
}