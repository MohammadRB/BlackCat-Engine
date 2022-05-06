// [03/07/2017 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/File/bcPath.h"
#include "Core/Math/bcVector4f.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcStringStream.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/File/bcFileStream.h"
#include "Core/File/bcPath.h"
#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		BC_JSON_STRUCTURE(_bc_mesh_material_desc)
		{
			BC_JSON_VALUE(core::bc_string_frame, name);
			BC_JSON_VALUE_OP(core::bc_vector4f, diffuse_color);
			BC_JSON_VALUE_OP(bcFLOAT, specular_intensity);
			BC_JSON_VALUE_OP(bcFLOAT, specular_power);
			BC_JSON_VALUE_OP(core::bc_string_frame, diffuse_map);
			BC_JSON_VALUE_OP(core::bc_string_frame, normal_map);
			BC_JSON_VALUE_OP(core::bc_string_frame, specular_map);
		};

		BC_JSON_STRUCTURE(_bc_collider_material_desc)
		{
			BC_JSON_VALUE(core::bc_string_frame, name);
			BC_JSON_VALUE_OP(bcFLOAT, static_friction);
			BC_JSON_VALUE_OP(bcFLOAT, dynamic_friction);
			BC_JSON_VALUE_OP(bcFLOAT, restitution);
			BC_JSON_VALUE_OP(core::bc_string_frame, particle);
			BC_JSON_VALUE_OP(core::bc_string_frame, decal);
		};

		BC_JSON_STRUCTURE(_bc_material_json)
		{
			BC_JSON_ARRAY(_bc_mesh_material_desc, mesh_materials);
			BC_JSON_ARRAY(_bc_collider_material_desc, collider_materials);
		};

		class _bc_material_manager_material : public bc_mesh_material
		{
		public:
			using hash_type = std::hash<const bcCHAR*>::result_type;

		public:
			explicit _bc_material_manager_material(hash_type p_hash, bc_mesh_material p_material)
				: bc_mesh_material(std::move(p_material)),
				m_hash(p_hash)
			{
			}

			hash_type m_hash;
		};

		bc_material_manager::bc_material_manager(core::bc_content_stream_manager& p_content_stream, bc_render_system& p_render_system, bc_physics_system& p_physics_system)
			: m_content_stream_manager(&p_content_stream),
			m_render_system(&p_render_system),
			m_physics_system(&p_physics_system),
			m_default_texture_config(),
			m_default_normal_map()
		{
			auto& l_content_manager = m_content_stream_manager->get_content_manager();

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

			const core::bc_vector4f l_normal_map_data(.5f, .5f, 1.0f, 1.0f);
			const core::bc_vector4f l_specular_map_data(.7f, .7f, .7f, 1.0f);
			const graphic::bc_subresource_data l_normal_map_init_data(&l_normal_map_data, sizeof(core::bc_vector4f), 0);
			const graphic::bc_subresource_data l_specular_map_init_data(&l_specular_map_data, sizeof(core::bc_vector4f), 0);
			auto l_default_normal_map = graphic::bc_texture2d_content(m_render_system->get_device().create_texture2d(m_default_texture_config, &l_normal_map_init_data));
			auto l_default_specular_map = graphic::bc_texture2d_content(m_render_system->get_device().create_texture2d(m_default_texture_config, &l_specular_map_init_data));

			m_default_diffuse_map = l_content_manager.load<graphic::bc_texture2d_content>
			(
				core::bc_alloc_type::program,
				core::bc_get_service<bc_game_system>()->get_file_system().get_content_texture_path(bcL("Default.dds")).c_str(),
				{},
				core::bc_content_loader_parameter()
			);
			m_default_normal_map = l_content_manager.store_content(bcL("default_normal_map"), std::move(l_default_normal_map));
			m_default_specular_map = l_content_manager.store_content(bcL("default_specular_map"), std::move(l_default_specular_map));

			m_collider_materials.insert(collider_material_map::value_type
			(
				string_hash()("default"),
				bc_collider_material("default", m_physics_system->get_physics().create_material(2, 2, 0.1f), "", "")
			));
		}

		bc_material_manager::bc_material_manager(bc_material_manager&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_material_manager::~bc_material_manager() = default;

		bc_material_manager& bc_material_manager::operator=(bc_material_manager&& p_other) noexcept
		{
			m_content_stream_manager = p_other.m_content_stream_manager;
			m_render_system = p_other.m_render_system;
			m_physics_system = p_other.m_physics_system;
			m_mesh_material_descriptions = std::move(p_other.m_mesh_material_descriptions);
			m_collider_materials = std::move(p_other.m_collider_materials);
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
			core::bc_string_frame l_json_file_buffer;

			if (!l_json_file.open_read(p_material_json_file))
			{
				const auto l_msg = core::bc_string_stream_frame() << "Error in reading material file: " << p_material_json_file;
				throw bc_io_exception(l_msg.str().c_str());
			}

			core::bc_read_all_lines(l_json_file, l_json_file_buffer);

			core::bc_json_document<_bc_material_json> l_material_json;
			l_material_json.load(l_json_file_buffer.c_str());

			for(core::bc_json_object<_bc_mesh_material_desc>& l_material_desc : l_material_json->m_mesh_materials)
			{
				_bc_mesh_material_desc_entry l_material;
				l_material.m_diffuse_color = *l_material_desc->m_diffuse_color;
				l_material.m_specular_intensity = *l_material_desc->m_specular_intensity;
				l_material.m_specular_power = *l_material_desc->m_specular_power;
				l_material.m_diffuse_map_name = *l_material_desc->m_diffuse_map;
				l_material.m_normal_map_name = *l_material_desc->m_normal_map;
				l_material.m_specular_map_name = *l_material_desc->m_specular_map;

				const auto l_material_hash = string_hash()(l_material_desc->m_name->c_str());
				auto l_ite = m_mesh_material_descriptions.find(l_material_hash);

				if(l_ite == std::end(m_mesh_material_descriptions))
				{
					m_mesh_material_descriptions.insert(mesh_material_desc_map::value_type(l_material_hash, std::move(l_material)));
				}
				else
				{
					l_ite->second = std::move(l_material);
					core::bc_log(core::bc_log_type::debug)
						<< "material description with name '"
						<< *l_material_desc->m_name
						<< "' already exist. old material will be replaced."
						<< core::bc_lend;
				}
			}

			for(core::bc_json_object<_bc_collider_material_desc>& l_material_desc : l_material_json->m_collider_materials)
			{
				auto l_px_material = m_physics_system->get_physics().create_material
				(
					*l_material_desc->m_static_friction,
					*l_material_desc->m_dynamic_friction,
					*l_material_desc->m_restitution
				);
				bc_collider_material l_material
				(
					l_material_desc->m_name->c_str(),
					std::move(l_px_material),
					l_material_desc->m_particle->c_str(),
					l_material_desc->m_decal->c_str()
				);

				const auto l_material_hash = string_hash()(l_material.get_name());
				auto l_ite = m_collider_materials.find(l_material_hash);

				if (l_ite == std::end(m_collider_materials))
				{
					m_collider_materials.insert(collider_material_map::value_type(l_material_hash, std::move(l_material)));
				}
				else
				{
					l_ite->second = std::move(l_material);
					core::bc_log(core::bc_log_type::debug)
						<< "collider material description with name '"
						<< *l_material_desc->m_name
						<< "' already exist. old material will be replaced."
						<< core::bc_lend;
				}
			}
		}

		bc_mesh_material_ptr bc_material_manager::load_mesh_material(const bcCHAR* p_name) noexcept
		{
			return load_mesh_material(core::bc_alloc_type::unknown, p_name);
		}

		bc_mesh_material_ptr bc_material_manager::load_mesh_material(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name) noexcept
		{
			const auto l_hash = string_hash()(p_name);

			{
				platform::bc_lock_guard l_guard(m_materials_mutex);

				const auto l_entry = m_materials.find(l_hash);
				if (l_entry != std::cend(m_materials))
				{
					return bc_mesh_material_ptr(l_entry->second.get(), _bc_mesh_material_ptr_deleter(this));
				}
			}

			const auto l_description_entry = m_mesh_material_descriptions.find(l_hash);
			if (l_description_entry == std::cend(m_mesh_material_descriptions))
			{
				return nullptr;
			}

			graphic::bc_texture2d_content_ptr l_diffuse_map;
			graphic::bc_texture2d_content_ptr l_normal_map;
			graphic::bc_texture2d_content_ptr l_specular_map;

			if (!l_description_entry->second.m_diffuse_map_name.empty())
			{
				l_diffuse_map = m_content_stream_manager->find_content<graphic::bc_texture2d_content>
				(
					l_description_entry->second.m_diffuse_map_name
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
				l_normal_map = m_content_stream_manager->find_content<graphic::bc_texture2d_content>
				(
					l_description_entry->second.m_normal_map_name
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
				l_specular_map = m_content_stream_manager->find_content<graphic::bc_texture2d_content>
				(
					l_description_entry->second.m_specular_map_name
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

			bc_mesh_material l_material;
			l_material.m_parameters.m_diffuse = l_description_entry->second.m_diffuse_color;
			l_material.m_parameters.m_specular_intensity = l_description_entry->second.m_specular_intensity;
			l_material.m_parameters.m_specular_power = l_description_entry->second.m_specular_power;
			l_material.m_parameters.m_has_normal_map = l_normal_map != nullptr && l_normal_map != m_default_normal_map;
			l_material.m_diffuse_map = l_diffuse_map;
			l_material.m_normal_map = l_normal_map;
			l_material.m_specular_map = l_specular_map;

			return _store_mesh_material(p_alloc_type, p_name, std::move(l_material));
		}

		bc_mesh_material_ptr bc_material_manager::load_mesh_material_throw(const bcCHAR* p_name)
		{
			return load_mesh_material_throw(core::bc_alloc_type::unknown, p_name);
		}

		bc_mesh_material_ptr bc_material_manager::load_mesh_material_throw(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name)
		{
			auto l_material_ptr = load_mesh_material(p_alloc_type, p_name);

			if(!l_material_ptr)
			{
				core::bc_string_frame l_msg = "No entry was found for material with name '";
				l_msg += p_name;
				l_msg += "'";

				throw bc_key_not_found_exception(l_msg.c_str());
			}

			return l_material_ptr;
		}

		bc_mesh_material_ptr bc_material_manager::store_mesh_material(const bcCHAR* p_name, bc_mesh_material_description p_material)
		{
			return store_mesh_material(core::bc_alloc_type::unknown, p_name, std::move(p_material));
		}

		bc_mesh_material_ptr bc_material_manager::store_mesh_material(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name, bc_mesh_material_description p_material)
		{
			bc_mesh_material l_material;
			std::memset(&l_material.m_parameters, 0, sizeof(bc_mesh_material_parameter));

			l_material.m_parameters.m_diffuse = p_material.m_diffuse;
			l_material.m_parameters.m_specular_intensity = p_material.m_specular_intensity;
			l_material.m_parameters.m_specular_power = p_material.m_specular_power;
			l_material.m_parameters.m_has_normal_map = p_material.m_normal_map != nullptr;
			l_material.m_diffuse_map = std::move(p_material.m_diffuse_map);
			l_material.m_normal_map = std::move(p_material.m_normal_map);
			l_material.m_specular_map = std::move(p_material.m_specular_map);

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
			
			return _store_mesh_material(p_alloc_type, p_name, std::move(l_material));
		}

		bc_collider_material_description bc_material_manager::get_default_collider_material() const
		{
			const auto l_default_material_ite = m_collider_materials.find(string_hash()("default"));

			bc_collider_material_description l_material;
			l_material.m_hash = l_default_material_ite->first;
			l_material.m_name = l_default_material_ite->second.get_name();
			l_material.m_px_material = l_default_material_ite->second.get_px_material();
			l_material.m_collision_particle = l_default_material_ite->second.get_particle_name();
			l_material.m_collision_decal = l_default_material_ite->second.get_decal_name();

			return l_material;
		}
		
		bc_collider_material_description bc_material_manager::find_collider_material(const bcCHAR* p_name) const
		{
			const auto l_hash = string_hash()(p_name);
			auto l_material = find_collider_material(l_hash);

			if(l_material.m_hash == string_hash()("default"))
			{
				const auto l_warning_msg = core::bc_estring_frame(bcL("No collider material was found for material with name ")) +
						core::bc_to_estring_frame(p_name) +
						bcL(". Using default material instead.");
				core::bc_log(core::bc_log_type::info, l_warning_msg.c_str());
			}

			return l_material;
		}

		bc_collider_material_description bc_material_manager::find_collider_material(bc_collider_material_description::hash_t p_hash) const
		{
			collider_material_map::const_iterator l_ite;
			bool l_was_found = true;

			{
				platform::bc_lock_guard l_guard(m_materials_mutex);

				l_ite = m_collider_materials.find(p_hash);
				if (l_ite == std::cend(m_collider_materials))
				{
					l_ite = m_collider_materials.find(string_hash()("default"));
					l_was_found = false;
				}
			}

			if (!l_was_found)
			{
				const auto l_msg = core::bc_string_stream_frame() << "No collider material was found with hash '" << p_hash << "'. Default material will be used instead.";
				core::bc_log(core::bc_log_type::warning) << l_msg.str() << core::bc_lend;
			}

			bc_collider_material_description l_material;
			l_material.m_hash = l_ite->first;
			l_material.m_name = l_ite->second.get_name();
			l_material.m_px_material = l_ite->second.get_px_material();
			l_material.m_collision_particle = l_ite->second.get_particle_name();
			l_material.m_collision_decal = l_ite->second.get_decal_name();

			return l_material;
		}

		bc_collider_material_description bc_material_manager::find_collider_material_throw(bc_collider_material_description::hash_t p_hash) const
		{
			{
				platform::bc_lock_guard l_guard(m_materials_mutex);

				const auto l_ite = m_collider_materials.find(p_hash);
				if (l_ite == std::cend(m_collider_materials))
				{
					const auto l_msg = core::bc_string_frame("No entry found for material with hash '") + core::bc_to_string_frame(p_hash) + "'";
					throw bc_key_not_found_exception(l_msg.c_str());
				}
			}

			return find_collider_material(p_hash);
		}

		bc_collider_material_description bc_material_manager::find_collider_material_throw(const bcCHAR* p_name) const
		{
			const auto l_hash = string_hash()(p_name);
			
			{
				platform::bc_lock_guard l_guard(m_materials_mutex);

				const auto l_ite = m_collider_materials.find(l_hash);
				if (l_ite == std::cend(m_collider_materials))
				{
					const auto l_msg = core::bc_string_frame("No entry found for material with name '") + p_name + "'";
					throw bc_key_not_found_exception(l_msg.c_str());
				}
			}

			return find_collider_material(p_name);
		}

		void bc_material_manager::destroy_mesh_material(bc_mesh_material* p_material)
		{
			const auto* l_material = static_cast<_bc_material_manager_material*>(p_material);

			{
				platform::bc_lock_guard l_guard(m_materials_mutex);

				const auto l_entry = m_materials.find(l_material->m_hash);

				BC_ASSERT(l_entry != std::cend(m_materials));

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

			const auto l_hash = p_color.to_rgba();

			{
				// We need to put all of these operations in one mutex block because after we found out that we do not
				// have this texture, creation and storing of texture must occur atomically
				platform::bc_lock_guard l_guard(m_materials_mutex);

				auto l_color_map_entry = m_default_color_textures.find(l_hash);
				if (l_color_map_entry != std::cend(m_default_color_textures))
				{
					return l_color_map_entry->second;
				}

				const graphic::bc_subresource_data l_color_map_init_data(&p_color, sizeof(core::bc_vector4f), 0);
				graphic::bc_texture2d_ref l_color_map_texture = m_render_system->get_device().create_texture2d
				(
					m_default_texture_config,
					&l_color_map_init_data
				);

				core::bc_estring_frame l_content_name = bcL("material_color_");
				l_content_name += core::bc_to_estring_frame(l_hash);

				auto l_color_map_texture_content = m_content_stream_manager->get_content_manager().store_content
				(
					core::bc_alloc_type::program,
					l_content_name.c_str(),
					graphic::bc_texture2d_content(std::move(l_color_map_texture))
				);

				l_color_map_entry = m_default_color_textures.insert(default_diffuse_map::value_type(l_hash, std::move(l_color_map_texture_content))).first;

				return l_color_map_entry->second;
			}
		}

		bc_mesh_material_ptr bc_material_manager::_store_mesh_material(core::bc_alloc_type p_alloc_type, const bcCHAR* p_name, bc_mesh_material p_material)
		{
			auto l_hash = string_hash()(p_name);
			auto l_material = core::bc_make_unique<_bc_material_manager_material>(p_alloc_type, l_hash, std::move(p_material));

			auto l_diffuse_map = l_material->get_diffuse_map();
			auto l_normal_map = l_material->get_normal_map();
			auto l_specular_map = l_material->get_specular_map();
			
			auto l_diffuse_map_view_config = graphic::bc_graphic_resource_builder()
				.as_resource_view()
				.as_texture_view(l_diffuse_map.get_format())
				.as_tex2d_shader_view(0, -1)
				.on_texture2d();
			auto l_normal_map_view_config = graphic::bc_graphic_resource_builder()
				.as_resource_view()
				.as_texture_view(l_normal_map.get_format())
				.as_tex2d_shader_view(0, -1)
				.on_texture2d();
			auto l_specular_map_view_config = graphic::bc_graphic_resource_builder()
				.as_resource_view()
				.as_texture_view(l_specular_map.get_format())
				.as_tex2d_shader_view(0, -1)
				.on_texture2d();
			auto l_parameters_cbuffer_config = graphic::bc_graphic_resource_builder()
				.as_resource()
				.as_buffer(1, sizeof(bc_mesh_material_parameter), graphic::bc_resource_usage::gpu_r, graphic::bc_resource_view_type::shader)
				.as_constant_buffer();
			auto l_parameters_cbuffer_data = graphic::bc_subresource_data(&l_material->m_parameters, 0, 0);

			auto& l_device = m_render_system->get_device();
			l_material->m_diffuse_map_view = l_device.create_resource_view(l_diffuse_map, l_diffuse_map_view_config);
			l_material->m_normal_map_view = l_device.create_resource_view(l_normal_map, l_normal_map_view_config);
			l_material->m_specular_map_view = l_device.create_resource_view(l_specular_map, l_specular_map_view_config);
			l_material->m_parameter_cbuffer = l_device.create_buffer(l_parameters_cbuffer_config, &l_parameters_cbuffer_data);

			{
				platform::bc_lock_guard l_guard(m_materials_mutex);

				auto l_ite = m_materials.insert(mesh_material_map::value_type(l_hash, core::bc_unique_ptr<bc_mesh_material>(std::move(l_material))));
				if (!l_ite.second)
				{
					const auto l_msg = core::bc_string_stream_frame() << "material with name '" << p_name << "' already exist in the materials map. newly loaded one will be discarded";
					core::bc_log(core::bc_log_type::error) << l_msg.str() << core::bc_lend;
				}
				else
				{
					l_ite.first->second->m_id = m_id_counter++;
				}

				return bc_mesh_material_ptr(l_ite.first->second.get(), _bc_mesh_material_ptr_deleter(this));
			}
		}
	}
}