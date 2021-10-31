// [12/31/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcDelegate.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcSamplingAnimationJob.h"
#include "Game/Object/Animation/Job/bcLocalToModelAnimationJob.h"
#include "Game/Object/Animation/Job/bcModelToSkinnedAnimationJob.h"
#include "Game/Object/Animation/Job/bcSequenceAnimationJob.h"
#include "Game/Object/Animation/Job/bcTagAnimationJob.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		class bc_sub_mesh;
		
		class bc_animation_job_builder2
		{
		public:
			bc_animation_job_builder2(core::bc_vector<bc_sequence_animation> p_animations);

			bc_animation_job_builder2(bc_animation_job_builder2&&) noexcept;

			~bc_animation_job_builder2();

			bc_animation_job_builder2& operator=(bc_animation_job_builder2&&) noexcept;
			
			core::bc_shared_ptr<bci_animation_job> build();
		
		private:
			core::bc_vector<bc_sequence_animation> m_animations;
		};
		
		class bc_animation_job_builder1
		{
		public:
			bc_animation_job_builder1(core::bc_vector<bc_sequence_animation> p_animations);

			bc_animation_job_builder1(bc_animation_job_builder1&&) noexcept;

			~bc_animation_job_builder1();

			bc_animation_job_builder1& operator=(bc_animation_job_builder1&&) noexcept;

			bc_animation_job_builder1& then(core::bc_shared_ptr<bci_animation_job> p_job, const bcCHAR* p_name = nullptr);
			
			bc_animation_job_builder1& then(bc_tag_animation_job p_job, const bcCHAR* p_name = nullptr);

			/**
			 * \brief Add local_to_model and model_to_skinned animation jobs and use last job as local transform provider
			 * \param p_sub_mesh
			 * \param p_model_transforms
			 * \param p_world_transforms
			 * \param p_out_to_model_job
			 * \param p_out_to_skinning_job
			 * \return
			 */
			bc_animation_job_builder1& with_local_to_model_to_skinning(const bc_sub_mesh& p_sub_mesh,
				bc_sub_mesh_mat4_transform& p_model_transforms,
				bc_sub_mesh_mat4_transform& p_world_transforms,
				core::bc_shared_ptr<bc_local_to_model_animation_job>* p_out_to_model_job = nullptr,
				core::bc_shared_ptr<bc_model_to_skinned_animation_job>* p_out_to_skinning_job = nullptr);
			
			bc_animation_job_builder2 end_with(core::bc_shared_ptr<bci_animation_job> p_job, const bcCHAR* p_name = nullptr);
			
			bc_animation_job_builder2 end_with(bc_tag_animation_job p_job, const bcCHAR* p_name = nullptr);

			bc_animation_job_builder2 end();

		private:
			core::bc_vector<bc_sequence_animation> m_animations;
		};
		
		class bc_animation_job_builder
		{
		public:
			bc_animation_job_builder();

			bc_animation_job_builder(bc_animation_job_builder&&) noexcept;

			~bc_animation_job_builder();

			bc_animation_job_builder& operator=(bc_animation_job_builder&&) noexcept;

			bc_animation_job_builder1 start_with(core::bc_shared_ptr<bci_animation_job> p_job, const bcCHAR* p_name = nullptr);
			
			bc_animation_job_builder1 start_with(bc_tag_animation_job p_job, const bcCHAR* p_name = nullptr);
		
		private:
			core::bc_vector<bc_sequence_animation> m_animations;
		};

		inline bc_animation_job_builder2::bc_animation_job_builder2(core::bc_vector<bc_sequence_animation> p_animations)
			: m_animations(std::move(p_animations))
		{
		}

		inline bc_animation_job_builder2::bc_animation_job_builder2(bc_animation_job_builder2&&) noexcept = default;

		inline bc_animation_job_builder2::~bc_animation_job_builder2() = default;

		inline bc_animation_job_builder2& bc_animation_job_builder2::operator=(bc_animation_job_builder2&&) noexcept = default;
		
		inline core::bc_shared_ptr<bci_animation_job> bc_animation_job_builder2::build()
		{
			auto l_span = core::bc_make_span(m_animations);
			return core::bc_make_unique<bc_sequence_animation_job>(bc_sequence_animation_job(l_span));
		}

		inline bc_animation_job_builder1::bc_animation_job_builder1(core::bc_vector<bc_sequence_animation> p_animations)
			: m_animations(std::move(p_animations))
		{
		}

		inline bc_animation_job_builder1::bc_animation_job_builder1(bc_animation_job_builder1&&) noexcept = default;

		inline bc_animation_job_builder1::~bc_animation_job_builder1() = default;

		inline bc_animation_job_builder1& bc_animation_job_builder1::operator=(bc_animation_job_builder1&&) noexcept = default;

		inline bc_animation_job_builder1& bc_animation_job_builder1::then(core::bc_shared_ptr<bci_animation_job> p_job, const bcCHAR* p_name)
		{
			m_animations.push_back(bc_sequence_animation{ p_name, false, std::move(p_job) });

			return *this;
		}

		inline bc_animation_job_builder1& bc_animation_job_builder1::then(bc_tag_animation_job p_job, const bcCHAR* p_name)
		{
			m_animations.push_back(bc_sequence_animation{ p_name, true, std::move(p_job.m_job) });

			return *this;
		}

		inline bc_animation_job_builder1& bc_animation_job_builder1::with_local_to_model_to_skinning(const bc_sub_mesh& p_sub_mesh,
			bc_sub_mesh_mat4_transform& p_model_transforms,
			bc_sub_mesh_mat4_transform& p_world_transforms,
			core::bc_shared_ptr<bc_local_to_model_animation_job>* p_out_to_model_job,
			core::bc_shared_ptr<bc_model_to_skinned_animation_job>* p_out_to_skinning_job)
		{
			auto* l_local_transform_job = dynamic_cast<bci_local_transform_animation_job*>(m_animations.back().m_animation.get());
			if (!l_local_transform_job)
			{
				throw bc_invalid_operation_exception("Last animation job must be a local transform job");
			}

			auto l_local_to_model_job = core::bc_make_shared<bc_local_to_model_animation_job>(bc_local_to_model_animation_job
			(
				m_animations.back().m_animation,
				p_sub_mesh,
				p_model_transforms
			));
			auto l_model_to_skinned_job = core::bc_make_shared<bc_model_to_skinned_animation_job>(bc_model_to_skinned_animation_job
			(
				m_animations.back().m_animation,
				p_world_transforms
			));

			if(p_out_to_model_job)
			{
				*p_out_to_model_job = l_local_to_model_job;
			}
			if(p_out_to_skinning_job)
			{
				*p_out_to_skinning_job = l_model_to_skinned_job;
			}
			
			m_animations.push_back(bc_sequence_animation{ nullptr, false, std::move(l_local_to_model_job) });
			m_animations.push_back(bc_sequence_animation{ nullptr, false, std::move(l_model_to_skinned_job) });

			return *this;
		}
		
		inline bc_animation_job_builder2 bc_animation_job_builder1::end_with(core::bc_shared_ptr<bci_animation_job> p_job, const bcCHAR* p_name)
		{
			m_animations.push_back(bc_sequence_animation{ p_name, false, std::move(p_job) });

			return bc_animation_job_builder2(std::move(m_animations));
		}

		inline bc_animation_job_builder2 bc_animation_job_builder1::end_with(bc_tag_animation_job p_job, const bcCHAR* p_name)
		{
			m_animations.push_back(bc_sequence_animation{ p_name, true, std::move(p_job.m_job) });

			return bc_animation_job_builder2(std::move(m_animations));
		}

		inline bc_animation_job_builder2 bc_animation_job_builder1::end()
		{
			return bc_animation_job_builder2(std::move(m_animations));
		}

		inline bc_animation_job_builder::bc_animation_job_builder() = default;

		inline bc_animation_job_builder::bc_animation_job_builder(bc_animation_job_builder&&) noexcept = default;

		inline bc_animation_job_builder::~bc_animation_job_builder() = default;

		inline bc_animation_job_builder& bc_animation_job_builder::operator=(bc_animation_job_builder&&) noexcept = default;

		inline bc_animation_job_builder1 bc_animation_job_builder::start_with(core::bc_shared_ptr<bci_animation_job> p_job, const bcCHAR* p_name)
		{
			m_animations.reserve(10);
			m_animations.push_back(bc_sequence_animation{ p_name, false, std::move(p_job) });

			return bc_animation_job_builder1(std::move(m_animations));
		}

		inline bc_animation_job_builder1 bc_animation_job_builder::start_with(bc_tag_animation_job p_job, const bcCHAR* p_name)
		{
			m_animations.reserve(10);
			m_animations.push_back(bc_sequence_animation{ p_name, true, std::move(p_job.m_job) });

			return bc_animation_job_builder1(std::move(m_animations));
		}
	}
}