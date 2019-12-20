// [05/04/2016 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "CorePlatformImp/File/bcFileInfo.h"
#include "Core/bcConstant.h"
#include "Core/File/bcPath.h"
#include "Core/Utility/bcNullable.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcArray.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "PhysicsImp/Fundation/bcMemoryBuffer.h"
#include "Game/System/Render/State/bcVertexLayout.h"
#include "Game/System/Render/bcMaterialManager.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"
#include "BlackCat/Loader/bcMeshColliderLoader.h"
#include "BlackCat/Loader/bcMeshLoader.h"

namespace black_cat
{
	void bc_mesh_loader::calculate_node_mesh_count(const aiNode& p_node, bcSIZE& p_node_count, bcSIZE& p_mesh_count)
	{
		if (bc_mesh_collider_loader::is_px_node(p_node))
		{
			return;
		}

		++p_node_count;
		p_mesh_count += p_node.mNumMeshes;

		for (bcUINT32 i = 0; i < p_node.mNumChildren; ++i)
		{
			calculate_node_mesh_count(*p_node.mChildren[i], p_node_count, p_mesh_count);
		}
	}

	void bc_mesh_loader::convert_aimatrix(const aiMatrix4x4& p_aimatrix, core::bc_matrix4f& p_matrix)
	{
		aiMatrix4x4 l_aimatrix = p_aimatrix;
		l_aimatrix.Transpose();

		p_matrix[0] = l_aimatrix.a1;
		p_matrix[1] = l_aimatrix.a2;
		p_matrix[2] = l_aimatrix.a3;
		p_matrix[3] = l_aimatrix.a4;
		p_matrix[4] = l_aimatrix.b1;
		p_matrix[5] = l_aimatrix.b2;
		p_matrix[6] = l_aimatrix.b3;
		p_matrix[7] = l_aimatrix.b4;
		p_matrix[8] = l_aimatrix.c1;
		p_matrix[9] = l_aimatrix.c2;
		p_matrix[10] = l_aimatrix.c3;
		p_matrix[11] = l_aimatrix.c4;
		p_matrix[12] = l_aimatrix.d1;
		p_matrix[13] = l_aimatrix.d2;
		p_matrix[14] = l_aimatrix.d3;
		p_matrix[15] = l_aimatrix.d4;
	}

	void bc_mesh_loader::convert_aimaterial(core::bc_content_loading_context& p_context, const aiMaterial& p_aimaterial, game::bc_render_material_description& p_material)
	{
		auto* l_content_manager = core::bc_get_service< core::bc_content_manager >();

		aiColor3D l_diffuse;
		bcFLOAT l_alpha = 1;
		bcFLOAT l_specular_intensity = 1;
		bcFLOAT l_specular_power = 1;

		p_aimaterial.Get(AI_MATKEY_COLOR_DIFFUSE, l_diffuse);
		p_aimaterial.Get(AI_MATKEY_SHININESS_STRENGTH, l_specular_intensity);
		p_aimaterial.Get(AI_MATKEY_SHININESS, l_specular_power);
		p_aimaterial.Get(AI_MATKEY_OPACITY, l_alpha);

		p_material.m_diffuse = core::bc_vector4f(l_diffuse.r, l_diffuse.g, l_diffuse.b, l_alpha);
		p_material.m_specular_intensity = l_specular_intensity;
		p_material.m_specular_power = l_specular_power;

		const auto l_root_path = core::bc_path(p_context.m_file_path.c_str()).set_filename(bcL(""));
		core::bc_nullable<core::bc_path> l_diffuse_file_name;
		aiString l_aistr;

		if (p_aimaterial.GetTexture(aiTextureType_DIFFUSE, 0, &l_aistr) == aiReturn_SUCCESS)
		{
			l_diffuse_file_name = core::bc_path(core::bc_to_exclusive_wstring(l_aistr.C_Str()).c_str());
			p_material.m_diffuse_map = l_content_manager->load< graphic::bc_texture2d_content >
			(
				p_context.get_allocator_alloc_type(),
				core::bc_path(l_root_path).set_filename(l_diffuse_file_name->get_path().c_str()).get_path().c_str(),
				core::bc_content_loader_parameter(p_context.m_parameter)
			);
		}

		core::bc_estring l_normal_map_path;
		core::bc_estring l_specular_map_path;

		if (p_aimaterial.GetTexture(aiTextureType_NORMALS, 0, &l_aistr) == aiReturn_SUCCESS)
		{
			l_normal_map_path = core::bc_path(l_root_path).set_filename(core::bc_to_exclusive_wstring(l_aistr.C_Str()).c_str()).get_path();
		}
		else if(l_diffuse_file_name != nullptr)
		{
			const auto l_conventional_normal_map_name = l_diffuse_file_name->get_filename_without_extension() + L"_NRM" + l_diffuse_file_name->get_file_extension();
			auto l_normal_map = core::bc_path(l_root_path).set_filename(l_conventional_normal_map_name.c_str()).get_path();
			
			if(core_platform::bc_file_info::exist(l_normal_map.c_str()))
			{
				l_normal_map_path = std::move(l_normal_map);
			}
		}

		if (p_aimaterial.GetTexture(aiTextureType_SPECULAR, 0, &l_aistr) == aiReturn_SUCCESS)
		{
			l_specular_map_path = core::bc_path(l_root_path).set_filename(core::bc_to_exclusive_wstring(l_aistr.C_Str()).c_str()).get_path();
		}
		else if (l_diffuse_file_name != nullptr)
		{
			const auto l_conventional_specular_map_name = l_diffuse_file_name->get_filename_without_extension() + L"_SPEC" + l_diffuse_file_name->get_file_extension();
			auto l_specular_map = core::bc_path(l_root_path).set_filename(l_conventional_specular_map_name.c_str()).get_path();

			if (core_platform::bc_file_info::exist(l_specular_map.c_str()))
			{
				l_specular_map_path = std::move(l_specular_map);
			}
		}

		if (!l_normal_map_path.empty())
		{
			p_material.m_normal_map = l_content_manager->load< graphic::bc_texture2d_content >
			(
				p_context.get_allocator_alloc_type(),
				l_normal_map_path.c_str(),
				core::bc_content_loader_parameter(p_context.m_parameter)
			);
		}
		if (!l_specular_map_path.empty())
		{
			p_material.m_specular_map = l_content_manager->load< graphic::bc_texture2d_content >
			(
				p_context.get_allocator_alloc_type(),
				l_specular_map_path.c_str(),
				core::bc_content_loader_parameter(p_context.m_parameter)
			);
		}
	}

	void bc_mesh_loader::convert_aimesh(game::bc_render_system& p_render_system,
		core::bc_content_loading_context& p_context,
		const aiScene& p_aiscene,
		const aiNode& p_ainode,
		const aiMesh& p_aimesh,
		game::bc_mesh_part_data& p_mesh,
		game::bc_render_state_ptr& p_mesh_render_state)
	{
		auto& l_device = p_render_system.get_device();
		core::bc_vector_movale< game::bc_vertex_pos_tex_nor_tan > l_vertices;
		core::bc_vector_movale< bcBYTE > l_indices;
		graphic::bc_graphic_resource_builder l_resource_configure;
		graphic::bc_buffer_ptr l_vertex_buffer;
		graphic::bc_buffer_ptr l_index_buffer;
		bcSIZE l_index_count = 0;
		graphic::bc_buffer_ptr l_cbuffer;
		game::bc_render_material_description l_material;

		bool l_need_32bit_indices = p_aimesh.mNumFaces * 3 > std::numeric_limits< bcUINT16 >::max();
		bool l_has_texcoord = p_aimesh.HasTextureCoords(0);
		bool l_has_normal = p_aimesh.HasNormals();
		bool l_has_tangent = p_aimesh.HasTangentsAndBitangents();

		l_vertices.reserve(p_aimesh.mNumVertices);
		l_indices.reserve(p_aimesh.mNumFaces * 3 * (l_need_32bit_indices ? static_cast< bcINT >(game::bc_index_type::i32bit) : static_cast< bcINT >(game::bc_index_type::i16bit)));

		for (bcUINT l_vertex = 0; l_vertex < p_aimesh.mNumVertices; ++l_vertex)
		{
			game::bc_vertex_pos_tex_nor_tan l_vertex_layout;
			auto* l_aivertex = &p_aimesh.mVertices[l_vertex];
			auto* l_aitexcoord = l_has_texcoord ? &p_aimesh.mTextureCoords[0][l_vertex] : nullptr;
			auto* l_ainoraml = l_has_normal ? &p_aimesh.mNormals[l_vertex] : nullptr;
			auto* l_aitangent = l_has_tangent ? &p_aimesh.mTangents[l_vertex] : nullptr;

			l_vertex_layout.m_position = core::bc_vector3f(l_aivertex->x, l_aivertex->y, l_aivertex->z);
			l_vertex_layout.m_texcoord = l_aitexcoord ? core::bc_vector2f(l_aitexcoord->x, l_aitexcoord->y) : core::bc_vector2f();
			l_vertex_layout.m_normal = l_ainoraml ? core::bc_vector3f(l_ainoraml->x, l_ainoraml->y, l_ainoraml->z) : core::bc_vector3f();
			l_vertex_layout.m_tangent = l_aitangent ? core::bc_vector3f(l_aitangent->x, l_aitangent->y, l_aitangent->z) : core::bc_vector3f();

			l_vertices.push_back(l_vertex_layout);
		}

		bcUINT16* l_16bit_indices = reinterpret_cast< bcUINT16* >(l_indices.data());
		bcUINT32* l_32bit_indices = reinterpret_cast< bcUINT32* >(l_indices.data());

		for (bcUINT l_face_index = 0; l_face_index < p_aimesh.mNumFaces; ++l_face_index)
		{
			for (bcBYTE l_index = 0; l_index < 3; ++l_index)
			{
				if (l_need_32bit_indices)
				{
					*l_32bit_indices = static_cast< bcUINT32 >(p_aimesh.mFaces[l_face_index].mIndices[l_index]);
					++l_32bit_indices;
				}
				else
				{
					*l_16bit_indices = static_cast< bcUINT16 >(p_aimesh.mFaces[l_face_index].mIndices[l_index]);
					++l_16bit_indices;
				}

				++l_index_count;
			}
		}

		auto l_vertex_buffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				l_vertices.size(),
				sizeof(game::bc_vertex_pos_tex_nor_tan),
				graphic::bc_resource_usage::gpu_r,
				graphic::bc_resource_view_type::none,
				false
			).as_vertex_buffer();
		auto l_index_buffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				l_index_count,
				l_need_32bit_indices ? static_cast< bcUINT32 >(game::bc_index_type::i32bit) : static_cast< bcUINT32 >(game::bc_index_type::i16bit),
				graphic::bc_resource_usage::gpu_r,
				graphic::bc_resource_view_type::none,
				false
			).as_index_buffer();
		auto l_vertex_buffer_data = graphic::bc_subresource_data(l_vertices.data(), 0, 0);
		auto l_index_buffer_data = graphic::bc_subresource_data(l_indices.data(), 0, 0);

		l_vertex_buffer = l_device.create_buffer(l_vertex_buffer_config, &l_vertex_buffer_data);
		l_index_buffer = l_device.create_buffer(l_index_buffer_config, &l_index_buffer_data);

		convert_aimaterial(p_context, *p_aiscene.mMaterials[p_aimesh.mMaterialIndex], l_material);

		auto l_material_name = core::bc_to_exclusive_string(core::bc_path(p_context.m_file->get_path().c_str()).get_filename()) + "." + p_aimesh.mName.C_Str();
		auto l_material_ptr = p_render_system.get_material_manager().store_material
		(
			p_context.get_allocator_alloc_type(),
			l_material_name.c_str(),
			std::move(l_material)
		);

		p_mesh.m_name = p_aimesh.mName.C_Str();
		p_mesh.m_material = std::move(l_material_ptr);
		p_mesh.m_vertices = std::move(l_vertices);
		p_mesh.m_indices = std::move(l_indices);
		p_mesh.m_vertex_buffer = std::move(l_vertex_buffer);
		p_mesh.m_index_buffer = std::move(l_index_buffer);
		p_mesh.m_bound_box = game::bc_extract_bound_box_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f >
		(
			&p_mesh.m_vertices[0].m_position,
			sizeof(game::bc_vertex_pos_tex_nor_tan),
			p_mesh.m_vertices.size()
		));
		p_mesh_render_state = p_render_system.create_render_state
		(
			graphic::bc_primitive::trianglelist,
			p_mesh.m_vertex_buffer.get(),
			sizeof(game::bc_vertex_pos_tex_nor_tan),
			0,
			p_mesh.m_index_buffer.get(),
			l_need_32bit_indices ? game::bc_index_type::i32bit : game::bc_index_type::i16bit,
			l_index_count,
			0,
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, p_mesh.m_material->get_diffuse_map_view()),
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, p_mesh.m_material->get_normal_map_view()),
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, p_mesh.m_material->get_specular_map_view()),
			},
			{
				p_render_system.get_per_object_cbuffer(),
				graphic::bc_constant_buffer_parameter
				(
					1, 
					core::bc_enum::or({graphic::bc_shader_type::vertex, graphic::bc_shader_type::pixel}), 
					p_mesh.m_material->get_parameters_cbuffer()
				)
			}
		);
	}

	void bc_mesh_loader::convert_ainodes(game::bc_render_system& p_render_system,
		core::bc_content_loading_context& p_context,
		const aiScene& p_aiscene,
		const aiNode& p_ainode,
		game::bc_mesh& p_mesh,
		game::bc_mesh_node* p_parent)
	{
		if (bc_mesh_collider_loader::is_px_node(p_ainode))
		{
			return;
		}

		core::bc_matrix4f l_node_transformation;
		convert_aimatrix(p_ainode.mTransformation, l_node_transformation);
		
		core::bc_vector_frame< std::tuple< game::bc_mesh_part_data, game::bc_render_state_ptr> > l_meshes;
		l_meshes.reserve(p_ainode.mNumMeshes);

		for (bcUINT32 i = 0; i < p_ainode.mNumMeshes; ++i)
		{
			game::bc_mesh_part_data l_mesh_data;
			game::bc_render_state_ptr l_mesh_render_state;
			aiMesh* l_aimesh = p_aiscene.mMeshes[p_ainode.mMeshes[i]];

			convert_aimesh
			(
				p_render_system,
				p_context,
				p_aiscene,
				p_ainode,
				*l_aimesh,
				l_mesh_data,
				l_mesh_render_state
			);

			l_meshes.push_back(std::make_tuple(std::move(l_mesh_data), l_mesh_render_state));
		}
		
		game::bc_mesh_node* l_added_node = p_mesh._add_node
		(
			p_ainode.mName.C_Str(),
			p_parent,
			l_node_transformation,
			l_meshes
		);

		for (bcUINT l_child_index = 0; l_child_index < p_ainode.mNumChildren; ++l_child_index)
		{
			aiNode* l_child = p_ainode.mChildren[l_child_index];
			convert_ainodes(p_render_system, p_context, p_aiscene, *l_child, p_mesh, l_added_node);
		}
	}

	bool bc_mesh_loader::support_offline_processing() const
	{
		return false;
	}
	
	void bc_mesh_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
		Assimp::Importer l_importer;

		const aiScene* l_scene = l_importer.ReadFileFromMemory
		(
			reinterpret_cast< void* >(p_context.m_file_buffer.get()),
			p_context.m_file_buffer_size,
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			(graphic::bc_render_api_info::is_left_handed() ? aiProcess_ConvertToLeftHanded : 0)
		);
		if (!l_scene || !l_scene->HasMeshes())
		{
			const auto l_error_msg =
				core::bc_string_frame("Content file loading error: ")
				+
				core::bc_to_exclusive_string(p_context.m_file_path.c_str()).c_str()
				+
				", "
				+
				l_importer.GetErrorString();
			p_context.set_result(bc_io_exception(l_error_msg.c_str()));

			return;
		}

		const game::bc_mesh_collider_ptr l_mesh_colliders = core::bc_get_service< core::bc_content_manager >()->load< game::bc_mesh_collider >
		(
			p_context.get_allocator_alloc_type(),
			p_context.m_file_path.c_str(),
			std::move(core::bc_content_loader_parameter(p_context.m_parameter).add_value("aiScene", l_scene))
		);

		game::bc_game_system& l_game_system = *core::bc_get_service< game::bc_game_system >();

		bcSIZE l_node_count = 0;
		bcSIZE l_mesh_count = 0;
		calculate_node_mesh_count(*l_scene->mRootNode, l_node_count, l_mesh_count);

		const core::bc_estring l_mesh_name = core::bc_path(p_context.m_file_path.c_str()).get_filename();
		game::bc_mesh l_mesh(core::bc_to_exclusive_string(l_mesh_name), l_node_count, l_mesh_count, l_mesh_colliders);

		convert_ainodes(l_game_system.get_render_system(), p_context, *l_scene, *l_scene->mRootNode, l_mesh, nullptr);

		bcFLOAT* l_auto_scale = p_context.m_parameter.get_value<bcFLOAT>("auto_scale");
		if(l_auto_scale)
		{
			l_mesh._apply_auto_scale(*l_auto_scale);
		}

		p_context.set_result(std::move(l_mesh));
	}
}