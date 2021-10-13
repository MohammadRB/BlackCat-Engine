// [05/04/2016 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "CorePlatformImp/File/bcFileInfo.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Utility/bcNullable.h"
#include "Core/File/bcPath.h"
#include "Core/bcConstant.h"
#include "Core/bcUtility.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "PhysicsImp/Fundation/bcMemoryBuffer.h"
#include "Game/System/Render/State/bcVertexLayout.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"
#include "BlackCat/Loader/bcMeshColliderLoader.h"
#include "BlackCat/Loader/bcMeshLoader.h"
#include "BlackCat/Loader/bcMeshLoaderUtility.h"

#include "3rdParty/Assimp/Include/Importer.hpp"
#include "3rdParty/Assimp/Include/postprocess.h"
#include "3rdParty/Assimp/Include/scene.h"

namespace black_cat
{
	bool bc_mesh_loader::support_offline_processing() const
	{
		return false;
	}
	
	void bc_mesh_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
		Assimp::Importer l_importer;

		const aiScene* l_scene = l_importer.ReadFileFromMemory
		(
			p_context.m_file_buffer.get(),
			p_context.m_file_buffer_size,
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			(graphic::bc_render_api_info::use_left_handed() ? aiProcess_ConvertToLeftHanded : 0)
		);
		if (!l_scene || !l_scene->HasMeshes())
		{
			const auto l_error_msg =
				core::bc_string_frame("Content file loading error: ")
				+
				core::bc_to_string_frame(p_context.m_file_path).c_str()
				+
				", "
				+
				l_importer.GetErrorString();
			throw bc_io_exception(l_error_msg.c_str());
		}

		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();
		auto& l_content_manager = *core::bc_get_service<core::bc_content_manager>();

		game::bc_mesh_builder l_builder;
		core::bc_unordered_map_frame<const bcCHAR*, bcUINT32> l_node_mapping;
		
		bc_mesh_loader_utility::calculate_node_mapping(*l_scene->mRootNode, l_node_mapping);
		
		convert_ai_nodes(l_game_system.get_render_system(), p_context, *l_scene, l_node_mapping, *l_scene->mRootNode, l_builder);

		const auto l_mesh_name = core::bc_to_exclusive_string(core::bc_path(p_context.m_file_path).get_filename());
		const auto* l_auto_scale = p_context.m_parameters->get_value<bcFLOAT>(constant::g_param_mesh_auto_scale);

		if(l_auto_scale)
		{
			l_builder.with_auto_scale(*l_auto_scale);
		}

		core::bc_path l_file_path(p_context.m_file_path);
		core::bc_path l_lod_paths[3]
		{
			core::bc_path(l_file_path).set_filename((l_file_path.get_filename_without_extension_frame() + bcL(".lod1") + l_file_path.get_file_extension_frame()).c_str()),
			core::bc_path(l_file_path).set_filename((l_file_path.get_filename_without_extension_frame() + bcL(".lod2") + l_file_path.get_file_extension_frame()).c_str()),
			core::bc_path(l_file_path).set_filename((l_file_path.get_filename_without_extension_frame() + bcL(".lod3") + l_file_path.get_file_extension_frame()).c_str())
		};

		for(auto& l_lod_path : l_lod_paths)
		{
			if(!l_lod_path.exist())
			{
				continue;
			}
			
			auto l_lod_mesh = l_content_manager.load<game::bc_mesh>
			(
				p_context.get_allocator_alloc_type(),
				l_lod_path.get_string_frame().c_str(),
				p_context.m_file_variant,
				*p_context.m_parameters,
				core::bc_content_loader_parameter(core::bc_alloc_type::frame).add_or_update(constant::g_param_mesh_is_lod, true)
			);

			l_builder.with_lod(std::move(l_lod_mesh));
		}

		const auto* l_collider_file_name_value = p_context.m_parameters->get_value<core::bc_string>(constant::g_param_mesh_collider);
		const auto l_is_lod = bc_null_default(p_context.m_instance_parameters.get_value<bool>(constant::g_param_mesh_is_lod), false);
		core::bc_estring_frame l_collider_file_name;
		
		if (!l_collider_file_name_value)
		{
			auto l_collider_file_path = core::bc_path(l_file_path).set_filename
			(
				(l_file_path.get_filename_without_extension_frame() + bcL(".collider") + l_file_path.get_file_extension_frame()).c_str()
			);
			if (l_collider_file_path.exist())
			{
				l_collider_file_name = l_collider_file_path.get_filename_frame();
			}
		}
		else
		{
			l_collider_file_name = core::bc_to_estring_frame(*l_collider_file_name_value);
		}

		game::bc_mesh_collider_ptr l_mesh_collider;
		if(!l_collider_file_name.empty() && !l_is_lod)
		{
			auto l_collider_file_path = core::bc_path(l_file_path).set_filename(l_collider_file_name.c_str());
			
			const aiScene* l_collider_ai_scene = nullptr;
			if (l_collider_file_name == l_file_path.get_filename_frame())
			{
				l_collider_ai_scene = l_scene;
			}

			l_mesh_collider = l_content_manager.load<game::bc_mesh_collider>
			(
				p_context.get_allocator_alloc_type(),
				l_collider_file_path.get_string_frame().c_str(),
				p_context.m_file_variant,
				*p_context.m_parameters,
				core::bc_content_loader_parameter(core::bc_alloc_type::frame).add_or_update("aiScene", l_collider_ai_scene)
			);
		}
		else
		{
			l_mesh_collider = l_content_manager.store_content((l_mesh_name + ".collider").c_str(), game::bc_mesh_collider());
		}
		
		p_context.set_result(l_builder.build(l_mesh_name.c_str(), std::move(l_mesh_collider)));
	}

	void bc_mesh_loader::fill_skinned_vertices(const aiMesh& p_ai_mesh,
		const core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_mapping,
		core::bc_vector_movable<game::bc_vertex_pos_tex_nor_tan_bon>& p_vertices,
		game::bc_mesh_builder& p_builder)
	{
		for (auto l_bone_ite = 0U; l_bone_ite < p_ai_mesh.mNumBones; ++l_bone_ite)
		{
			const auto* l_ai_bone = p_ai_mesh.mBones[l_bone_ite];

			core::bc_matrix4f l_bone_offset;
			bc_mesh_loader_utility::convert_ai_matrix(l_ai_bone->mOffsetMatrix, l_bone_offset);
			p_builder.add_bone(l_ai_bone->mName.C_Str(), l_bone_offset);

			for (auto l_bone_weight_ite = 0U; l_bone_weight_ite < l_ai_bone->mNumWeights; ++l_bone_weight_ite)
			{
				const auto& l_ai_bone_weight = l_ai_bone->mWeights[l_bone_weight_ite];
				const auto l_bone_index = p_node_mapping.find(l_ai_bone->mName.C_Str())->second;
				const auto l_bone_weight = l_ai_bone_weight.mWeight;

				auto& l_vertex_ids = p_vertices[l_ai_bone_weight.mVertexId].m_bone_indices;
				auto& l_vertex_weights = p_vertices[l_ai_bone_weight.mVertexId].m_bone_weights;

				bc_mesh_loader_utility::store_skinned_vertex_weights(l_vertex_ids, l_vertex_weights, l_bone_index, l_bone_weight);
			}
		}
	}

	void bc_mesh_loader::convert_ai_material(core::bc_content_loading_context& p_context,
		const aiMaterial& p_ai_material,
		game::bc_mesh_material_description& p_material)
	{
		auto* l_content_manager = core::bc_get_service<core::bc_content_manager>();

		aiColor3D l_diffuse;
		bcFLOAT l_alpha = 1;
		bcFLOAT l_specular_intensity = 1;
		bcFLOAT l_specular_power = 1;

		p_ai_material.Get(AI_MATKEY_COLOR_DIFFUSE, l_diffuse);
		p_ai_material.Get(AI_MATKEY_SHININESS_STRENGTH, l_specular_intensity);
		p_ai_material.Get(AI_MATKEY_SHININESS, l_specular_power);
		p_ai_material.Get(AI_MATKEY_OPACITY, l_alpha);

		p_material.m_diffuse = core::bc_vector4f(l_diffuse.r, l_diffuse.g, l_diffuse.b, l_alpha);
		p_material.m_specular_intensity = l_specular_intensity;
		p_material.m_specular_power = l_specular_power;

		const auto l_root_path = core::bc_path(p_context.m_file_path).set_filename(bcL(""));
		core::bc_nullable<core::bc_path> l_diffuse_file_name;
		aiString l_aistr;

		if (p_ai_material.GetTexture(aiTextureType_DIFFUSE, 0, &l_aistr) == aiReturn_SUCCESS)
		{
			l_diffuse_file_name = core::bc_path(core::bc_to_exclusive_wstring(l_aistr.C_Str()).c_str());
			p_material.m_diffuse_map = l_content_manager->load< graphic::bc_texture2d_content >
			(
				p_context.get_allocator_alloc_type(),
				core::bc_path(l_root_path).set_filename(l_diffuse_file_name->get_string().c_str()).get_string().c_str(),
				nullptr,
				*p_context.m_parameters
			);
		}

		core::bc_estring l_normal_map_path;
		core::bc_estring l_specular_map_path;

		if (p_ai_material.GetTexture(aiTextureType_NORMALS, 0, &l_aistr) == aiReturn_SUCCESS)
		{
			l_normal_map_path = core::bc_path(l_root_path).set_filename(core::bc_to_exclusive_wstring(l_aistr.C_Str()).c_str()).get_string();
		}
		else if (l_diffuse_file_name != nullptr)
		{
			const auto l_conventional_normal_map_name = l_diffuse_file_name->get_filename_without_extension() + L".nrm" + l_diffuse_file_name->get_file_extension();
			auto l_normal_map = core::bc_path(l_root_path).set_filename(l_conventional_normal_map_name.c_str()).get_string();

			if (core_platform::bc_file_info::exist(l_normal_map.c_str()))
			{
				l_normal_map_path = std::move(l_normal_map);
			}
		}

		if (p_ai_material.GetTexture(aiTextureType_SPECULAR, 0, &l_aistr) == aiReturn_SUCCESS)
		{
			l_specular_map_path = core::bc_path(l_root_path).set_filename(core::bc_to_exclusive_wstring(l_aistr.C_Str()).c_str()).get_string();
		}
		else if (l_diffuse_file_name != nullptr)
		{
			const auto l_conventional_specular_map_name = l_diffuse_file_name->get_filename_without_extension() + L".spec" + l_diffuse_file_name->get_file_extension();
			auto l_specular_map = core::bc_path(l_root_path).set_filename(l_conventional_specular_map_name.c_str()).get_string();

			if (core_platform::bc_file_info::exist(l_specular_map.c_str()))
			{
				l_specular_map_path = std::move(l_specular_map);
			}
		}

		if (!l_normal_map_path.empty())
		{
			p_material.m_normal_map = l_content_manager->load<graphic::bc_texture2d_content>
			(
				p_context.get_allocator_alloc_type(),
				l_normal_map_path.c_str(),
				nullptr,
				*p_context.m_parameters
			);
		}
		if (!l_specular_map_path.empty())
		{
			p_material.m_specular_map = l_content_manager->load<graphic::bc_texture2d_content>
			(
				p_context.get_allocator_alloc_type(),
				l_specular_map_path.c_str(),
				nullptr,
				*p_context.m_parameters
			);
		}
	}

	void bc_mesh_loader::convert_ai_mesh(game::bc_render_system& p_render_system,
		core::bc_content_loading_context& p_context,
		const aiScene& p_ai_scene,
		const core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_mapping,
		const aiNode& p_ai_node,
		const aiMesh& p_ai_mesh,
		game::bc_mesh_builder& p_builder)
	{
		auto& l_device = p_render_system.get_device();
		core::bc_vector_movable< game::bc_vertex_pos_tex_nor_tan > l_vertices;
		core::bc_vector_movable< game::bc_vertex_pos_tex_nor_tan_bon > l_skinned_vertices;
		core::bc_vector_movable< bcUINT16 > l_16bit_indices;
		core::bc_vector_movable< bcUINT32 > l_32bit_indices;
		graphic::bc_buffer_ref l_vertex_buffer;
		graphic::bc_buffer_ref l_index_buffer;
		bcSIZE l_index_count = 0;
		graphic::bc_buffer_ref l_cbuffer;
		game::bc_mesh_material_description l_material;

		bool l_need_32bit_indices = p_ai_mesh.mNumFaces * 3 > std::numeric_limits<bcUINT16>::max();
		bool l_has_texcoord = p_ai_mesh.HasTextureCoords(0);
		bool l_has_normal = p_ai_mesh.HasNormals();
		bool l_has_tangent = p_ai_mesh.HasTangentsAndBitangents();
		bool l_has_skinned = p_ai_mesh.HasBones() && bc_null_default(p_context.m_parameters->get_value<bool>(constant::g_param_mesh_skinned), false);

		if (l_has_skinned)
		{
			l_skinned_vertices.resize(p_ai_mesh.mNumVertices);
		}
		else
		{
			l_vertices.resize(p_ai_mesh.mNumVertices);
		}

		if(l_need_32bit_indices)
		{
			l_32bit_indices.resize(p_ai_mesh.mNumFaces * 3);
		}
		else
		{
			l_16bit_indices.resize(p_ai_mesh.mNumFaces * 3);
		}

		for (bcUINT l_vertex = 0; l_vertex < p_ai_mesh.mNumVertices; ++l_vertex)
		{
			auto* l_ai_vertex = &p_ai_mesh.mVertices[l_vertex];
			auto* l_ai_texcoord = l_has_texcoord ? &p_ai_mesh.mTextureCoords[0][l_vertex] : nullptr;
			auto* l_ai_normal = l_has_normal ? &p_ai_mesh.mNormals[l_vertex] : nullptr;
			auto* l_ai_tangent = l_has_tangent ? &p_ai_mesh.mTangents[l_vertex] : nullptr;

			core::bc_vector3f* l_position;
			core::bc_vector2f* l_texcoord;
			core::bc_vector3f* l_normal;
			core::bc_vector3f* l_tangent;

			if (l_has_skinned)
			{
				l_position = &l_skinned_vertices[l_vertex].m_position;
				l_texcoord = &l_skinned_vertices[l_vertex].m_texcoord;
				l_normal = &l_skinned_vertices[l_vertex].m_normal;
				l_tangent = &l_skinned_vertices[l_vertex].m_tangent;
				l_skinned_vertices[l_vertex].m_bone_indices = core::bc_vector4i(0);
				l_skinned_vertices[l_vertex].m_bone_weights = core::bc_vector4f(0);
			}
			else
			{
				l_position = &l_vertices[l_vertex].m_position;
				l_texcoord = &l_vertices[l_vertex].m_texcoord;
				l_normal = &l_vertices[l_vertex].m_normal;
				l_tangent = &l_vertices[l_vertex].m_tangent;
			}

			*l_position = core::bc_vector3f(l_ai_vertex->x, l_ai_vertex->y, l_ai_vertex->z);
			*l_texcoord = l_ai_texcoord ? core::bc_vector2f(l_ai_texcoord->x, l_ai_texcoord->y) : core::bc_vector2f();
			*l_normal = l_ai_normal ? core::bc_vector3f(l_ai_normal->x, l_ai_normal->y, l_ai_normal->z) : core::bc_vector3f();
			*l_tangent = l_ai_tangent ? core::bc_vector3f(l_ai_tangent->x, l_ai_tangent->y, l_ai_tangent->z) : core::bc_vector3f();
		}

		if (l_has_skinned)
		{
			fill_skinned_vertices(p_ai_mesh, p_node_mapping, l_skinned_vertices, p_builder);
		}

		auto* l_16bit_index = l_16bit_indices.data();
		auto* l_32bit_index = l_32bit_indices.data();

		for (bcUINT l_face_index = 0; l_face_index < p_ai_mesh.mNumFaces; ++l_face_index)
		{
			for (bcBYTE l_index = 0; l_index < 3; ++l_index)
			{
				if (l_need_32bit_indices)
				{
					*l_32bit_index = static_cast<bcUINT32>(p_ai_mesh.mFaces[l_face_index].mIndices[l_index]);
					++l_32bit_index;
				}
				else
				{
					*l_16bit_index = static_cast<bcUINT16>(p_ai_mesh.mFaces[l_face_index].mIndices[l_index]);
					++l_16bit_index;
				}

				++l_index_count;
			}
		}

		auto l_vertex_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer
			(
				l_has_skinned ? l_skinned_vertices.size() : l_vertices.size(),
				l_has_skinned ? sizeof(decltype(l_skinned_vertices)::value_type) : sizeof(decltype(l_vertices)::value_type),
				graphic::bc_resource_usage::gpu_r,
				graphic::bc_resource_view_type::none,
				false
			).as_vertex_buffer();
		auto l_index_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer
			(
				l_index_count,
				l_need_32bit_indices ? sizeof(bcINT32) : sizeof(bcINT16),
				graphic::bc_resource_usage::gpu_r,
				graphic::bc_resource_view_type::none,
				false
			).as_index_buffer();
		auto l_vertex_buffer_data = graphic::bc_subresource_data(l_has_skinned ? static_cast<void*>(l_skinned_vertices.data()) : static_cast<void*>(l_vertices.data()), 0, 0);
		auto l_index_buffer_data = graphic::bc_subresource_data(l_need_32bit_indices ? static_cast<void*>(l_32bit_indices.data()) : static_cast<void*>(l_16bit_indices.data()), 0, 0);

		l_vertex_buffer = l_device.create_buffer(l_vertex_buffer_config, &l_vertex_buffer_data);
		l_index_buffer = l_device.create_buffer(l_index_buffer_config, &l_index_buffer_data);

		convert_ai_material(p_context, *p_ai_scene.mMaterials[p_ai_mesh.mMaterialIndex], l_material);

		auto l_material_name = core::bc_to_exclusive_string(core::bc_path(p_context.m_file.get_path().c_str()).get_filename()) + "." + p_ai_mesh.mName.C_Str();
		auto l_material_ptr = p_render_system.get_material_manager().store_mesh_material
		(
			p_context.get_allocator_alloc_type(),
			l_material_name.c_str(),
			std::move(l_material)
		);

		auto l_mesh_render_state = p_render_system.create_render_state
		(
			graphic::bc_primitive::trianglelist,
			l_vertex_buffer.get(),
			l_has_skinned ? sizeof(decltype(l_skinned_vertices)::value_type) : sizeof(decltype(l_vertices)::value_type),
			0,
			l_index_buffer.get(),
			l_need_32bit_indices ? game::bc_index_type::i32bit : game::bc_index_type::i16bit,
			l_index_count,
			0,
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_material_ptr->get_diffuse_map_view()),
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_material_ptr->get_normal_map_view()),
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_material_ptr->get_specular_map_view()),
			},
			{
				l_has_skinned ? p_render_system.get_per_skinned_object_cbuffer() : p_render_system.get_per_object_cbuffer(),
				graphic::bc_constant_buffer_parameter
				(
					1,
					core::bc_enum::mask_or({graphic::bc_shader_type::vertex, graphic::bc_shader_type::pixel}),
					l_material_ptr->get_parameters_cbuffer()
				)
			}
		);

		if (l_has_skinned)
		{
			auto l_bound_box = game::bc_extract_bound_box_from_points
			(
				physics::bc_bounded_strided_typed_data<core::bc_vector3f>
				(
					&l_skinned_vertices[0].m_position,
					sizeof(game::bc_vertex_pos_tex_nor_tan_bon),
					l_skinned_vertices.size()
				)
			);

			p_builder.add_skinned_mesh_part
			(
				p_ai_node.mName.C_Str(),
				p_ai_mesh.mName.C_Str(),
				std::move(l_material_ptr),
				std::move(l_skinned_vertices),
				std::move(l_16bit_indices),
				std::move(l_32bit_indices),
				l_bound_box,
				std::move(l_vertex_buffer),
				std::move(l_index_buffer),
				std::move(l_mesh_render_state)
			);
		}
		else
		{
			auto l_bound_box = game::bc_extract_bound_box_from_points
			(
				physics::bc_bounded_strided_typed_data<core::bc_vector3f>
				(
					&l_vertices[0].m_position,
					sizeof(game::bc_vertex_pos_tex_nor_tan),
					l_vertices.size()
				)
			);

			p_builder.add_mesh_part
			(
				p_ai_node.mName.C_Str(),
				p_ai_mesh.mName.C_Str(),
				std::move(l_material_ptr),
				std::move(l_vertices),
				std::move(l_16bit_indices),
				std::move(l_32bit_indices),
				l_bound_box,
				std::move(l_vertex_buffer),
				std::move(l_index_buffer),
				std::move(l_mesh_render_state)
			);
		}
	}

	void bc_mesh_loader::convert_ai_nodes(game::bc_render_system& p_render_system,
		core::bc_content_loading_context& p_context,
		const aiScene& p_ai_scene,
		const core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_mapping,
		const aiNode& p_ai_node,
		game::bc_mesh_builder& p_builder)
	{
		if (bc_mesh_loader_utility::is_px_node(p_ai_node))
		{
			return;
		}

		core::bc_matrix4f l_node_transformation;
		bc_mesh_loader_utility::convert_ai_matrix(p_ai_node.mTransformation, l_node_transformation);

		p_builder.add_node
		(
			p_ai_node.mParent ? p_ai_node.mParent->mName.C_Str() : nullptr,
			p_ai_node.mName.C_Str(),
			l_node_transformation
		);

		for (bcUINT32 i = 0; i < p_ai_node.mNumMeshes; ++i)
		{
			aiMesh* l_ai_mesh = p_ai_scene.mMeshes[p_ai_node.mMeshes[i]];

			convert_ai_mesh
			(
				p_render_system,
				p_context,
				p_ai_scene,
				p_node_mapping,
				p_ai_node,
				*l_ai_mesh,
				p_builder
			);
		}

		for (bcUINT l_child_index = 0; l_child_index < p_ai_node.mNumChildren; ++l_child_index)
		{
			aiNode* l_child = p_ai_node.mChildren[l_child_index];
			convert_ai_nodes(p_render_system, p_context, p_ai_scene, p_node_mapping, *l_child, p_builder);
		}
	}
}