// [12/26/2020 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Container/bcVector.h"
#include "Graphic/bcRenderApiInfo.h"
#include "BlackCat/Loader/bcSkinnedAnimationLoader.h"

#include "3rdParty/Assimp/Include/Importer.hpp"
#include "3rdParty/Assimp/Include/postprocess.h"
#include "3rdParty/Assimp/Include/scene.h"
#include "3rdParty/Ozz/Include/ozz/base/io/stream.h"
#include "3rdParty/Ozz/Include/ozz/base/io/archive.h"
#include "3rdParty/Ozz/Include/ozz/animation/offline/raw_skeleton.h"
#include "3rdParty/Ozz/Include/ozz/animation/offline/raw_animation.h"
#include "3rdParty/Ozz/Include/ozz/animation/offline/skeleton_builder.h"
#include "3rdParty/Ozz/Include/ozz/animation/offline/animation_builder.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/skeleton.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/animation.h"

namespace black_cat
{
	using namespace ozz;
	using namespace ozz::io;
	using namespace ozz::math;
	using namespace ozz::animation;
	using namespace ozz::animation::offline;
	
	void _build_ozz_skeleton(const aiNode* p_ai_nodes_begin, bcSIZE p_ai_nodes_count, RawSkeleton::Joint::Children& p_ozz_skeletons)
	{
		p_ozz_skeletons.resize(p_ai_nodes_count);
		
		for(auto l_node_ite = 0U; l_node_ite < p_ai_nodes_count; ++l_node_ite)
		{
			const auto& l_ai_node = p_ai_nodes_begin[p_ai_nodes_count];
			auto& l_ozz_joint = p_ozz_skeletons[l_node_ite];

			aiVector3D l_node_translation;
			aiVector3D l_node_scale;
			aiQuaternion l_node_rotation;
			l_ai_node.mTransformation.Decompose(l_node_scale, l_node_rotation, l_node_translation);
			
			l_ozz_joint.name = l_ai_node.mName.C_Str();
			l_ozz_joint.transform.translation = Float3(l_node_translation.x, l_node_translation.y, l_node_translation.z);
			l_ozz_joint.transform.scale = Float3(l_node_scale.x, l_node_scale.y, l_node_scale.z);
			l_ozz_joint.transform.rotation = Quaternion(l_node_rotation.x, l_node_rotation.y, l_node_rotation.z, l_node_rotation.w);

			_build_ozz_skeleton(l_ai_node.mChildren[0], l_ai_node.mNumChildren, l_ozz_joint.children);
		}
	}

	void _build_ozz_animation(const aiAnimation* p_ai_animations, bcSIZE p_ai_animation_count, const Skeleton& p_ozz_skeleton, core::bc_vector_frame<RawAnimation>& p_ozz_animations)
	{
		p_ozz_animations.reserve(p_ai_animation_count);
		
		for(auto l_anim_ite = 0U; l_anim_ite < p_ai_animation_count; ++l_anim_ite)
		{
			const auto& l_ai_animation = p_ai_animations[l_anim_ite];
			RawAnimation l_ozz_animation;
			
			l_ozz_animation.name = l_ai_animation.mName.C_Str();
			l_ozz_animation.duration = l_ai_animation.mDuration * l_ai_animation.mTicksPerSecond;
			l_ozz_animation.tracks.resize(l_ai_animation.mNumChannels);

			auto l_ozz_joint_names = p_ozz_skeleton.joint_names();
			auto l_find_joint_index = [&](const bcCHAR* p_joint_name)
			{
				const auto* l_joint_ite = std::find_if(std::begin(l_ozz_joint_names), std::end(l_ozz_joint_names), [](const bcCHAR* p_1, const bcCHAR* p_2)
				{
					return std::strcmp(p_1, p_2);
				});

				bcAssert(l_joint_ite != std::end(l_ozz_joint_names));

				return std::distance(std::begin(l_ozz_joint_names), l_joint_ite);
			};
			
			for(auto l_anim_channel_ite = 0U; l_anim_channel_ite < l_ai_animation.mNumChannels; ++l_anim_channel_ite)
			{
				const auto* l_ai_channel = l_ai_animation.mChannels[l_anim_channel_ite];
				const auto l_ozz_animation_index = l_find_joint_index(l_ai_channel->mNodeName.C_Str());
				
				for(auto l_ai_channel_position_ite = 0U; l_ai_channel_position_ite < l_ai_channel->mNumPositionKeys; ++l_ai_channel_position_ite)
				{
					auto& l_ai_position = l_ai_channel->mPositionKeys[l_ai_channel_position_ite];
					l_ozz_animation.tracks[l_ozz_animation_index].translations.push_back(RawAnimation::TranslationKey
					{
						static_cast<bcFLOAT>(l_ai_position.mTime),
						Float3(l_ai_position.mValue.x, l_ai_position.mValue.y, l_ai_position.mValue.z)
					});
				}
				for (auto l_ai_channel_scale_ite = 0U; l_ai_channel_scale_ite < l_ai_channel->mNumScalingKeys; ++l_ai_channel_scale_ite)
				{
					auto& l_ai_scale = l_ai_channel->mScalingKeys[l_ai_channel_scale_ite];
					l_ozz_animation.tracks[l_ozz_animation_index].scales.push_back(RawAnimation::ScaleKey
					{
						static_cast<bcFLOAT>(l_ai_scale.mTime),
						Float3(l_ai_scale.mValue.x, l_ai_scale.mValue.y, l_ai_scale.mValue.z)
					});
				}
				for (auto l_ai_channel_rotation_ite = 0U; l_ai_channel_rotation_ite < l_ai_channel->mNumRotationKeys; ++l_ai_channel_rotation_ite)
				{
					auto& l_ai_rotation = l_ai_channel->mRotationKeys[l_ai_channel_rotation_ite];
					l_ozz_animation.tracks[l_ozz_animation_index].rotations.push_back(RawAnimation::RotationKey
					{
						static_cast<bcFLOAT>(l_ai_rotation.mTime),
						Quaternion(l_ai_rotation.mValue.x, l_ai_rotation.mValue.y, l_ai_rotation.mValue.z, l_ai_rotation.mValue.w)
					});
				}
			}
		}
	}

	void _write_ozz_structures(const Skeleton& p_ozz_skeleton, const core::bc_vector_frame<unique_ptr<Animation>>& p_ozz_animations, MemoryStream& p_stream)
	{
		auto l_num_animations = p_ozz_animations.size();
		p_stream.Write(&l_num_animations, sizeof(decltype(l_num_animations)));

		OArchive l_ozz_archive(&p_stream);
		l_ozz_archive << p_ozz_skeleton;

		for(auto& l_ozz_animation : p_ozz_animations)
		{
			l_ozz_archive << *l_ozz_animation;
		}
	}
	
	bool bc_skinned_animation_loader::support_offline_processing() const
	{
		return true;
	}

	void bc_skinned_animation_loader::content_offline_processing(core::bc_content_loading_context& p_context) const
	{
		Assimp::Importer l_importer;

		const aiScene* const* l_ai_scene_value = p_context.m_instance_parameters.get_value<const aiScene*>("aiScene");
		const aiScene* l_ai_scene = l_ai_scene_value ? *l_ai_scene_value : nullptr;
		if (!l_ai_scene)
		{
			l_ai_scene = l_importer.ReadFileFromMemory
			(
				p_context.m_file_buffer.get(),
				p_context.m_file_buffer_size,
				graphic::bc_render_api_info::is_left_handed() ? aiProcess_ConvertToLeftHanded : 0
			);
		}

		if (!l_ai_scene || !l_ai_scene->HasAnimations())
		{
			const auto l_error_msg =
				core::bc_string_frame("Content file loading error: ")
				+
				core::bc_to_string_frame(p_context.m_file_path)
				+
				", "
				+
				l_importer.GetErrorString();
			p_context.set_result(bc_io_exception(l_error_msg.c_str()));

			return;
		}

		RawSkeleton l_ozz_raw_skeleton;
		l_ozz_raw_skeleton.roots.resize(1);
		
		_build_ozz_skeleton(l_ai_scene->mRootNode, 1, l_ozz_raw_skeleton.roots);

		const auto l_ozz_skeleton = SkeletonBuilder()(l_ozz_raw_skeleton);
		core::bc_vector_frame<RawAnimation> l_ozz_raw_animations;

		_build_ozz_animation(l_ai_scene->mAnimations[0], l_ai_scene->mNumAnimations, *l_ozz_skeleton, l_ozz_raw_animations);

		core::bc_vector_frame<unique_ptr<Animation>> l_ozz_animations;
		l_ozz_animations.reserve(l_ozz_raw_animations.size());

		std::transform
		(
			std::begin(l_ozz_raw_animations),
			std::end(l_ozz_raw_animations),
			std::back_inserter(l_ozz_animations),
			[](const RawAnimation& p_ozz_raw_animation)
			{
				bcAssert(p_ozz_raw_animation.Validate());
				return AnimationBuilder()(p_ozz_raw_animation);
			}
		);

		MemoryStream l_ozz_stream;
		const core::bc_unique_ptr<bcBYTE> l_buffer(static_cast<bcBYTE*>(bcAllocThrow(l_ozz_stream.Size(), core::bc_alloc_type::frame)));
		
		_write_ozz_structures(*l_ozz_skeleton, l_ozz_animations, l_ozz_stream);
		
		l_ozz_stream.Seek(0, Stream::kSet);
		l_ozz_stream.Read(l_buffer.get(), l_ozz_stream.Size());
		
		p_context.m_file.write(l_buffer.get(), l_ozz_stream.Size());
	}

	void bc_skinned_animation_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
	}
}