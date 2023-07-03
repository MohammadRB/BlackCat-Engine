// [30/12/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcSpan.h"
#include "Game/bcExport.h"
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

			core::bc_const_span<const bcCHAR*> get_joint_names() const noexcept;

			/**
			 * \brief Try to find a bone which is most like the provided name
			 * \param p_name 
			 * \return pair of found bone index and name.
			 */
			std::pair<bcUINT32, const bcCHAR*> find_joint_by_name(const bcCHAR* p_name) const noexcept;
			
			ozz::animation::Skeleton& get_native_handle() noexcept;
			
		private:
			core::bc_unique_ptr<ozz::animation::Skeleton> m_skeleton;
			mutable core::bc_vector_movable<const bcCHAR*> m_joint_names;
		};

		inline bc_animation_skeleton::bc_animation_skeleton()
		{
			m_skeleton = core::bc_make_unique< ozz::animation::Skeleton >();
		}

		inline bc_animation_skeleton::bc_animation_skeleton(bc_animation_skeleton&& p_other) noexcept = default;
		
		inline bc_animation_skeleton::~bc_animation_skeleton() = default;
		
		inline bc_animation_skeleton& bc_animation_skeleton::operator=(bc_animation_skeleton&& p_other) noexcept = default;

		inline bcUINT32 bc_animation_skeleton::get_num_joints() const noexcept
		{
			return m_skeleton->num_joints();
		}

		inline core::bc_const_span<const bcCHAR*> bc_animation_skeleton::get_joint_names() const noexcept
		{
			if (m_joint_names.empty() && m_skeleton)
			{
				auto& l_skeleton = *m_skeleton;
				m_joint_names.resize(l_skeleton.num_joints());

				for (auto l_ite = 0; l_ite < l_skeleton.num_joints(); ++l_ite)
				{
					m_joint_names[l_ite] = l_skeleton.joint_names()[l_ite];
				}
			}

			return core::bc_make_cspan(m_joint_names);
		}

		inline std::pair<bcUINT32, const bcCHAR*> bc_animation_skeleton::find_joint_by_name(const bcCHAR* p_name) const noexcept
		{
			const bcCHAR* l_alike_bone = nullptr;
			auto l_alike_bone_length = 0U;
			auto l_alike_bone_index = 0U;
			auto l_bone_names = get_joint_names();

			auto l_ite = 0U;
			for (const bcCHAR* l_bone_name : l_bone_names)
			{
				if (std::strstr(l_bone_name, p_name) != nullptr)
				{
					const auto l_bone_name_length = std::strlen(l_bone_name);
					if (l_alike_bone == nullptr || l_alike_bone_length > l_bone_name_length)
					{
						l_alike_bone = l_bone_name;
						l_alike_bone_length = l_bone_name_length;
						l_alike_bone_index = l_ite;
					}
				}

				++l_ite;
			}

			return std::make_pair(l_alike_bone_index, l_alike_bone);
		}
		
		inline ozz::animation::Skeleton& bc_animation_skeleton::get_native_handle() noexcept
		{
			return *m_skeleton;
		}
	}	
}