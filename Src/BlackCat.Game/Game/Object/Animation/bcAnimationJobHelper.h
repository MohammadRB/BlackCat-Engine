// [03/16/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Game/Object/Animation/Job/bcSequenceAnimationJob.h"
#include "Game/Object/Animation/Job/bcAimAnimationJob.h"
#include "Game/Object/Animation/Job/bcModelToSkinnedAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		class bc_animation_job_helper
		{
		public:
			template<typename TJob>
			static TJob* find_job(bci_animation_job& p_job, const bcCHAR* p_name = nullptr);

			static void set_aim_world_target(bci_animation_job& p_job, const core::bc_vector3f& p_target);
			
			/**
			 * \brief Find 'model to skinned' job and set world transform
			 * \param p_job
			 * \param p_world
			 */
			static void set_skinning_world_transform(bci_animation_job& p_job, const core::bc_matrix4f& p_world);
		};

		template<typename TJob>
		TJob* bc_animation_job_helper::find_job(bci_animation_job& p_job, const bcCHAR* p_name)
		{
			auto& l_sequence_job = static_cast<bc_sequence_animation_job&>(p_job);
			
			for (auto l_job_ite = 0U; l_job_ite <l_sequence_job.size(); ++l_job_ite)
			{
				auto [l_job_name, l_job] = l_sequence_job.at(l_job_ite);
				if(p_name)
				{
					if (!l_job_name || std::strcmp(l_job_name, p_name) != 0)
					{
						continue;
					}
				}
				
				auto* l_concrete_job = dynamic_cast<TJob*>(l_job);
				if (l_concrete_job)
				{
					return l_concrete_job;
				}
			}

			return nullptr;
		}

		inline void bc_animation_job_helper::set_aim_world_target(bci_animation_job& p_job, const core::bc_vector3f& p_target)
		{
			auto& l_sequence_job = static_cast<bc_sequence_animation_job&>(p_job);
			auto* l_aim_job = find_job<bc_aim_animation_job>(l_sequence_job);
			if(!l_aim_job)
			{
				BC_ASSERT(false);
				return;
			}

			l_aim_job->set_world_target(p_target);
		}

		inline void bc_animation_job_helper::set_skinning_world_transform(bci_animation_job& p_job, const core::bc_matrix4f& p_world)
		{
			auto& l_sequence_job = static_cast<bc_sequence_animation_job&>(p_job);
			
			for (bcINT32 l_job_ite = l_sequence_job.size() - 1; l_job_ite>= 0; --l_job_ite)
			{
				auto [l_name, l_job] = l_sequence_job.at(l_job_ite);
				auto* l_concrete_job = dynamic_cast<bc_model_to_skinned_animation_job*>(l_job);
				if (l_concrete_job)
				{
					l_concrete_job->set_world(p_world);
					break;
				}
			}
		}
	}	
}