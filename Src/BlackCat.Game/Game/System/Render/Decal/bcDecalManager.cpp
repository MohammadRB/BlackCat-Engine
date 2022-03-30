// [02/21/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcString.h"
#include "Core/Container/bcStringStream.h"
#include "Core/Utility/bcLogger.h"
#include "Core/File/bcStream.h"
#include "Core/File/bcFileStream.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/bcUtility.h"

namespace black_cat
{
	namespace game
	{
		BC_JSON_STRUCTURE(_bc_decal_desc)
		{
			BC_JSON_VALUE(core::bc_string_frame, name);
			BC_JSON_VALUE(core::bc_string_frame, material_name);
			BC_JSON_VALUE_OP(bcUINT32, u0);
			BC_JSON_VALUE_OP(bcUINT32, v0);
			BC_JSON_VALUE_OP(bcUINT32, u1);
			BC_JSON_VALUE_OP(bcUINT32, v1);
			BC_JSON_VALUE(bcFLOAT, width);
			BC_JSON_VALUE(bcFLOAT, height);
			BC_JSON_VALUE(bcFLOAT, depth);
			BC_JSON_VALUE_OP(bcFLOAT, view_distance);
			BC_JSON_VALUE_OP(bcUINT32, group);
			BC_JSON_VALUE_OP(bool, auto_remove);
		};
		
		BC_JSON_STRUCTURE(_bc_decal_json)
		{
			BC_JSON_ARRAY(_bc_decal_desc, decals);
		};

		bc_decal_manager::bc_decal_manager(bc_material_manager& p_material_manager)
			: m_material_manager(&p_material_manager)
		{
			m_decals_pool.initialize(100, core::bc_alloc_type::unknown);
		}

		bc_decal_manager::bc_decal_manager(bc_decal_manager&& p_other) noexcept
			: m_material_manager(p_other.m_material_manager),
			m_decals_pool(std::move(p_other.m_decals_pool)),
			m_decal_descriptions(std::move(p_other.m_decal_descriptions)),
			m_decals(std::move(p_other.m_decals))
		{
		}

		bc_decal_manager::~bc_decal_manager() = default;

		bc_decal_manager& bc_decal_manager::operator=(bc_decal_manager&& p_other) noexcept
		{
			m_material_manager = p_other.m_material_manager;
			m_decals_pool = std::move(p_other.m_decals_pool);
			m_decal_descriptions = std::move(p_other.m_decal_descriptions);
			m_decals = std::move(p_other.m_decals);
			
			return *this;
		}

		bc_decal_manager_container bc_decal_manager::create_container()
		{
			return bc_decal_manager_container(*this);
		}

		void bc_decal_manager::read_decal_file(const bcECHAR* p_decal_file)
		{
			core::bc_file_stream l_json_file;
			core::bc_string_frame l_json_file_buffer;

			if (!l_json_file.open_read(p_decal_file))
			{
				const auto l_msg = core::bc_string_stream_frame() << "Error in reading decal file: " << p_decal_file;
				throw bc_io_exception(l_msg.str().c_str());
			}

			core::bc_read_all_lines(l_json_file, l_json_file_buffer);

			core::bc_json_document<_bc_decal_json> l_decal_json;
			l_decal_json.load(l_json_file_buffer.c_str());

			for (core::bc_json_object<_bc_decal_desc>& l_decal : l_decal_json->m_decals)
			{
				auto l_hash = string_hash()(l_decal->m_name->c_str());
				_bc_decal_desc_entry l_desc
				{
					l_decal->m_name->c_str(),
					l_decal->m_material_name->c_str(),
					l_decal->m_u0.has_value() ? core::bc_nullable(*l_decal->m_u0) : nullptr,
					l_decal->m_v0.has_value() ? core::bc_nullable(*l_decal->m_v0) : nullptr,
					l_decal->m_u1.has_value() ? core::bc_nullable(*l_decal->m_u1) : nullptr,
					l_decal->m_v1.has_value() ? core::bc_nullable(*l_decal->m_v1) : nullptr,
					*l_decal->m_width,
					*l_decal->m_height,
					*l_decal->m_depth,
					l_decal->m_view_distance.has_value() ? *l_decal->m_view_distance : _calculate_default_view_distance(*l_decal->m_width, *l_decal->m_height, *l_decal->m_depth),
					l_decal->m_group.has_value() ? static_cast<bc_actor_render_group>(*l_decal->m_group) : bc_actor_render_group::all,
					l_decal->m_auto_remove.has_value() ? *l_decal->m_auto_remove : false
				};

				auto l_ite = m_decal_descriptions.find(l_hash);
				if(l_ite == std::end(m_decal_descriptions))
				{
					m_decal_descriptions.insert(std::make_pair(l_hash, std::move(l_desc)));
				}
				else
				{
					l_ite->second = std::move(l_desc);
					core::bc_log(core::bc_log_type::warning) << "decal description with name '" << *l_decal->m_name << "' already exist. old decal will be replaced." << core::bc_lend;
				}
			}
		}

		core::bc_vector_frame<core::bc_string_view> bc_decal_manager::get_decal_names() const
		{
			core::bc_vector_frame<core::bc_string_view> l_result;
			l_result.reserve(m_decal_descriptions.size());

			for (const auto& [l_key, l_entry] : m_decal_descriptions)
			{
				if(l_entry.m_auto_remove)
				{
					continue;
				}
				
				l_result.push_back(l_entry.m_name);
			}

			std::sort(std::begin(l_result), std::end(l_result));

			return l_result;
		}

		bool bc_decal_manager::is_decal_auto_remove(core::bc_string_view p_name) const noexcept
		{
			const auto l_hash = string_hash()(p_name);
			const auto l_desc_ite = m_decal_descriptions.find(l_hash);

			return l_desc_ite == std::end(m_decal_descriptions) || l_desc_ite->second.m_auto_remove;
		}

		bc_decal_ptr bc_decal_manager::load_decal(core::bc_string_view p_name) noexcept
		{
			const auto l_hash = string_hash()(p_name);

			{
				platform::bc_mutex_guard l_lock(m_decals_mutex);

				const auto l_ite = m_decals.find(l_hash);
				if (l_ite != std::end(m_decals))
				{
					return bc_decal_ptr(l_ite->second, bc_decal_deleter(*this));
				}
			}

			const auto l_desc_ite = m_decal_descriptions.find(l_hash);
			if(l_desc_ite == std::cend(m_decal_descriptions))
			{
				core::bc_log(core::bc_log_type::warning) << "No entry was found for decal with name '" << p_name << "'" << core::bc_lend;
				return nullptr;
			}

			auto l_material = m_material_manager->load_mesh_material_throw(l_desc_ite->second.m_material_name.c_str());
			const auto l_diffuse_width = static_cast<bcFLOAT>(l_material->get_diffuse_map().get_width());
			const auto l_diffuse_height = static_cast<bcFLOAT>(l_material->get_diffuse_map().get_height());
			auto* l_decal = m_decals_pool.alloc(_bc_decal_entry
			(
				bc_decal
				(
					l_desc_ite->second.m_name,
					std::move(l_material), 
					l_desc_ite->second.m_u0.has_value() ? *l_desc_ite->second.m_u0 / l_diffuse_width : 0.f,
					l_desc_ite->second.m_v0.has_value() ? *l_desc_ite->second.m_v0 / l_diffuse_height : 0.f,
					l_desc_ite->second.m_u1.has_value() ? *l_desc_ite->second.m_u1 / l_diffuse_width : 1.f,
					l_desc_ite->second.m_v1.has_value() ? *l_desc_ite->second.m_v1 / l_diffuse_height : 1.f,
					l_desc_ite->second.m_width,
					l_desc_ite->second.m_height,
					l_desc_ite->second.m_depth,
					l_desc_ite->second.m_view_distance,
					l_desc_ite->second.m_group,
					l_desc_ite->second.m_auto_remove
				),
				l_hash
			));

			{
				platform::bc_mutex_guard l_lock(m_decals_mutex);
				
				m_decals.insert(std::make_pair(l_hash, l_decal));
			}

			return bc_decal_ptr(l_decal, bc_decal_deleter(*this));
		}

		bc_decal_ptr bc_decal_manager::load_decal_throw(core::bc_string_view p_name)
		{
			auto l_decal_ptr = load_decal(p_name);
			if(l_decal_ptr == nullptr)
			{
				const auto l_msg = core::bc_string_stream_frame() << "No entry was found for decal with name '" << p_name << "'";
				throw bc_key_not_found_exception(l_msg.str().c_str());
			}

			return l_decal_ptr;
		}

		void bc_decal_manager::destroy_decal(bc_decal* p_decal)
		{
			auto* l_entry = static_cast<_bc_decal_entry*>(p_decal);
			
			{
				platform::bc_mutex_guard l_lock(m_decals_mutex);
				const auto l_ite = m_decals.find(l_entry->m_hash);

				BC_ASSERT(l_ite != std::end(m_decals));

				m_decals.erase(l_entry->m_hash);
			}

			m_decals_pool.free(l_entry);
		}

		bcFLOAT bc_decal_manager::_calculate_default_view_distance(bcFLOAT p_width, bcFLOAT p_height, bcFLOAT p_depth)
		{
			const auto l_box_length = std::max({ p_width, p_height, p_depth });
			return l_box_length * 25 * bc_get_global_config().get_scene_global_view_distance_scale();
		}
	}	
}