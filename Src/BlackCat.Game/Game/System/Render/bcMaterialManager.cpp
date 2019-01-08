// [03/07/2017 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/File/bcPath.h"
#include "Core/Container/bcString.h"
#include "Core/Math/bcVector4f.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/File/bcFileStream.h"
#include "Core/File/bcContentStreamManager.h"
#include "Core/Utility/bcJsonParse.h"
#include "GraphicImp/Resource/bcResourceConfig.h"
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
			BC_JSON_VALUE_OP(core::bc_vector4i, diffuse_color);
			BC_JSON_VALUE_OP(bcUINT32, specular_intensity);
			BC_JSON_VALUE_OP(bcUINT32, specular_power);
			BC_JSON_VALUE_OP(core::bc_string, diffuse_map);
			BC_JSON_VALUE_OP(core::bc_string, normal_map);
		};

		BC_JSON_STRUCTURE(_bc_material_json)
		{
		public:
			BC_JSON_ARRAY(_bc_material_desc, materials);
		};

		class _bc_material_manager_material : public bc_render_material
		{
		public:
			std::hash< const bcCHAR* >::result_type m_hash;
		};

		bc_material_manager::bc_material_manager(core::bc_content_stream_manager& p_content_stream, bc_render_system& p_render_system)
			: m_content_stream_manager(p_content_stream),
			m_render_system(p_render_system),
			m_default_normal_map()
		{
			m_default_texture_config = graphic::bc_graphic_resource_configure().as_resource()
				.as_texture2d
				(
					1,
					1,
					false,
					1,
					graphic::bc_format::R8G8B8A8_UNORM,
					graphic::bc_resource_usage::gpu_r,
					graphic::bc_resource_view_type::shader
				)
				.as_normal_texture();

			core::bc_vector4f l_normal_map_data(0.5, 0.5, 1, 1);
			graphic::bc_subresource_data l_normal_map_init_data(&l_normal_map_data, sizeof(core::bc_vector4f), 0);
			auto l_default_normal_map = graphic::bc_texture2d_content(m_render_system.get_device().create_texture2d(m_default_texture_config, &l_normal_map_init_data));

			m_default_diffuse_map = m_content_stream_manager.get_content_manager().load< graphic::bc_texture2d_content >
			(
				core::bc_alloc_type::program,
				bcL("Content\\Texture\\Default.jpg"),
				core::bc_content_loader_parameter()
			);
			m_default_normal_map = m_content_stream_manager.get_content_manager().store_content("material_default_normal_map", std::move(l_default_normal_map));
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
			m_default_diffuse_maps = std::move(p_other.m_default_diffuse_maps);

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
			}

			core::bc_json_document< _bc_material_json > l_material_json;
			l_material_json.parse(l_buffer.c_str());

			for(core::bc_json_object<_bc_material_desc>& l_material_desc : *l_material_json->m_materials)
			{
				_bc_material_description l_material_description;
				l_material_description.m_diffuse_color = *l_material_desc->m_diffuse_color;
				l_material_description.m_specular_intensity = *l_material_desc->m_specular_intensity;
				l_material_description.m_specular_power = *l_material_desc->m_specular_power;
				l_material_description.m_diffuse_map_name = *l_material_desc->m_diffuse_map;
				l_material_description.m_normal_map_name = *l_material_desc->m_normal_map;

				m_material_descriptions.insert(material_desc_map::value_type
				(
					string_hash()(l_material_desc->m_name->c_str()), std::move(l_material_description)
				));
			}
		}

		bc_render_material_ptr bc_material_manager::load_material(const bcCHAR* p_material) noexcept
		{
			return load_material(core::bc_alloc_type::unknown, p_material);
		}

		bc_render_material_ptr bc_material_manager::load_material(core::bc_alloc_type p_alloc_type, const bcCHAR* p_material) noexcept
		{
			auto l_hash = string_hash()(p_material);
			material_map::iterator l_entry;
			
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_materials_mutex);
			
				l_entry = m_materials.find(l_hash);
				if (l_entry != std::cend(m_materials))
				{
					return bc_render_material_ptr(l_entry->second.get(), _bc_material_ptr_deleter(this));
				}
			}

			auto l_description_entry = m_material_descriptions.find(l_hash);

			if (l_description_entry == std::cend(m_material_descriptions))
			{
				return nullptr;
			}

			graphic::bc_texture2d_content_ptr l_diffuse_map;
			graphic::bc_texture2d_content_ptr l_normal_map;

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
				l_diffuse_map = _add_get_default_diffuse_map(l_description_entry->second.m_diffuse_color);
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

			core::bc_unique_ptr< _bc_material_manager_material > l_material = core::bc_make_unique< _bc_material_manager_material >(p_alloc_type);
			l_material->m_hash = l_hash;
			l_material->m_diffuse = core::bc_vector4f
			(
				l_description_entry->second.m_diffuse_color.x / 255.0f,
				l_description_entry->second.m_diffuse_color.y / 255.0f,
				l_description_entry->second.m_diffuse_color.z / 255.0f,
				l_description_entry->second.m_diffuse_color.w / 255.0f
			);
			l_material->m_specular_intensity = l_description_entry->second.m_specular_intensity;
			l_material->m_specular_power = l_description_entry->second.m_specular_power;
			l_material->m_diffuse_map = l_diffuse_map;
			l_material->m_normal_map = l_normal_map;

			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_materials_mutex);

				auto* l_result = m_materials.insert(material_map::value_type
				(
					l_hash,
					core::bc_unique_ptr<bc_render_material>(l_material.release())
				)).first->second.get();

				return bc_render_material_ptr(l_result, _bc_material_ptr_deleter(this));
			}
		}

		bc_render_material_ptr bc_material_manager::load_material_throw(const bcCHAR* p_material)
		{
			return load_material_throw(core::bc_alloc_type::unknown, p_material);
		}

		bc_render_material_ptr bc_material_manager::load_material_throw(core::bc_alloc_type p_alloc_type, const bcCHAR* p_material)
		{
			auto l_material_ptr = load_material(p_alloc_type, p_material);

			if(!l_material_ptr)
			{
				core::bc_string_frame l_msg = "No entry found for material with name '";
				l_msg += p_material;
				l_msg += "'";

				throw bc_key_not_found_exception(l_msg.c_str());
			}

			return l_material_ptr;
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

		graphic::bc_texture2d_content_ptr bc_material_manager::_add_get_default_diffuse_map(core::bc_vector4i p_diffuse)
		{
			constexpr bcUBYTE l_color_step_tolerance = 10;

			bcINT32 l_step_min;
			bcINT32 l_step_max;

			l_step_min = (p_diffuse.x / l_color_step_tolerance) * l_color_step_tolerance;
			l_step_max = ((p_diffuse.x / l_color_step_tolerance) + 1) * l_color_step_tolerance;
			p_diffuse.x = p_diffuse.x - l_step_min <= l_color_step_tolerance / 2 ? l_step_min : l_step_max;

			l_step_min = (p_diffuse.y / l_color_step_tolerance) * l_color_step_tolerance;
			l_step_max = ((p_diffuse.y / l_color_step_tolerance) + 1) * l_color_step_tolerance;
			p_diffuse.y = p_diffuse.y - l_step_min <= l_color_step_tolerance / 2 ? l_step_min : l_step_max;

			l_step_min = (p_diffuse.z / l_color_step_tolerance) * l_color_step_tolerance;
			l_step_max = ((p_diffuse.z / l_color_step_tolerance) + 1) * l_color_step_tolerance;
			p_diffuse.z = p_diffuse.z - l_step_min <= l_color_step_tolerance / 2 ? l_step_min : l_step_max;

			l_step_min = (p_diffuse.w / l_color_step_tolerance) * l_color_step_tolerance;
			l_step_max = ((p_diffuse.w / l_color_step_tolerance) + 1) * l_color_step_tolerance;
			p_diffuse.w = p_diffuse.w - l_step_min <= l_color_step_tolerance / 2 ? l_step_min : l_step_max;

			auto l_hash = p_diffuse.to_rgba();
			auto l_diffuse_map_entry = m_default_diffuse_maps.find(l_hash);

			if(l_diffuse_map_entry != std::cend(m_default_diffuse_maps))
			{
				return l_diffuse_map_entry->second;
			}

			graphic::bc_subresource_data l_normal_map_init_data(&p_diffuse, sizeof(core::bc_vector4f), 0);
			auto l_diffuse_texture = graphic::bc_texture2d_content(m_render_system.get_device().create_texture2d
			(
				m_default_texture_config,
				&l_normal_map_init_data
			));

			core::bc_string_frame l_diffuse_content_name = "material_default_diffuse_map_";
			l_diffuse_content_name += core::bc_to_string_frame(l_hash);

			auto l_diffuse_texture_ptr = m_content_stream_manager.get_content_manager().store_content
			(
				core::bc_alloc_type::program,
				l_diffuse_content_name.c_str(),
				std::move(l_diffuse_texture)
			);
			l_diffuse_map_entry = m_default_diffuse_maps.insert(default_diffuse_map::value_type(l_hash, std::move(l_diffuse_texture_ptr))).first;

			return l_diffuse_map_entry->second;
		}
	}
}