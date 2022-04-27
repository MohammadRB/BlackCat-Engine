// [12/26/2020 MRB]

#include "App/AppPCH.h"

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcStringStream.h"
#include "Core/bcUtility.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Game/System/Animation/bcSkinnedAnimation.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"
#include "App/Loader/bcSkinnedAnimationLoader.h"
#include "App/bcException.h"
#include "3rdParty/Assimp/Include/assimp/Importer.hpp"
#include "3rdParty/Assimp/Include/assimp/postprocess.h"
#include "3rdParty/Assimp/Include/assimp/scene.h"
#include "3rdParty/Ozz/Include/ozz/base/io/stream.h"
#include "3rdParty/Ozz/Include/ozz/base/io/archive.h"
#include "3rdParty/Ozz/Include/ozz/animation/offline/raw_skeleton.h"
#include "3rdParty/Ozz/Include/ozz/animation/offline/raw_animation.h"
#include "3rdParty/Ozz/Include/ozz/animation/offline/skeleton_builder.h"
#include "3rdParty/Ozz/Include/ozz/animation/offline/animation_builder.h"
#include "3rdParty/Ozz/Include/ozz/animation/offline/additive_animation_builder.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/skeleton.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/animation.h"

namespace black_cat
{
	void _build_ozz_skeleton(aiNode* const* p_ai_nodes_begin,
		bcSIZE p_ai_nodes_count,
		ozz::animation::offline::RawSkeleton::Joint::Children& p_ozz_skeletons)
	{
		p_ozz_skeletons.resize(p_ai_nodes_count);
		
		for(auto l_node_ite = 0U; l_node_ite < p_ai_nodes_count; ++l_node_ite)
		{
			const auto& l_ai_node = p_ai_nodes_begin[l_node_ite];
			auto& l_ozz_joint = p_ozz_skeletons[l_node_ite];
			
			aiVector3D l_node_translation;
			aiVector3D l_node_scale;
			aiQuaternion l_node_rotation;
			l_ai_node->mTransformation.Decompose(l_node_scale, l_node_rotation, l_node_translation);
			
			l_ozz_joint.name = l_ai_node->mName.C_Str();
			l_ozz_joint.transform.translation = ozz::math::Float3(l_node_translation.x, l_node_translation.y, l_node_translation.z);
			l_ozz_joint.transform.scale = ozz::math::Float3(l_node_scale.x, l_node_scale.y, l_node_scale.z);
			l_ozz_joint.transform.rotation = ozz::math::Quaternion(l_node_rotation.x, l_node_rotation.y, l_node_rotation.z, l_node_rotation.w);

			if(l_ai_node->mNumChildren > 0)
			{
				_build_ozz_skeleton(l_ai_node->mChildren, l_ai_node->mNumChildren, l_ozz_joint.children);
			}
		}
	}

	void _build_ozz_animation(aiAnimation* const * p_ai_animations,
		bcSIZE p_ai_animation_count,
		const ozz::animation::Skeleton& p_ozz_skeleton,
		core::bc_vector_frame< ozz::animation::offline::RawAnimation >& p_ozz_animations)
	{
		p_ozz_animations.resize(p_ai_animation_count);

		const auto l_ozz_joint_names = p_ozz_skeleton.joint_names();
		const auto l_find_joint_index = [&](const bcCHAR* p_joint_name)
		{
			const auto* l_joint_ite = std::find_if
			(
				std::begin(l_ozz_joint_names),
				std::end(l_ozz_joint_names),
				[p_joint_name](const bcCHAR* p_ozz_joint_name)
				{
					return std::strcmp(p_joint_name, p_ozz_joint_name) == 0;
				}
			);

			BC_ASSERT(l_joint_ite != std::end(l_ozz_joint_names));

			return std::distance(std::begin(l_ozz_joint_names), l_joint_ite);
		};
		
		for(auto l_anim_ite = 0U; l_anim_ite < p_ai_animation_count; ++l_anim_ite)
		{
			const auto& l_ai_animation = p_ai_animations[l_anim_ite];
			auto& l_ozz_animation = p_ozz_animations[l_anim_ite];
			
			l_ozz_animation.name = l_ai_animation->mName.C_Str();
			l_ozz_animation.duration = static_cast<bcFLOAT>(l_ai_animation->mDuration / l_ai_animation->mTicksPerSecond);
			l_ozz_animation.tracks.resize(l_ozz_joint_names.size());
			
			for(auto l_anim_channel_ite = 0U; l_anim_channel_ite < l_ai_animation->mNumChannels; ++l_anim_channel_ite)
			{
				const auto* l_ai_channel = l_ai_animation->mChannels[l_anim_channel_ite];
				const auto l_ozz_animation_index = l_find_joint_index(l_ai_channel->mNodeName.C_Str());
				auto& l_ozz_track = l_ozz_animation.tracks[l_ozz_animation_index];
				
				for(auto l_ai_channel_position_ite = 0U; l_ai_channel_position_ite < l_ai_channel->mNumPositionKeys; ++l_ai_channel_position_ite)
				{
					l_ozz_track.translations.resize(l_ai_channel->mNumPositionKeys);

					const auto& l_ai_position = l_ai_channel->mPositionKeys[l_ai_channel_position_ite];
					l_ozz_track.translations[l_ai_channel_position_ite].time = static_cast<bcFLOAT>(l_ai_position.mTime / l_ai_animation->mTicksPerSecond);
					l_ozz_track.translations[l_ai_channel_position_ite].value = ozz::math::Float3(l_ai_position.mValue.x, l_ai_position.mValue.y, l_ai_position.mValue.z);
				}
				for (auto l_ai_channel_scale_ite = 0U; l_ai_channel_scale_ite < l_ai_channel->mNumScalingKeys; ++l_ai_channel_scale_ite)
				{
					l_ozz_track.scales.resize(l_ai_channel->mNumScalingKeys);

					const auto& l_ai_scale = l_ai_channel->mScalingKeys[l_ai_channel_scale_ite];
					l_ozz_track.scales[l_ai_channel_scale_ite].time = static_cast<bcFLOAT>(l_ai_scale.mTime / l_ai_animation->mTicksPerSecond);
					l_ozz_track.scales[l_ai_channel_scale_ite].value = ozz::math::Float3(l_ai_scale.mValue.x, l_ai_scale.mValue.y, l_ai_scale.mValue.z);
				}
				for (auto l_ai_channel_rotation_ite = 0U; l_ai_channel_rotation_ite < l_ai_channel->mNumRotationKeys; ++l_ai_channel_rotation_ite)
				{
					l_ozz_track.rotations.resize(l_ai_channel->mNumRotationKeys);

					const auto& l_ai_rotation = l_ai_channel->mRotationKeys[l_ai_channel_rotation_ite];
					l_ozz_track.rotations[l_ai_channel_rotation_ite].time = static_cast<bcFLOAT>(l_ai_rotation.mTime / l_ai_animation->mTicksPerSecond);
					l_ozz_track.rotations[l_ai_channel_rotation_ite].value = ozz::math::Quaternion(l_ai_rotation.mValue.x, l_ai_rotation.mValue.y, l_ai_rotation.mValue.z, l_ai_rotation.mValue.w);
				}
			}
		}
	}

	void _write_ozz_structures(const ozz::animation::Skeleton& p_ozz_skeleton,
		const core::bc_vector_frame< ozz::unique_ptr< ozz::animation::Animation>>& p_ozz_animations,
		ozz::io::MemoryStream& p_stream)
	{
		auto l_num_animations = p_ozz_animations.size();
		p_stream.Write(&l_num_animations, sizeof(decltype(l_num_animations)));

		ozz::io::OArchive l_ozz_archive(&p_stream);
		l_ozz_archive << p_ozz_skeleton;

		for(auto& l_ozz_animation : p_ozz_animations)
		{
			l_ozz_archive << *l_ozz_animation;
		}
	}

	void _read_ozz_structures(ozz::io::MemoryStream& p_stream, game::bc_animation_skeleton& p_skeleton, core::bc_vector<game::bc_skeleton_animation>& p_animations)
	{
		bcSIZE l_num_animations;
		p_stream.Read(&l_num_animations, sizeof(decltype(l_num_animations)));

		ozz::io::IArchive l_ozz_archive(&p_stream);
		
		if(!l_ozz_archive.TestTag< ozz::animation::Skeleton>())
		{
			throw bc_io_exception("Cannot read animation skeleton from input file");
		}

		l_ozz_archive >> p_skeleton.get_native_handle();
		
		p_animations.resize(l_num_animations);
		
		for(auto l_anim_ite = 0U; l_anim_ite < l_num_animations; ++l_anim_ite)
		{
			if (!l_ozz_archive.TestTag< ozz::animation::Animation>())
			{
				throw bc_io_exception("Cannot read animation from input file");
			}

			l_ozz_archive >> p_animations[l_anim_ite].get_native_handle();
		}
	}
	
	bool bc_skinned_animation_loader::support_offline_processing() const
	{
		return true;
	}

	void bc_skinned_animation_loader::content_offline_processing(core::bc_content_loading_context& p_context) const
	{
		Assimp::Importer l_importer;

		const aiScene* const* l_ai_scene_value = nullptr;
		json_parse::bc_load(p_context.m_instance_parameters, "aiScene", l_ai_scene_value);

		const auto* l_ai_scene = l_ai_scene_value ? *l_ai_scene_value : nullptr;
		if (!l_ai_scene)
		{
			l_ai_scene = l_importer.ReadFileFromMemory
			(
				p_context.m_file_buffer.get(),
				p_context.m_file_buffer_size,
				graphic::bc_render_api_info::use_left_handed() ? aiProcess_ConvertToLeftHanded : 0
			);
		}

		if (!l_ai_scene || !l_ai_scene->HasAnimations())
		{
			const auto l_error_msg = core::bc_string_stream_frame()
				<< "Content file loading error: "
				<< p_context.m_file_path
				<< ", "
				<< l_importer.GetErrorString();
			throw bc_io_exception(l_error_msg.str().c_str());
		}

		ozz::animation::offline::RawSkeleton l_ozz_raw_skeleton;
		
		_build_ozz_skeleton(&l_ai_scene->mRootNode, 1, l_ozz_raw_skeleton.roots);

		const auto l_ozz_skeleton = ozz::animation::offline::SkeletonBuilder()(l_ozz_raw_skeleton);
		core::bc_vector_frame<ozz::animation::offline::RawAnimation> l_ozz_raw_animations;

		_build_ozz_animation(l_ai_scene->mAnimations, l_ai_scene->mNumAnimations, *l_ozz_skeleton, l_ozz_raw_animations);

		core::bc_vector_frame< ozz::unique_ptr< ozz::animation::Animation>> l_ozz_animations;
		l_ozz_animations.reserve(l_ozz_raw_animations.size());

		bool l_additive_animation = false;
		json_parse::bc_load(p_context.m_parameters, constant::g_param_animation_additive, l_additive_animation);
		
		std::transform
		(
			std::begin(l_ozz_raw_animations),
			std::end(l_ozz_raw_animations),
			std::back_inserter(l_ozz_animations),
			[l_additive_animation](ozz::animation::offline::RawAnimation& p_ozz_raw_animation)
			{
				// TODO use animation optimizer
				if(!p_ozz_raw_animation.Validate())
				{
					const auto l_msg = core::bc_string_stream_frame() << "animation '" << p_ozz_raw_animation.name.c_str() << "' is not valid.";
					throw bc_io_exception(l_msg.str().c_str());
				}

				if(l_additive_animation)
				{
					ozz::animation::offline::RawAnimation l_ozz_raw_additive_animation;
					ozz::animation::offline::AdditiveAnimationBuilder()(p_ozz_raw_animation, &l_ozz_raw_additive_animation);
					l_ozz_raw_additive_animation.name = p_ozz_raw_animation.name;
					
					p_ozz_raw_animation = l_ozz_raw_additive_animation;
				}
				
				return ozz::animation::offline::AnimationBuilder()(p_ozz_raw_animation);
			}
		);

		ozz::io::MemoryStream l_ozz_stream;		
		_write_ozz_structures(*l_ozz_skeleton, l_ozz_animations, l_ozz_stream);

		const core::bc_unique_ptr<bcBYTE> l_buffer(static_cast<bcBYTE*>(BC_ALLOC_THROW(l_ozz_stream.Size(), core::bc_alloc_type::frame)));
		l_ozz_stream.Seek(0, ozz::io::Stream::kSet);
		l_ozz_stream.Read(l_buffer.get(), l_ozz_stream.Size());
		
		p_context.m_file.write(l_buffer.get(), l_ozz_stream.Size());
	}

	void bc_skinned_animation_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
		ozz::io::MemoryStream l_ozz_stream;

		l_ozz_stream.Write(p_context.m_file_buffer.get(), p_context.m_file_buffer_size);
		l_ozz_stream.Seek(0, ozz::io::Stream::kSet);

		game::bc_animation_skeleton l_skeleton;
		core::bc_vector< game::bc_skeleton_animation > l_animations;

		_read_ozz_structures(l_ozz_stream, l_skeleton, l_animations);

		// Call get_bone_names to create its structure
		const auto l_bone_names = l_skeleton.get_joint_names();
		if(l_bone_names.empty())
		{
			throw bc_io_exception(":|"); // Just to prevent interpret bone names as unused variable
		}
		
		p_context.set_result
		(
			game::bc_skinned_animation
			(
				std::move(l_skeleton),
				std::move(l_animations)
			)
		);
	}
}