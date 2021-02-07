// [12/30/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcIteratorAdapter.h"

#include "3rdParty/Ozz/Include/ozz/animation/runtime/skeleton.h"

namespace black_cat
{
	namespace game
	{
		class bc_animation_skeleton
		{
		public:
			using bone_name_iterator = core::bc_const_iterator_adapter< core::bc_vector_movable< const bcCHAR* > >;
		
		public:
			bc_animation_skeleton();

			bc_animation_skeleton(bc_animation_skeleton&& p_other) noexcept;

			~bc_animation_skeleton();

			bc_animation_skeleton& operator=(bc_animation_skeleton&& p_other) noexcept;

			bcUINT32 get_num_bones() const noexcept;

			bone_name_iterator get_bone_names() const noexcept;
			
			ozz::animation::Skeleton& get_native_handle() noexcept;
			
		private:
			core::bc_unique_ptr<ozz::animation::Skeleton> m_skeleton;
			mutable core::bc_vector_movable< const bcCHAR* > m_bone_names;
		};

		inline bc_animation_skeleton::bc_animation_skeleton()
		{
			m_skeleton = core::bc_make_unique< ozz::animation::Skeleton >();
		}

		inline bc_animation_skeleton::bc_animation_skeleton(bc_animation_skeleton&& p_other) noexcept = default;
		
		inline bc_animation_skeleton::~bc_animation_skeleton() = default;
		
		inline bc_animation_skeleton& bc_animation_skeleton::operator=(bc_animation_skeleton&& p_other) noexcept = default;

		inline bcUINT32 bc_animation_skeleton::get_num_bones() const noexcept
		{
			return m_skeleton->num_joints();
		}

		inline bc_animation_skeleton::bone_name_iterator bc_animation_skeleton::get_bone_names() const noexcept
		{
			if(m_bone_names.empty() && m_skeleton)
			{
				auto& l_skeleton = *m_skeleton;
				m_bone_names.resize(l_skeleton.num_joints());

				for(auto l_ite = 0; l_ite < l_skeleton.num_joints(); ++l_ite)
				{
					m_bone_names[l_ite] = l_skeleton.joint_names()[l_ite];
				}
			}

			return bone_name_iterator(m_bone_names);
		}

		inline ozz::animation::Skeleton& bc_animation_skeleton::get_native_handle() noexcept
		{
			return *m_skeleton;
		}
	}	
}