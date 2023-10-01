// [13/09/2016 MRB]

#include "App/AppPCH.h"

#include "CorePlatformImp/File/bcFileInfo.h"
#include "Core/bcUtility.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Math/bcVector3f.h"
#include "Core/File/bcJsonDocument.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "PhysicsImp/Fundation/bcMemoryBuffer.h"
#include "PhysicsImp/Fundation/bcSerializeBuffer.h"
#include "PhysicsImp/Shape/bcHeightField.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderThread.h"
#include "Game/System/Render/State/bcVertexLayout.h"
#include "Game/System/Render/bcRenderTask.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Render/Material/bcMeshMaterial.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "Game/bcJsonParse.h"
#include "App/bcConstant.h"
#include "App/Loader/bcHeightMapLoaderDx11.h"

#include <utility>

namespace 
{
	const black_cat::bcECHAR* g_height_map_extension = bcL("_height_map");
	const black_cat::bcECHAR* g_texture_map_extension = bcL("_texture_map");
}

namespace black_cat
{
	struct _bc_parameter_buffer
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_width;
		bcUINT32 m_height;
		bcUINT32 m_chunk_size;
		bcUINT32 m_xz_multiplier;
		BC_CBUFFER_ALIGN
		bcFLOAT m_y_multiplier;
		bcUINT32 m_scale;
		bcFLOAT m_physics_y_scale;
		bcUINT32 m_distance_detail;
		BC_CBUFFER_ALIGN
		bcUINT32 m_height_detail;
		bcUINT32 m_texture_map_width;
		bcUINT32 m_texture_map_height;
	};

	struct _bc_material_properties
	{
		BC_CBUFFER_ALIGN
		bcFLOAT m_specular_intensity[8]{ 0,0,0,0,0,0,0,0 };
		BC_CBUFFER_ALIGN
		bcFLOAT m_specular_power[8]{ 0,0,0,0,0,0,0,0 };
		BC_CBUFFER_ALIGN
		bcFLOAT m_scale[8]{ 0,0,0,0,0,0,0,0 };
	};

	class bc_height_map_texture_read_task : public game::bci_render_task
	{
	public:
		explicit bc_height_map_texture_read_task(graphic::bc_texture2d p_height_map)
			: m_height_map(p_height_map),
			m_texel_size(0)
		{
		}

		void execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_render_thread) override
		{
			auto& l_device = p_render_system.get_device();
			const auto l_width = m_height_map.get_width();
			const auto l_height = m_height_map.get_height();

			const auto l_texture_config = graphic::bc_graphic_resource_builder().as_resource()
				.as_texture2d
				(
					l_width,
					l_height,
					false,
					1,
					m_height_map.get_format(),
					graphic::bc_resource_usage::gpu_r_cpu_r,
					graphic::bc_resource_view_type::none
				)
				.as_normal_texture();
			auto l_texture = l_device.create_texture2d(l_texture_config, nullptr);
			auto l_command_list = l_device.create_command_list();

			p_render_thread.start(l_command_list.get());

			p_render_thread.copy_resource(l_texture.get(), m_height_map);

			p_render_thread.finish();

			const auto l_mapped = l_device.map_resource(l_texture.get(), 0, graphic::bc_resource_map::read);

			m_texel_size = l_mapped.m_row_pitch / l_width;
			const bcUINT32 l_buffer_size = l_width * l_height * m_texel_size;
			m_texture_buffer.reset(static_cast<bcUBYTE*>(BC_ALLOC(l_buffer_size, core::bc_alloc_type::frame)));

			void* l_src = l_mapped.m_data;
			void* l_dest = m_texture_buffer.get();

			for (bcUINT32 l_height_index = 0; l_height_index < l_height; ++l_height_index)
			{
				std::memcpy(l_dest, l_src, l_mapped.m_row_pitch);

				l_src = reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(l_src) + l_mapped.m_row_pitch);
				l_dest = reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(l_dest) + l_mapped.m_row_pitch);
			}

			l_device.unmap_resource(l_texture.get(), 0);
		}

		graphic::bc_texture2d m_height_map;
		core::bc_unique_ptr<bcUBYTE> m_texture_buffer;
		bcUINT32 m_texel_size;
	};

	bc_height_map_dx11::bc_height_map_dx11(bcUINT32 p_xz_multiplier,
		bcFLOAT p_y_multiplier,
		bcFLOAT p_physics_y_scale,
		bcUINT32 p_distance_detail,
		bcUINT32 p_height_detail,
		game::bc_render_state_ptr p_render_state,
		graphic::bc_buffer_ref p_vertex_buffer,
		graphic::bc_buffer_ref p_index_buffer,
		graphic::bc_texture2d_ref p_height_map,
		graphic::bc_resource_view_ref p_height_map_view,
		graphic::bc_texture2d_content_ptr p_texture_map,
		graphic::bc_resource_view_ref p_texture_map_view,
		graphic::bc_buffer_ref p_parameter_cbuffer,
		graphic::bc_buffer_ref p_chunk_info_buffer,
		graphic::bc_resource_view_ref p_chunk_info_view,
		graphic::bc_resource_view_ref p_chunk_info_unordered_view,
		graphic::bc_buffer_ref p_material_properties_buffer,
		core::bc_vector<game::bc_height_map_material> p_materials,
		physics::bc_height_field_ref p_px_height_map,
		void* p_px_height_map_deserialize_buffer)
		: bc_height_map
		(
			p_height_map->get_width(),
			p_height_map->get_height(),
			p_xz_multiplier,
			p_y_multiplier,
			p_physics_y_scale,
			std::move(p_render_state),
			std::move(p_vertex_buffer),
			std::move(p_index_buffer),
			std::move(p_materials),
			std::move(p_px_height_map),
			p_px_height_map_deserialize_buffer
		),
		m_distance_detail(p_distance_detail),
		m_height_detail(p_height_detail),
		m_height_map(std::move(p_height_map)),
		m_height_map_view(std::move(p_height_map_view)),
		m_texture_map(std::move(p_texture_map)),
		m_texture_map_view(std::move(p_texture_map_view)),
		m_parameter_cbuffer(std::move(p_parameter_cbuffer)),
		m_chunk_info_buffer(std::move(p_chunk_info_buffer)),
		m_chunk_info_view(std::move(p_chunk_info_view)),
		m_chunk_info_unordered_view(std::move(p_chunk_info_unordered_view)),
		m_material_properties_buffer(std::move(p_material_properties_buffer))
	{
	}

	bool bc_height_map_loader_dx11::support_offline_processing() const
	{
		return true;
	}

	void bc_height_map_loader_dx11::content_offline_processing(core::bc_content_loading_context& p_context) const
	{
		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();
		auto& l_content_manager = l_game_system.get_file_system().get_content_manager();
		auto& l_render_system = l_game_system.get_render_system();
		auto& l_physics_system = l_game_system.get_physics_system();
		auto& l_physics = l_physics_system.get_physics();

		const auto l_height_map_texture = l_content_manager.load<graphic::bc_texture2d_content>
		(
			p_context.get_allocator_alloc_type(),
			p_context.m_file_path,
			{},
			p_context.m_parameters
		);

		const auto l_width = l_height_map_texture->get_resource().get_width();
		const auto l_height = l_height_map_texture->get_resource().get_height();

		if (!bc_is_power_of_two(l_width) || !bc_is_power_of_two(l_height))
		{
			throw bc_invalid_operation_exception("Height map texture width and height must be power of 2");
		}

		if (l_width < s_chunk_size || l_height < s_chunk_size)
		{
			throw bc_invalid_operation_exception("Height map texture width and height must be greater than 64");
		}

		bc_height_map_texture_read_task l_height_map_copy_task(l_height_map_texture->get_resource());
		l_render_system.add_render_task(l_height_map_copy_task);

		_create_texture_map(p_context, l_render_system, l_width, l_height);

		const bcUINT32 l_samples_count = l_width * l_height;
		core::bc_unique_ptr<bcINT16> l_samples(static_cast<bcINT16*>(BC_ALLOC(sizeof(bcINT16) * l_samples_count, core::bc_alloc_type::frame)));

		l_height_map_copy_task.wait();

		const bcUINT32 l_texel_size = l_height_map_copy_task.m_texel_size;
		const bcUBYTE* l_texture_data = l_height_map_copy_task.m_texture_buffer.get();
		bcINT16* l_dest = l_samples.get();
		bcINT32 l_y_multiplier = 512;

		json_parse::bc_load(p_context.m_parameters, "y_multiplier", l_y_multiplier);

		for (bcUINT32 l_i = 0, l_end = l_samples_count; l_i < l_end; ++l_i)
		{
			const bcSIZE l_texel_index = l_i * l_texel_size;
			// read red channel of texture
			const bcINT16 l_value = (l_texture_data[l_texel_index] / 255.f) / l_physics_system.get_height_field_y_scale() * l_y_multiplier;

			l_dest[l_i] = l_value;
		}

		l_height_map_copy_task.m_texture_buffer.reset();

		physics::bc_height_field_ref l_height_field = _create_px_height_field(l_physics_system, l_samples.get(), l_samples_count, l_width, l_height);
		physics::bc_serialize_buffer l_serialize_buffer = l_physics.create_serialize_buffer();
		l_serialize_buffer.add(l_height_field.get());
		physics::bc_memory_buffer l_serialized_buffer = l_physics.serialize(l_serialize_buffer);

		p_context.m_file.write
		(
			static_cast<bcBYTE*>(l_serialized_buffer.get_buffer_pointer()),
			l_serialized_buffer.get_buffer_size()
		);
	}
	
	void bc_height_map_loader_dx11::content_processing(core::bc_content_loading_context& p_context) const
	{
		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();
		auto& l_content_manager = l_game_system.get_file_system().get_content_manager();
		auto& l_render_system = l_game_system.get_render_system();
		auto& l_physics_system = l_game_system.get_physics_system();
		auto& l_physics = l_physics_system.get_physics();
		auto& l_material_manager = l_render_system.get_material_manager();

		physics::bc_memory_buffer l_px_serialized_buffer = l_physics.read_to_memory_buffer(p_context.m_file_buffer.get(), p_context.m_file_buffer_size);
		physics::bc_serialize_buffer l_px_deserialized_buffer = l_physics.deserialize(l_px_serialized_buffer);
		physics::bc_height_field_ref l_px_height_map;

		l_px_deserialized_buffer.get(0, l_px_height_map.get());

		auto l_width = l_px_height_map->get_num_column();
		auto l_height = l_px_height_map->get_num_row();
		auto l_height_map_config = get_height_map_texture_config(l_width, l_height);
		auto& l_height_map_texture_config = l_height_map_config.first;
		auto& l_height_map_view_config = l_height_map_config.second;
		auto l_texture_map_config = get_texture_map_texture_config(l_width, l_height);
		auto& l_texture_map_texture_config = l_texture_map_config.first;
		auto& l_texture_map_view_config = l_texture_map_config.second;

		bool l_16bit_sample = l_height_map_texture_config.get_format() == graphic::bc_format::R16_FLOAT;
		bcUINT32 l_sample_size = l_16bit_sample ? 2 : 4;

		core::bc_unique_ptr<bcFLOAT> l_height_map_buffer(static_cast<bcFLOAT*>(BC_ALLOC(l_width * l_height * l_sample_size, core::bc_alloc_type::frame)));
		_copy_px_height_field(l_px_height_map.get(), l_physics_system.get_height_field_y_scale(), l_height_map_buffer.get());

		graphic::bc_subresource_data l_height_map_texture_data(l_height_map_buffer.get(), l_width * l_sample_size, l_width * l_height * l_sample_size);
		graphic::bc_texture2d_ref l_height_map_texture = l_render_system.get_device().create_texture2d(l_height_map_texture_config, &l_height_map_texture_data);

		const auto l_texture_map_file_path = _get_texture_map_texture_name(p_context.m_file_path);
		core::bc_json_key_value l_texture_loader_parameters;
		l_texture_loader_parameters.add_or_update(constant::g_param_texture_config, core::bc_any(l_texture_map_texture_config));

		graphic::bc_texture2d_content_ptr l_texture_map_texture = l_content_manager.load<graphic::bc_texture2d_content>
		(
			p_context.get_allocator_alloc_type(),
			l_texture_map_file_path.c_str(),
			{},
			p_context.m_parameters,
			std::move(l_texture_loader_parameters)
		);

		bcINT32 l_xz_multiplier = 1;
		bcINT32 l_y_multiplier = 512;
		bcINT32 l_scale = 1;
		bcINT32 l_distance_detail = 100;
		bcINT32 l_height_detail = 2;
		const core::bc_vector<core::bc_any>* l_material_names_param = nullptr;

		json_parse::bc_load(p_context.m_parameters, "xz_multiplier", l_xz_multiplier);
		json_parse::bc_load(p_context.m_parameters, "y_multiplier", l_y_multiplier);
		json_parse::bc_load(p_context.m_parameters, "scale", l_scale);
		json_parse::bc_load(p_context.m_parameters, "distance_detail", l_distance_detail);
		json_parse::bc_load(p_context.m_parameters, "height_detail", l_height_detail);
		json_parse::bc_load(p_context.m_parameters, "materials", l_material_names_param);

		_bc_material_properties l_material_properties;
		core::bc_vector<game::bc_height_map_material> l_materials = _read_materials(l_material_manager, l_material_properties, l_material_names_param);

		core::bc_vector_frame<game::bc_vertex_pos_tex> l_vertices;
		core::bc_vector_frame<bcUINT16> l_indices;
		core::bc_vector_frame<std::array<bcINT32, 3>> l_chunk_infos;
		_create_vertices_indices(l_width, l_height, l_xz_multiplier, l_vertices, l_indices, l_chunk_infos);

		_bc_parameter_buffer l_parameter;
		l_parameter.m_width = l_width;
		l_parameter.m_height = l_height;
		l_parameter.m_chunk_size = s_chunk_size;
		l_parameter.m_xz_multiplier = l_xz_multiplier;
		l_parameter.m_y_multiplier = l_y_multiplier;
		l_parameter.m_scale = l_scale;
		l_parameter.m_physics_y_scale = l_physics_system.get_height_field_y_scale();
		l_parameter.m_distance_detail = l_distance_detail;
		l_parameter.m_height_detail = l_height_detail;
		l_parameter.m_texture_map_width = l_texture_map_texture->get_resource().get_width();
		l_parameter.m_texture_map_height = l_texture_map_texture->get_resource().get_height();

		auto l_height_map = _create_height_map
		(
			l_render_system,
			std::move(l_height_map_texture),
			std::move(l_texture_map_texture),
			l_height_map_view_config,
			l_texture_map_view_config,
			l_vertices,
			l_indices,
			l_chunk_infos,
			l_materials,
			l_parameter,
			l_material_properties,
			std::move(l_px_height_map),
			std::move(l_px_deserialized_buffer),
			l_physics_system.get_height_field_y_scale(),
			l_distance_detail,
			l_height_detail
		);

		p_context.set_result(std::move(l_height_map));
	}

	void bc_height_map_loader_dx11::content_processing(core::bc_content_saving_context& p_context) const
	{
		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();
		auto& l_content_manager = p_context.m_content_manager;
		auto& l_render_system = l_game_system.get_render_system();
		auto& l_physics = l_game_system.get_physics_system().get_physics();
		auto& l_height_map_dx11 = static_cast<bc_height_map_dx11&>(p_context.m_content);

		physics::bc_height_field l_px_height_field = l_height_map_dx11.get_px_height_field();
		graphic::bc_texture2d l_height_map = l_height_map_dx11.get_height_map();
		graphic::bc_texture2d_content& l_texture_map = l_height_map_dx11.get_texture_map_content();

		core::bc_task<void> l_texture_map_task = l_content_manager.save_async(l_texture_map);

		physics::bc_serialize_buffer l_px_serialize_buffer = l_physics.create_serialize_buffer();
		l_px_serialize_buffer.add(l_px_height_field);
		physics::bc_memory_buffer l_px_serialized_buffer = l_physics.serialize(l_px_serialize_buffer);
		p_context.m_file.write(static_cast<bcBYTE*>(l_px_serialized_buffer.get_buffer_pointer()), l_px_serialized_buffer.get_buffer_size());

		const auto l_height_map_file_path = _get_height_map_texture_name(p_context.m_file_path);
		l_render_system.get_device().save_texture2d(l_height_map, graphic::bc_image_format::dds, l_height_map_file_path.c_str());

		l_texture_map_task.wait();
	}

	std::pair<graphic::bc_texture_config, graphic::bc_resource_view_config> bc_height_map_loader_dx11::get_height_map_texture_config(bcUINT32 p_width, 
		bcUINT32 p_height) const
	{
		auto l_texture_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_texture2d
			(
				p_width,
				p_height,
				false,
				1,
				graphic::bc_format::R32_FLOAT,
				graphic::bc_resource_usage::gpu_r,
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::shader })
			)
			.as_normal_texture();
		auto l_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(l_texture_config.get_format())
			.as_tex2d_shader_view(0, 1)
			.on_texture2d();

		return std::make_pair(l_texture_config, l_view_config);
	}

	std::pair<graphic::bc_texture_config, graphic::bc_resource_view_config> bc_height_map_loader_dx11::get_texture_map_texture_config(bcUINT32 p_width, 
		bcUINT32 p_height) const
	{
		auto l_texture_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_texture2d
			(
				p_width,
				p_height,
				false,
				1,
				graphic::bc_format::R16G16B16A16_UINT,
				graphic::bc_resource_usage::gpu_r,
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::shader })
			)
			.as_normal_texture();
		auto l_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(l_texture_config.get_format())
			.as_tex2d_shader_view(0, 1)
			.on_texture2d();

		return std::make_pair(l_texture_config, l_view_config);
	}

	core::bc_estring_frame bc_height_map_loader_dx11::_get_height_map_texture_name(core::bc_estring_view p_file_path) const
	{
		auto l_height_map_file_path = core::bc_path(p_file_path);
		const auto l_height_map_file_name = l_height_map_file_path.get_filename_without_extension() + g_height_map_extension;
		l_height_map_file_path.set_filename(l_height_map_file_name.c_str()).set_file_extension(bcL("dds"));

		return l_height_map_file_path.get_string_frame();
	}

	core::bc_estring_frame bc_height_map_loader_dx11::_get_texture_map_texture_name(core::bc_estring_view p_file_path) const
	{
		auto l_texture_map_file_path = core::bc_path(p_file_path);
		const auto l_texture_map_file_name = l_texture_map_file_path.get_filename_without_extension() + g_texture_map_extension;
		l_texture_map_file_path.set_filename(l_texture_map_file_name.c_str()).set_file_extension(bcL("dds"));

		return l_texture_map_file_path.get_string_frame();
	}

	void bc_height_map_loader_dx11::_create_texture_map(core::bc_content_loading_context& p_context, 
		game::bc_render_system& p_render_system, 
		bcUINT32 p_width, 
		bcUINT32 p_height) const
	{
		auto l_texture_map_file_path = core::bc_path(p_context.m_file_path);
		const auto l_texture_map_file_name = l_texture_map_file_path.get_filename_without_extension() + g_texture_map_extension;
		l_texture_map_file_path.set_filename(l_texture_map_file_name.c_str()).set_file_extension(bcL("dds"));

		platform::bc_basic_file_info l_texture_map_file_info{};
		platform::bc_file_info::get_basic_info(l_texture_map_file_path.get_string_frame().c_str(), &l_texture_map_file_info);

		if (!l_texture_map_file_info.m_exist)
		{
			const auto l_texture_map_config = get_texture_map_texture_config(p_width, p_height).first;
			auto l_texture_map = p_render_system.get_device().create_texture2d(l_texture_map_config, nullptr);

			p_render_system.get_device().save_texture2d(l_texture_map.get(), graphic::bc_image_format::dds, l_texture_map_file_path.get_string_frame().c_str());
		}
	}

	physics::bc_height_field_ref bc_height_map_loader_dx11::_create_px_height_field(game::bc_physics_system& p_physics_system,
		const bcINT16* p_samples, 
		bcUINT32 p_sample_count,
		bcUINT32 p_width, 
		bcUINT32 p_height) const
	{
		const physics::bc_bounded_strided_typed_data<bcINT16> l_samples_data(p_samples, sizeof(bcINT16), p_sample_count);
		const physics::bc_bounded_strided_typed_data<physics::bc_material_index> l_samples_material;

		physics::bc_height_field_desc l_px_desc(p_width, p_height, l_samples_data, l_samples_material);
		physics::bc_memory_buffer l_height_field_buffer = p_physics_system.get_physics().create_height_field(l_px_desc);
		physics::bc_height_field_ref l_height_field = p_physics_system.get_physics().create_height_field(l_height_field_buffer);

		return l_height_field;
	}

	void bc_height_map_loader_dx11::_copy_px_height_field(const physics::bc_height_field& p_px_height_field,
		bcFLOAT p_px_y_scale,
		bcFLOAT* p_dest) const
	{
		const auto l_width = p_px_height_field.get_num_column();
		const auto l_height = p_px_height_field.get_num_row();

		for (bcUINT32 l_z = 0; l_z < l_width; ++l_z)
		{
			for (bcUINT32 l_x = 0; l_x < l_height; ++l_x)
			{
				p_dest[l_z * l_width + l_x] = p_px_height_field.get_sample(l_x, l_z).m_height * p_px_y_scale;
			}
		}
	}

	core::bc_vector<game::bc_height_map_material> bc_height_map_loader_dx11::_read_materials(game::bc_material_manager& p_material_manager,
		_bc_material_properties& p_material_properties,
		const core::bc_vector<core::bc_any>* p_material_names_param) const
	{
		core::bc_vector<game::bc_height_map_material> l_materials;

		if (p_material_names_param == nullptr)
		{
			return l_materials;
		}

		core::bc_vector<std::tuple<core::bc_string, core::bc_string, bcFLOAT>> l_material_names;
		l_material_names.reserve(p_material_names_param->size());

		std::transform
		(
			std::cbegin(*p_material_names_param),
			std::cend(*p_material_names_param),
			std::back_inserter(l_material_names),
			[](const core::bc_any& p_material)
			{
				const auto& l_key_value = *p_material.as<core::bc_json_key_value>();

				const auto l_mesh_material_ite = l_key_value.find("mesh_material");
				const auto l_collider_material_ite = l_key_value.find("collider_material");
				const auto l_scale_ite = l_key_value.find("scale");

				core::bc_string l_mesh_material;
				core::bc_string l_collider_material;
				bcFLOAT l_scale = 1;

				if (l_mesh_material_ite != std::cend(l_key_value))
				{
					l_mesh_material = *l_mesh_material_ite->second.as<core::bc_string>();
				}
				if (l_collider_material_ite != std::cend(l_key_value))
				{
					l_collider_material = *l_collider_material_ite->second.as<core::bc_string>();
				}
				if (l_scale_ite != std::cend(l_key_value))
				{
					l_scale = *l_scale_ite->second.as<bcFLOAT>();
				}

				return std::make_tuple(std::move(l_mesh_material), std::move(l_collider_material), l_scale);
			}
		);
		
		l_materials.reserve(l_material_names.size());
		auto l_material_ite = 0U;

		for (auto& l_material_name : l_material_names)
		{
			game::bc_mesh_material_ptr l_mesh_material = p_material_manager.load_mesh_material_throw(std::get<0>(l_material_name).c_str());
			game::bc_collider_material_description l_collider_material = p_material_manager.find_collider_material(std::get<1>(l_material_name).c_str());

			p_material_properties.m_specular_intensity[l_material_ite] = l_mesh_material->get_specular_intensity();
			p_material_properties.m_specular_power[l_material_ite] = l_mesh_material->get_specular_power();
			p_material_properties.m_scale[l_material_ite] = std::get<bcFLOAT>(l_material_name);
			l_material_ite++;

			l_materials.push_back({ std::move(l_mesh_material), std::move(l_collider_material) });
		}

		return l_materials;
	}

	void bc_height_map_loader_dx11::_create_vertices_indices(bcUINT32 p_width,
		bcUINT32 p_height, 
		bcINT32 p_xz_multiplier, 
		core::bc_vector_frame<game::bc_vertex_pos_tex>& p_vertices, 
		core::bc_vector_frame<bcUINT16>& p_indices, 
		core::bc_vector_frame<std::array<bcINT32, 3>>& p_chunk_infos) const
	{
		const auto l_width_chunk_count = p_width / s_chunk_size;
		const auto l_height_chunk_count = p_height / s_chunk_size;
		const auto l_width_chunk_count_half = static_cast<bcINT32>(l_width_chunk_count / 2);
		const auto l_height_chunk_count_half = static_cast<bcINT32>(l_height_chunk_count / 2);

		p_vertices.resize((l_width_chunk_count + 1) * (l_height_chunk_count + 1));
		p_indices.resize(l_width_chunk_count * l_height_chunk_count * 4);
		p_chunk_infos.resize(l_width_chunk_count * l_height_chunk_count);

		bcUINT16 l_counter = 0;

		for (auto l_z = -l_height_chunk_count_half; l_z <= l_height_chunk_count_half; ++l_z)
		{
			for (auto l_x = -l_width_chunk_count_half; l_x <= l_width_chunk_count_half; ++l_x)
			{
				game::bc_vertex_pos_tex& l_vertex = p_vertices[l_counter++];

				l_vertex.m_position = core::bc_vector3f
				(
					l_x * static_cast<bcINT32>(s_chunk_size * p_xz_multiplier),
					0,
					l_z * static_cast<bcINT32>(s_chunk_size * p_xz_multiplier)
				);
				l_vertex.m_texcoord = core::bc_vector2f
				(
					(static_cast<bcFLOAT>(l_x) + l_width_chunk_count_half) / l_width_chunk_count,
					(static_cast<bcFLOAT>(l_z) + l_height_chunk_count_half) / l_height_chunk_count
				);

				if constexpr (graphic::bc_render_api_info::use_top_left_texcoord())
				{
					l_vertex.m_texcoord.y = 1 - l_vertex.m_texcoord.y;
				}
			}
		}

		l_counter = 0;

		for (auto l_z = 0U; l_z < l_height_chunk_count; ++l_z)
		{
			for (auto l_x = 0U; l_x < l_width_chunk_count; l_x++)
			{
				p_indices[l_counter++] = l_x + l_z * (l_width_chunk_count + 1);
				p_indices[l_counter++] = l_x + (l_z + 1) * (l_width_chunk_count + 1);
				p_indices[l_counter++] = (l_x + 1) + (l_z + 1) * (l_width_chunk_count + 1);
				p_indices[l_counter++] = (l_x + 1) + l_z * (l_width_chunk_count + 1);
				/*
				1-------2
				|		|
				|		|
				0-------3
				*/
			}
		}

		for (bcUINT32 l_ite = 0, l_end = l_width_chunk_count * l_height_chunk_count; l_ite < l_end; ++l_ite)
		{
			p_chunk_infos[l_ite] = std::array<bcINT32, 3>{std::numeric_limits<bcINT32>::max(), 0, 0};
		}
	}

	bc_height_map_dx11 bc_height_map_loader_dx11::_create_height_map(game::bc_render_system& p_render_system,
		graphic::bc_texture2d_ref p_height_map_texture,
		graphic::bc_texture2d_content_ptr p_texture_map_texture,
		graphic::bc_resource_view_config& p_height_map_view_config,
		graphic::bc_resource_view_config& p_texture_map_view_config,
		core::bc_vector_frame<game::bc_vertex_pos_tex>& p_vertices,
		core::bc_vector_frame<bcUINT16>& p_indices,
		core::bc_vector_frame<std::array<bcINT32, 3>>& p_chunk_infos,
		core::bc_vector<game::bc_height_map_material>& p_materials,
		_bc_parameter_buffer& p_parameters,
		_bc_material_properties& p_material_properties,
		physics::bc_height_field_ref p_px_height_map,
		physics::bc_serialize_buffer p_px_deserialized_buffer,
		bcFLOAT p_px_y_scale,
		bcINT32 p_distance_detail,
		bcINT32 p_height_detail) const
	{
		auto& l_device = p_render_system.get_device();
		auto l_resource_configure = graphic::bc_graphic_resource_builder();
		const auto l_width_chunk_count = p_parameters.m_width / s_chunk_size;
		const auto l_height_chunk_count = p_parameters.m_height / s_chunk_size;

		auto l_vbuffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				p_vertices.size(),
				sizeof(game::bc_vertex_pos_tex),
				graphic::bc_resource_usage::gpu_r
			)
			.as_vertex_buffer();
		auto l_ibuffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				p_indices.size(),
				sizeof(bcUINT16),
				graphic::bc_resource_usage::gpu_r
			)
			.as_index_buffer();
		auto l_chunk_info_buffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				l_width_chunk_count * l_height_chunk_count,
				sizeof(core::bc_vector3f),
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::unordered })
			)
			.with_structured_buffer(sizeof(core::bc_vector3f))
			.as_buffer();
		auto l_parameter_cbuffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				1,
				sizeof(_bc_parameter_buffer),
				graphic::bc_resource_usage::gpu_r
			)
			.as_constant_buffer();
		auto l_material_properties_cbuffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				1,
				sizeof(_bc_material_properties),
				graphic::bc_resource_usage::gpu_r
			)
			.as_constant_buffer();

		graphic::bc_subresource_data l_vbuffer_data(p_vertices.data(), 0, 0);
		graphic::bc_subresource_data l_ibuffer_data(p_indices.data(), 0, 0);
		graphic::bc_subresource_data l_chunk_info_buffer_data(p_chunk_infos.data(), 0, 0);
		graphic::bc_subresource_data l_parameter_cbuffer_data(&p_parameters, 0, 0);
		graphic::bc_subresource_data l_material_properties_data(&p_material_properties, 0, 0);

		auto l_vertex_buffer = l_device.create_buffer(l_vbuffer_config, &l_vbuffer_data);
		auto l_index_buffer = l_device.create_buffer(l_ibuffer_config, &l_ibuffer_data);
		auto l_chunk_info_buffer = l_device.create_buffer(l_chunk_info_buffer_config, &l_chunk_info_buffer_data);
		auto l_parameter_cbuffer = l_device.create_buffer(l_parameter_cbuffer_config, &l_parameter_cbuffer_data);
		auto l_material_properties_cbuffer = l_device.create_buffer(l_material_properties_cbuffer_config, &l_material_properties_data);

		auto l_chunk_info_view_config = l_resource_configure
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, l_width_chunk_count * l_height_chunk_count)
			.as_structured_buffer();
		auto l_chunk_info_unordered_view_config = l_resource_configure
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_unordered_view(0, l_width_chunk_count * l_height_chunk_count)
			.as_structured_buffer();

		auto l_height_map_view = l_device.create_resource_view(p_height_map_texture.get(), p_height_map_view_config);
		auto l_texture_map_view = l_device.create_resource_view(p_texture_map_texture->get_resource(), p_texture_map_view_config);
		auto l_chunk_info_view = l_device.create_resource_view(l_chunk_info_buffer.get(), l_chunk_info_view_config);
		auto l_chunk_info_unordered_view = l_device.create_resource_view(l_chunk_info_buffer.get(), l_chunk_info_unordered_view_config);

		game::bc_render_state_resource_view_array l_render_state_resource_view_array =
		{
			graphic::bc_resource_view_parameter(0, core::bc_enum::mask_or
			({
				graphic::bc_shader_type::vertex,
				graphic::bc_shader_type::hull,
				graphic::bc_shader_type::domain,
				graphic::bc_shader_type::pixel
			}), l_height_map_view.get()),
			graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::hull, l_chunk_info_view.get()),
			graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::pixel, l_texture_map_view.get())
		};

		bcUINT32 l_resource_view_count = 3;
		for (auto& l_material : p_materials)
		{
			auto l_diffuse_map_parameter = graphic::bc_resource_view_parameter(l_resource_view_count, graphic::bc_shader_type::pixel, l_material.m_mesh_material->get_diffuse_map_view());
			auto l_normal_map_parameter = graphic::bc_resource_view_parameter(l_resource_view_count + 1, graphic::bc_shader_type::pixel, l_material.m_mesh_material->get_normal_map_view());

			l_render_state_resource_view_array[l_resource_view_count++] = l_diffuse_map_parameter;
			l_render_state_resource_view_array[l_resource_view_count++] = l_normal_map_parameter;
		}

		auto l_render_state = p_render_system.create_render_state
		(
			graphic::bc_primitive::control_point_4_patchlist,
			l_vertex_buffer.get(),
			sizeof(game::bc_vertex_pos_tex),
			0,
			l_index_buffer.get(),
			game::i16bit,
			p_indices.size(),
			0,
			std::move(l_render_state_resource_view_array),
			{
				p_render_system.get_per_object_cbuffer(),
				graphic::bc_constant_buffer_parameter(0, core::bc_enum::mask_or
				({
					graphic::bc_shader_type::vertex,
					graphic::bc_shader_type::hull,
					graphic::bc_shader_type::domain,
					graphic::bc_shader_type::pixel
				}), l_parameter_cbuffer.get()),
				graphic::bc_constant_buffer_parameter
				(
					1, 
					core::bc_enum::mask_or({ graphic::bc_shader_type::pixel }), 
					l_material_properties_cbuffer.get()
				)
			}
		);

		auto l_height_map = bc_height_map_dx11
		(
			p_parameters.m_xz_multiplier,
			p_parameters.m_y_multiplier,
			p_px_y_scale,
			p_distance_detail,
			p_height_detail,
			std::move(l_render_state),
			std::move(l_vertex_buffer),
			std::move(l_index_buffer),
			std::move(p_height_map_texture),
			std::move(l_height_map_view),
			std::move(p_texture_map_texture),
			std::move(l_texture_map_view),
			std::move(l_parameter_cbuffer),
			std::move(l_chunk_info_buffer),
			std::move(l_chunk_info_view),
			std::move(l_chunk_info_unordered_view),
			std::move(l_material_properties_cbuffer),
			std::move(p_materials),
			std::move(p_px_height_map),
			p_px_deserialized_buffer.get_deserialize_buffer()
		);

		return l_height_map;
	}
}
