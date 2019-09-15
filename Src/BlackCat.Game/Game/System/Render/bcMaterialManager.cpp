// [03/07/2017 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/File/bcPath.h"
#include "Core/Container/bcString.h"
#include "Core/Math/bcVector4f.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/File/bcFileStream.h"
#include "Core/File/bcPath.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcJsonParse.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcMaterialManager.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		BC_JSON_STRUCTURE(_bc_material_desc)
		{
		public:
			BC_JSON_VALUE(core::bc_string, name);
			BC_JSON_VALUE_OP(core::bc_vector4f, diffuse_color);
			BC_JSON_VALUE_OP(bcFLOAT, specular_intensity);
			BC_JSON_VALUE_OP(bcFLOAT, specular_power);
			BC_JSON_VALUE_OP(core::bc_string, diffuse_map);
			BC_JSON_VALUE_OP(core::bc_string, normal_map);
			BC_JSON_VALUE_OP(core::bc_string, specular_map);
		};

		BC_JSON_STRUCTURE(_bc_material_json)
		{
		public:
			BC_JSON_ARRAY(_bc_material_desc, materials);
		};

		class _bc_material_manager_material : public bc_render_material
		{
		public:
			using hash_type = std::hash< const bcCHAR* >::result_type;

		public:
			explicit _bc_material_manager_material(hash_type p_hash, bc_render_material p_material)
				: bc_render_material(std::move(p_material)),
				m_hash(p_hash)
			{
			}

			hash_type m_hash;
		};

		bc_material_manager::bc_material_manager(core::bc_content_stream_manager& p_content_stream, bc_render_system& p_render_system)
			: m_content_stream_manager(p_content_stream),
			m_render_system(p_render_system),
			m_default_texture_config(),
			m_default_normal_map()
		{
			auto& l_content_manager = m_content_stream_manager.get_content_manager();

			m_default_texture_config = graphic::bc_graphic_resource_builder().as_resource()
				.as_texture2d
				(
					1,
					1,
					false,
					1,
					graphic::bc_format::R32G32B32A32_FLOAT,
					graphic::bc_resource_usage::gpu_r,
					graphic::bc_resource_view_type::shader
				)
				.as_normal_texture();

			core::bc_vector4f l_normal_map_data(0.5, 0.5, 1, 1);
			core::bc_vector4f l_specular_map_data(1, 1, 1, 1);
			graphic::bc_subresource_data l_normal_map_init_data(&l_normal_map_data, sizeof(core::bc_vector4f), 0);
			graphic::bc_subresource_data l_specular_map_init_data(&l_specular_map_data, sizeof(core::bc_vector4f), 0);
			auto l_default_normal_map = graphic::bc_texture2d_content(m_render_system.get_device().create_texture2d(m_default_texture_config, &l_normal_map_init_data));
			auto l_default_specular_map = graphic::bc_texture2d_content(m_render_system.get_device().create_texture2d(m_default_texture_config, &l_specular_map_init_data));

			m_default_diffuse_map = l_content_manager.load< graphic::bc_texture2d_content >
			(
				core::bc_alloc_type::program,
				core::bc_path::get_absolute_path(bcL("Content\\Texture\\Default.jpg")).c_str(),
				core::bc_content_loader_parameter()
			);
			m_default_normal_map = l_content_manager.store_content("default_material_normal_map", std::move(l_default_normal_map));
			m_default_specular_map = l_content_manager.store_content("default_material_specular_map", std::move(l_default_specular_map));
		}

		bc_material_manager::bc_material_manager(bc_material_manager&& p_other) noexcept
			: m_content_stream_manager(p_other.m_content_stream_manager),
			m_render_system(p_other.m_render_system)
		{
			operator=(std::move(p_other));
		}

		bc_material_manager::~bc_material_manager()
		{
		}

		bc_material_manager& bc_material_manager::operator=(bc_material_manager&& p_other) noexcept
		{
			m_material_descriptions = std::move(p_other.m_material_descriptions);
			m_materials = std::move(p_other.m_materials);
			m_default_texture_config = p_other.m_default_texture_config;
			m_default_diffuse_map = std::move(p_other.m_default_diffuse_map);
			m_default_normal_map = std::move(p_other.m_default_normal_map);
			m_default_specular_map = std::move(p_other.m_default_specular_map);
			m_default_color_textures = std::move(p_other.m_default_color_textures);

			return *this;
		}

		void bc_material_manager::read_material_file(const bcECHAR* p_material_json_file)
		{
			core::bc_file_stream l_json_file;
			core::bc_string_frame l_buffer;

			if (!l_json_file.open_read(p_material_json_file))
			{
				core::bc_string_frame l_msg = "Error in reading material file: ";
				l_msg += core::bc_to_exclusive_string(p_material_json_file).c_str();

				throw bc_io_exception(l_msg.c_str());
			}

			core::bc_string_frame l_line;
			while (core::bc_get_line(l_json_file, l_line))
			{
				l_buffer.append(l_line);
				l_line.clear();
			}

			core::bc_json_document< _bc_material_json > l_material_json;
			l_material_json.load(l_buffer.c_str());

			for(core::bc_json_object<_bc_material_desc>& l_material_desc : l_material_json->m_materials)
			{
				_bc_render_material_file_description l_material_description;
				l_material_description.m_diffuse_color = *l_material_desc->m_diffuse_color;
				l_material_description.m_specular_intensity = *l_material_desc->m_specular_intensity;
				l_material_description.m_specular_power = *l_material_desc->m_specular_power;
				l_material_description.m_diffuse_map_name = *l_material_desc->m_diffuse_map;
				l_material_description.m_normal_map_name = *l_material_desc->m_normal_map;
				l_material_description.m_specular_map_name = *l_material_desc->m_specular_map;

				m_material_descriptions.insert(material_desc_map::value_type
				(
					string_hash()(l_material_desc->m_name->c_str()), std::move(l_material_description)
				));
			}
		}

		bc_render_material_ptr bc_material_manager::load_material(const bcCHAR* p_name) noexcept
		{
			return load_material(core::bc_alloc_type::unknown, p_name);
		}

		bc_render_material_ptr bc_material_manager::load_material(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name) noexcept
		{
			auto l_hash = string_hash()(p_name);
			material_map::iterator l_entry;
			
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_materials_mutex);
			
				l_entry = m_materials.find(l_hash);
				if (l_entry != std::cend(m_materials))
				{
					return bc_render_material_ptr(l_entry->second.get(), _bc_material_ptr_deleter(this));
				}
			}

			const auto l_description_entry = m_material_descriptions.find(l_hash);
			if (l_description_entry == std::cend(m_material_descriptions))
			{
				return nullptr;
			}

			graphic::bc_texture2d_content_ptr l_diffuse_map;
			graphic::bc_texture2d_content_ptr l_normal_map;
			graphic::bc_texture2d_content_ptr l_specular_map;

			if (!l_description_entry->second.m_diffuse_map_name.empty())
			{
				l_diffuse_map = m_content_stream_manager.find_content< graphic::bc_texture2d_content >
				(
					l_description_entry->second.m_diffuse_map_name.c_str()
				);

				if(!l_diffuse_map)
				{
					l_diffuse_map = m_default_diffuse_map;
				}
			}
			else
			{
				l_diffuse_map = _create_texture_from_color(l_description_entry->second.m_diffuse_color);
			}

			if (!l_description_entry->second.m_normal_map_name.empty())
			{
				l_normal_map = m_content_stream_manager.find_content< graphic::bc_texture2d_content >
				(
					l_description_entry->second.m_normal_map_name.c_str()
				);

				if (!l_normal_map)
				{
					l_normal_map = m_default_normal_map;
				}
			}
			else
			{
				l_normal_map = m_default_normal_map;
			}

			if (!l_description_entry->second.m_specular_map_name.empty())
			{
				l_specular_map = m_content_stream_manager.find_content< graphic::bc_texture2d_content >
				(
					l_description_entry->second.m_specular_map_name.c_str()
				);

				if (!l_specular_map)
				{
					l_specular_map = m_default_specular_map;
				}
			}
			else
			{
				l_specular_map = _create_texture_from_color(core::bc_vector4f
				(
					l_description_entry->second.m_specular_intensity,
					l_description_entry->second.m_specular_intensity,
					l_description_entry->second.m_specular_intensity,
					l_description_entry->second.m_specular_intensity
				));
			}

			bc_render_material l_material;
			l_material.m_parameters.m_diffuse = l_description_entry->second.m_diffuse_color;
			l_material.m_parameters.m_specular_intensity = l_description_entry->second.m_specular_intensity;
			l_material.m_parameters.m_specular_power = l_description_entry->second.m_specular_power;
			l_material.m_parameters.m_has_normal_map = l_normal_map != nullptr && l_normal_map != m_default_normal_map;
			l_material.m_diffuse_map = l_diffuse_map;
			l_material.m_normal_map = l_normal_map;
			l_material.m_specular_map = l_specular_map;

			return _store_material(p_alloc_type, p_name, std::move(l_material));
		}

		bc_render_material_ptr bc_material_manager::load_material_throw(const bcCHAR* p_name)
		{
			return load_material_throw(core::bc_alloc_type::unknown, p_name);
		}

		bc_render_material_ptr bc_material_manager::load_material_throw(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name)
		{
			auto l_material_ptr = load_material(p_alloc_type, p_name);

			if(!l_material_ptr)
			{
				core::bc_string_frame l_msg = "No entry found for material with name '";
				l_msg += p_name;
				l_msg += "'";

				throw bc_key_not_found_exception(l_msg.c_str());
			}

			return l_material_ptr;
		}

		bc_render_material_ptr bc_material_manager::store_material(const bcCHAR* p_name, bc_render_material_description p_material)
		{
			return store_material(core::bc_alloc_type::unknown, p_name, std::move(p_material));
		}

		bc_render_material_ptr bc_material_manager::store_material(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name, bc_render_material_description p_material)
		{
			bc_render_material l_material;
			l_material.m_parameters.m_diffuse = p_material.m_diffuse;
			l_material.m_parameters.m_specular_intensity = p_material.m_specular_intensity;
			l_material.m_parameters.m_specular_power = p_material.m_specular_power;
			l_material.m_parameters.m_has_normal_map = p_material.m_normal_map != nullptr;
			l_material.m_diffuse_map = p_material.m_diffuse_map;
			l_material.m_normal_map = p_material.m_normal_map;
			l_material.m_specular_map = p_material.m_specular_map;

			if(l_material.m_diffuse_map == nullptr)
			{
				l_material.m_diffuse_map = _create_texture_from_color(p_material.m_diffuse);
			}

			if(l_material.m_normal_map == nullptr)
			{
				l_material.m_normal_map = m_default_normal_map;
			}

			if(l_material.m_specular_map == nullptr)
			{
				const core::bc_vector4f l_specular
				(
					p_material.m_specular_intensity, 
					p_material.m_specular_intensity, 
					p_material.m_specular_intensity, 
					p_material.m_specular_intensity
				);
				l_material.m_specular_map = _create_texture_from_color(l_specular);
			}

			return _store_material(p_alloc_type, p_name, std::move(l_material));
		}

		void bc_material_manager::destroy_material(bc_render_material* p_material)
		{
			auto* l_material = static_cast< _bc_material_manager_material* >(p_material);

			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_materials_mutex);

				auto l_entry = m_materials.find(l_material->m_hash);

				bcAssert(l_entry != std::cend(m_materials));

				m_materials.erase(l_entry);
			}
		}

		graphic::bc_texture2d_content_ptr bc_material_manager::_create_texture_from_color(core::bc_vector4f p_color)
		{
			constexpr bcUBYTE l_color_step_tolerance = 10;

			core::bc_vector4i l_color(p_color.x * 255, p_color.y * 255, p_color.z * 255, p_color.w * 255);

			bcINT32 l_step_min;
			bcINT32 l_step_max;

			l_step_min = (l_color.x / l_color_step_tolerance) * l_color_step_tolerance;
			l_step_max = ((l_color.x / l_color_step_tolerance) + 1) * l_color_step_tolerance;
			l_color.x = l_color.x - l_step_min <= l_color_step_tolerance / 2 ? l_step_min : l_step_max;

			l_step_min = (l_color.y / l_color_step_tolerance) * l_color_step_tolerance;
			l_step_max = ((l_color.y / l_color_step_tolerance) + 1) * l_color_step_tolerance;
			l_color.y = l_color.y - l_step_min <= l_color_step_tolerance / 2 ? l_step_min : l_step_max;

			l_step_min = (l_color.z / l_color_step_tolerance) * l_color_step_tolerance;
			l_step_max = ((l_color.z / l_color_step_tolerance) + 1) * l_color_step_tolerance;
			l_color.z = l_color.z - l_step_min <= l_color_step_tolerance / 2 ? l_step_min : l_step_max;

			l_step_min = (l_color.w / l_color_step_tolerance) * l_color_step_tolerance;
			l_step_max = ((l_color.w / l_color_step_tolerance) + 1) * l_color_step_tolerance;
			l_color.w = l_color.w - l_step_min <= l_color_step_tolerance / 2 ? l_step_min : l_step_max;

			p_color = core::bc_vector4f(l_color.x / 255.0, l_color.y / 255.0, l_color.z / 255.0, l_color.w / 255.0);

			auto l_hash = p_color.to_rgba();
			auto l_color_map_entry = m_default_color_textures.find(l_hash);

			if(l_color_map_entry != std::cend(m_default_color_textures))
			{
				return l_color_map_entry->second;
			}

			graphic::bc_subresource_data l_color_map_init_data(&p_color, sizeof(core::bc_vector4f), 0);
			auto l_color_map_texture = graphic::bc_texture2d_content(m_render_system.get_device().create_texture2d
			(
				m_default_texture_config,
				&l_color_map_init_data
			));

			core::bc_string_frame l_content_name = "material_color_map_";
			l_content_name += core::bc_to_string_frame(l_hash);

			auto l_diffuse_texture_ptr = m_content_stream_manager.get_content_manager().store_content
			(
				core::bc_alloc_type::program,
				l_content_name.c_str(),
				std::move(l_color_map_texture)
			);
			l_color_map_entry = m_default_color_textures.insert(default_diffuse_map::value_type(l_hash, std::move(l_diffuse_texture_ptr))).first;

			return l_color_map_entry->second;
		}

		bc_render_material_ptr bc_material_manager::_store_material(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name, bc_render_material p_material)
		{
			auto& l_device = m_render_system.get_device();
			auto l_hash = string_hash()(p_name);
			auto l_material = core::bc_make_unique< _bc_material_manager_material >(p_alloc_type, l_hash, std::move(p_material));

			auto l_diffuse_map = l_material->m_diffuse_map.get()->get_resource();
			auto l_normal_map = l_material->m_normal_map.get()->get_resource();
			auto l_specular_map = l_material->m_specular_map.get()->get_resource();

			auto m_diffuse_map_view_config = graphic::bc_graphic_resource_builder()
				.as_resource_view()
				.as_texture_view(l_diffuse_map.get_format())
				.as_tex2d_shader_view(0, -1)
				.on_texture2d();
			auto m_normal_map_view_config = graphic::bc_graphic_resource_builder()
				.as_resource_view()
				.as_texture_view(l_normal_map.get_format())
				.as_tex2d_shader_view(0, -1)
				.on_texture2d();
			auto m_specular_map_view_config = graphic::bc_graphic_resource_builder()
				.as_resource_view()
				.as_texture_view(l_specular_map.get_format())
				.as_tex2d_shader_view(0, -1)
				.on_texture2d();

			l_material->m_diffuse_map_view = l_device.create_resource_view(l_diffuse_map, m_diffuse_map_view_config);
			l_material->m_normal_map_view = l_device.create_resource_view(l_normal_map, m_normal_map_view_config);
			l_material->m_specular_map_view = l_device.create_resource_view(l_specular_map, m_specular_map_view_config);

			auto l_parameters_cbuffer_config = graphic::bc_graphic_resource_builder()
				.as_resource()
				.as_buffer(1, sizeof(bc_render_material_parameter), graphic::bc_resource_usage::gpu_r, graphic::bc_resource_view_type::shader)
				.as_constant_buffer();
			auto l_parameters_cbuffer_data = graphic::bc_subresource_data(&l_material->m_parameters, sizeof(bc_render_material_parameter), 0);

			l_material->m_parameter_cbuffer = l_device.create_buffer(l_parameters_cbuffer_config, &l_parameters_cbuffer_data);

			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_materials_mutex);

				auto* l_result = m_materials.insert(material_map::value_type
				(
					l_hash,
					core::bc_unique_ptr<bc_render_material>(std::move(l_material))
				)).first->second.get();

				return bc_render_material_ptr(l_result, _bc_material_ptr_deleter(this));
			}
		}
	}
}