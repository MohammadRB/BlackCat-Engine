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
			template< typename TJob >
			static TJob* find_job(bc_sequence_animation_job& p_job, bcUINT32 p_search_from = 0);

			static void set_aim_world_target(bc_sequence_animation_job& p_job, const core::bc_vector3f& p_target);
			
			/**
			 * \brief Find 'model to skinned' job and set world transform
			 * \param p_job
			 * \param p_world
			 */
			static void set_skinning_world_transform(bc_sequence_animation_job& p_job, const core::bc_matrix4f& p_world);
		};

		template< typename TJob >
		TJob* bc_animation_job_helper::find_job(bc_sequence_animation_job& p_job, bcUINT32 p_search_from)
		{
			for (bcINT32 l_job_ite = p_search_from; l_job_ite < p_job.size(); ++l_job_ite)
			{
				auto* l_job = dynamic_cast<TJob*>(p_job.at(l_job_ite));
				if (l_job)
				{
					return l_job;
				}
			}

			return nullptr;
		}

		inline void bc_animation_job_helper::set_aim_world_target(bc_sequence_animation_job& p_job, const core::bc_vector3f& p_target)
		{
			auto* l_aim_job = find_job<bc_aim_animation_job>(p_job);
			if(!l_aim_job)
			{
				BC_ASSERT(false);
				return;
			}

			l_aim_job->set_world_target(p_target);
		}

		inline void bc_animation_job_helper::set_skinning_world_transform(bc_sequence_animation_job& p_job, const core::bc_matrix4f& p_world)
		{
			for (bcINT32 l_job_ite = p_job.size() - 1; l_job_ite >= 0; --l_job_ite)
			{
				auto* l_job = dynamic_cast<bc_model_to_skinned_animation_job*>(p_job.at(l_job_ite));
				if (l_job)
				{
					l_job->set_world(p_world);
					break;
				}
			}
		}
	}	
}