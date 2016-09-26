// [05/04/2016 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/File/bcPath.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcArray.h"
#include "Core/File/bcContentManager.h"
#include "Graphic/Math/bcVector2f.h"
#include "Graphic/Math/bcVector3f.h"
#include "Graphic/Math/bcMatrix4f.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Resource/bcResourceConfig.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "Game/System/Render/bcVertexLayout.h"
#include "Game/System/bcGameSystem.h"
#include "BlackCat/Loader/bcMeshLoader.h"

#include "3rdParty/Assimp/include/assimp/Importer.hpp"
#include "3rdParty/Assimp/include/assimp/postprocess.h"
#include "3rdParty/Assimp/include/assimp/scene.h"

namespace black_cat
{
	void _calculate_node_mesh_count(const aiNode* p_begin, bcSIZE* p_node_count, bcSIZE* p_mesh_count);

	void _convert_aimatrix(const aiMatrix4x4& p_aimatrix, graphic::bc_matrix4f& p_matrix);

	void _convert_aimaterial(core::bc_content_loader_context& p_context, const aiMaterial& p_aimaterial, game::bc_mesh_material& p_material);

	void _convert_aimesh(game::bc_game_system* p_game_system,
		graphic::bc_device* p_device,
		core::bc_content_loader_context& p_context,
		const aiScene* p_aiscene,
		const aiNode* p_ainode,
		const aiMesh* p_aimesh,
		game::bc_mesh_data* p_mesh,
		game::bc_render_state_ptr* p_mesh_render_state);

	void _convert_ainodes(game::bc_game_system* p_game_system,
		graphic::bc_device* p_device,
		core::bc_content_loader_context& p_context,
		const aiScene* p_aiscene,
		const aiNode* p_ainode,
		game::bc_mesh* p_mesh,
		game::bc_mesh_node* p_parent);

	bc_mesh_loader::bc_mesh_loader()
	{
	}

	bc_mesh_loader::bc_mesh_loader(bc_mesh_loader&& p_other) noexcept
		: core::bc_base_content_loader(std::move(p_other))
	{
	}

	bc_mesh_loader::~bc_mesh_loader()
	{
	}

	bc_mesh_loader& bc_mesh_loader::operator=(bc_mesh_loader&& p_other) noexcept
	{
		core::bc_base_content_loader::operator=(std::move(p_other));

		return *this;
	}

	void bc_mesh_loader::content_offline_processing(core::bc_content_loader_context& p_context)
	{
	}

	void bc_mesh_loader::content_processing(core::bc_content_loader_context& p_context)
	{
		Assimp::Importer l_importer;

		const aiScene* l_scene = l_importer.ReadFileFromMemory
		(
			p_context.m_data.data(),
			p_context.m_data.size(),
				0
			/*aiProcess_GenSmoothNormals |*/
			/*aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |*/
			/*graphic::bc_render_api_info::is_left_handed() ? aiProcess_MakeLeftHanded : 0*/
		);

		if (!l_scene || !l_scene->HasMeshes())
		{
			auto l_error_msg = core::bc_string_frame("Content file doesn't include mesh data") + core::bc_to_exclusive_string(p_context.m_file_path.c_str()).c_str();
			p_context.set_result(bc_io_exception(l_error_msg.c_str()));
			return;
		}

		game::bc_game_system* l_game_system = core::bc_service_manager::get().get_service< game::bc_game_system >();
		graphic::bc_device* l_device = &l_game_system->get_render_system().get_device();

		bcSIZE l_node_count = 0;
		bcSIZE l_mesh_count = 0;
		_calculate_node_mesh_count(l_scene->mRootNode, &l_node_count, &l_mesh_count);

		core::bc_estring l_mesh_name = core::bc_path(p_context.m_file_path.c_str()).get_filename();
		game::bc_mesh l_mesh(core::bc_to_exclusive_string(l_mesh_name), l_node_count, l_mesh_count);

		_convert_ainodes(l_game_system, l_device, p_context, l_scene, l_scene->mRootNode, &l_mesh, nullptr);

		p_context.set_result(std::move(l_mesh));
	}

	void _calculate_node_mesh_count(const aiNode* p_begin, bcSIZE* p_node_count, bcSIZE* p_mesh_count)
	{
		++*p_node_count;
		*p_mesh_count += p_begin->mNumMeshes;

		for (bcUINT32 i = 0; i < p_begin->mNumChildren; ++i)
		{
			_calculate_node_mesh_count(p_begin->mChildren[i], p_node_count, p_mesh_count);
		}
	}

	void _convert_aimatrix(const aiMatrix4x4& p_aimatrix, graphic::bc_matrix4f& p_matrix)
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

	void _convert_aimaterial(core::bc_content_loader_context& p_context, const aiMaterial& p_aimaterial, game::bc_mesh_material& p_material)
	{
		core::bc_content_manager* l_content_manager = core::bc_service_manager::get().get_service< core::bc_content_manager >();

		aiColor3D l_diffuse;
		bcFLOAT l_alpha;
		bcFLOAT l_specular_intency;
		bcFLOAT l_specular_power;

		p_aimaterial.Get(AI_MATKEY_COLOR_DIFFUSE, l_diffuse);
		p_aimaterial.Get(AI_MATKEY_SHININESS_STRENGTH, l_specular_intency);
		p_aimaterial.Get(AI_MATKEY_SHININESS, l_specular_power);
		p_aimaterial.Get(AI_MATKEY_OPACITY, l_alpha);

		p_material.m_diffuse = graphic::bc_vector4f(l_diffuse.r, l_diffuse.g, l_diffuse.b, l_alpha);
		p_material.m_specular_intency = l_specular_intency;
		p_material.m_specular_power = l_specular_power;

		aiString l_aistr;
		core::bc_path l_file_path = core::bc_path(p_context.m_file_path.c_str()).set_filename(bcL(""));

		if (p_aimaterial.GetTexture(aiTextureType_DIFFUSE, 0, &l_aistr) == aiReturn_SUCCESS)
		{
			p_material.m_diffuse_map = l_content_manager->load< graphic::bc_texture2d_content >
			(
				p_context.get_allocator_alloc_type(),
				core::bc_path(l_file_path).set_filename(core::bc_to_exclusive_wstring(l_aistr.C_Str()).c_str()).get_path().c_str(),
				core::bc_content_loader_parameter(p_context.m_parameter)
			);
		}
		if (p_aimaterial.GetTexture(aiTextureType_SPECULAR, 0, &l_aistr) == aiReturn_SUCCESS)
		{
			p_material.m_specular_map = l_content_manager->load< graphic::bc_texture2d_content >
			(
				p_context.get_allocator_alloc_type(),
				core::bc_path(l_file_path).set_filename(core::bc_to_exclusive_wstring(l_aistr.C_Str()).c_str()).get_path().c_str(),
				core::bc_content_loader_parameter(p_context.m_parameter)
			);
		}
		if (p_aimaterial.GetTexture(aiTextureType_NORMALS, 0, &l_aistr) == aiReturn_SUCCESS)
		{
			p_material.m_normal_map = l_content_manager->load< graphic::bc_texture2d_content >
			(
				p_context.get_allocator_alloc_type(),
				core::bc_path(l_file_path).set_filename(core::bc_to_exclusive_wstring(l_aistr.C_Str()).c_str()).get_path().c_str(),
				core::bc_content_loader_parameter(p_context.m_parameter)
			);
		}
	}

	void _convert_aimesh(game::bc_game_system* p_game_system,
		graphic::bc_device* p_device,
		core::bc_content_loader_context& p_context,
		const aiScene* p_aiscene,
		const aiNode* p_ainode,
		const aiMesh* p_aimesh,
		game::bc_mesh_data* p_mesh,
		game::bc_render_state_ptr* p_mesh_render_state)
	{
		core::bc_vector_frame< game::bc_vertex_pos_tex_nor_tan > l_vertices;
		core::bc_vector_frame< bcBYTE > l_indices;
		graphic::bc_graphic_resource_configure l_resource_configure;
		graphic::bc_buffer_ptr l_vertex_buffer;
		graphic::bc_buffer_ptr l_index_buffer;
		bcSIZE l_index_count = 0;
		graphic::bc_buffer_ptr l_cbuffer;
		graphic::bc_resource_view_ptr l_diffuse_map_view;
		graphic::bc_resource_view_ptr l_specular_map_view;
		graphic::bc_resource_view_ptr l_normal_map_view;
		game::bc_mesh_material l_material;

		bool l_need_32bit_indices = p_aimesh->mNumFaces * 3 > std::numeric_limits< bcUINT16 >::max();
		bool l_has_texcoord = p_aimesh->HasTextureCoords(0);
		bool l_has_normal = p_aimesh->HasNormals();
		bool l_has_tangent = p_aimesh->HasTangentsAndBitangents();

		l_vertices.reserve(p_aimesh->mNumVertices);
		l_indices.reserve(p_aimesh->mNumFaces * 3 * (l_need_32bit_indices ? static_cast< bcINT >(game::bc_index_type::i32bit) : static_cast< bcINT >(game::bc_index_type::i16bit)));

		for (bcUINT l_vertex = 0; l_vertex < p_aimesh->mNumVertices; ++l_vertex)
		{
			game::bc_vertex_pos_tex_nor_tan l_vertex_layout;
			auto* l_aivertex = &p_aimesh->mVertices[l_vertex];
			auto* l_aitexcoord = l_has_texcoord ? &p_aimesh->mTextureCoords[0][l_vertex] : nullptr;
			auto* l_ainoraml = l_has_normal ? &p_aimesh->mNormals[l_vertex] : nullptr;
			auto* l_aitangent = l_has_tangent ? &p_aimesh->mTangents[l_vertex] : nullptr;

			l_vertex_layout.m_position = graphic::bc_vector3f(l_aivertex->x, l_aivertex->y, l_aivertex->z);
			l_vertex_layout.m_texcoord = l_aitexcoord ? graphic::bc_vector2f(l_aitexcoord->x, l_aitexcoord->y) : graphic::bc_vector2f();
			l_vertex_layout.m_normal = l_ainoraml ? graphic::bc_vector3f(l_ainoraml->x, l_ainoraml->y, l_ainoraml->z) : graphic::bc_vector3f();
			l_vertex_layout.m_tangent = l_aitangent ? graphic::bc_vector3f(l_aitangent->x, l_aitangent->y, l_aitangent->z) : graphic::bc_vector3f();

			l_vertices.push_back(l_vertex_layout);
		}

		bcUINT16* l_16bit_indices = reinterpret_cast< bcUINT16* >(l_indices.data());
		bcUINT32* l_32bit_indices = reinterpret_cast< bcUINT32* >(l_indices.data());

		for (bcUINT l_face_index = 0; l_face_index < p_aimesh->mNumFaces; ++l_face_index)
		{
			for (bcBYTE l_index = 2; l_index >= 0; --l_index)
			{
				if (l_need_32bit_indices)
				{
					*l_32bit_indices = static_cast< bcUINT32 >(p_aimesh->mFaces[l_face_index].mIndices[l_index]);
					++l_32bit_indices;
				}
				else
				{
					*l_16bit_indices = static_cast< bcUINT16 >(p_aimesh->mFaces[l_face_index].mIndices[l_index]);
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

		l_vertex_buffer = p_device->create_buffer(l_vertex_buffer_config, &l_vertex_buffer_data);
		l_index_buffer = p_device->create_buffer(l_index_buffer_config, &l_index_buffer_data);

		_convert_aimaterial(p_context, *p_aiscene->mMaterials[p_aimesh->mMaterialIndex], l_material);

		auto l_cbuffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				1,
				sizeof(game::bc_mesh_part_cbuffer),
				graphic::bc_resource_usage::gpu_r,
				graphic::bc_resource_view_type::none,
				false
			).as_constant_buffer();
		auto l_texture_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(graphic::bc_format::R8G8B8A8_UNORM)
			.as_tex2d_shader_view(0, -1)
			.on_texture2d();
		game::bc_mesh_part_cbuffer l_mesh_part_cbuffer
		{
			l_material.m_diffuse,
			l_material.m_specular_intency,
			l_material.m_specular_power
		};
		auto l_cbuffer_data = graphic::bc_subresource_data(&l_mesh_part_cbuffer, 0, 0);

		l_cbuffer = p_device->create_buffer(l_cbuffer_config, &l_cbuffer_data);

		l_diffuse_map_view = l_material.m_diffuse_map ?
			                     p_device->create_resource_view(l_material.m_diffuse_map->get_resource().get(), l_texture_view_config) :
			                     nullptr;
		l_specular_map_view = l_material.m_specular_map ?
			                      p_device->create_resource_view(l_material.m_specular_map->get_resource().get(), l_texture_view_config) :
			                      nullptr;
		l_normal_map_view = l_material.m_normal_map ?
			                    p_device->create_resource_view(l_material.m_normal_map->get_resource().get(), l_texture_view_config) :
			                    nullptr;

		*p_mesh_render_state = p_game_system->get_render_system().create_render_state
		(
			graphic::bc_primitive::trianglelist,
			l_vertex_buffer,
			0,
			l_index_buffer,
			l_need_32bit_indices ? game::bc_index_type::i32bit : game::bc_index_type::i16bit,
			l_index_count,
			0,
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_diffuse_map_view),
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_normal_map_view),
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_specular_map_view),
			},
			{
				graphic::bc_constant_buffer_parameter(0, graphic::bc_shader_type::pixel, l_cbuffer)
			}
		);
	}

	void _convert_ainodes(game::bc_game_system* p_game_system,
		graphic::bc_device* p_device,
		core::bc_content_loader_context& p_context,
		const aiScene* p_aiscene,
		const aiNode* p_ainode,
		game::bc_mesh* p_mesh,
		game::bc_mesh_node* p_parent)
	{
		core::bc_vector_frame<std::pair< game::bc_mesh_data, game::bc_render_state_ptr>> l_meshes;
		
		l_meshes.reserve(p_ainode->mNumMeshes);

		for(bcUINT32 i = 0; i < p_ainode->mNumMeshes; ++i)
		{
			game::bc_mesh_data l_mesh_data;
			game::bc_render_state_ptr l_mesh_render_state;

			_convert_aimesh
			(
				p_game_system,
				p_device,
				p_context,
				p_aiscene,
				p_ainode,
				p_aiscene->mMeshes[p_ainode->mMeshes[i]],
				&l_mesh_data,
				&l_mesh_render_state
			);

			l_meshes.push_back(std::make_pair(std::move(l_mesh_data), l_mesh_render_state));
		}

		graphic::bc_matrix4f l_node_transformation;
		_convert_aimatrix(p_ainode->mTransformation, l_node_transformation);

		game::bc_mesh_node* l_added_node = p_mesh->_add_node
		(
			p_ainode->mName.C_Str(),
			p_parent,
			l_node_transformation,
			std::move(l_meshes)
		);

		for (bcUINT l_child_index = 0; l_child_index < p_ainode->mNumChildren; ++l_child_index)
		{
			aiNode* l_child = p_ainode->mChildren[l_child_index];

			_convert_ainodes(p_game_system, p_device, p_context, p_aiscene, l_child, p_mesh, l_added_node);
		}
	}
}
