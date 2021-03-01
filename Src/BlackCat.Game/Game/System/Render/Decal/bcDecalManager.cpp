// [02/21/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcString.h"
#include "Core/File/bcStream.h"
#include "Core/File/bcFileStream.h"
#include "Core/File/bcJsonDocument.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/Object/Scene/Component/Event/bcRemoveDecalActorEvent.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/bcUtility.h"
#include "Game/System/Render/bcRenderSystem.h"

namespace black_cat
{
	namespace game
	{
		BC_JSON_STRUCTURE(_bc_decal_desc)
		{
			BC_JSON_VALUE(core::bc_string_frame, name);
			BC_JSON_VALUE(core::bc_string_frame, material_name);
			BC_JSON_VALUE(bcFLOAT, u0);
			BC_JSON_VALUE(bcFLOAT, v0);
			BC_JSON_VALUE(bcFLOAT, u1);
			BC_JSON_VALUE(bcFLOAT, v1);
			BC_JSON_VALUE(bcFLOAT, width);
			BC_JSON_VALUE(bcFLOAT, height);
			BC_JSON_VALUE_OP(bcUINT32, group);
			BC_JSON_VALUE_OP(bool, auto_remove);
		};
		
		BC_JSON_STRUCTURE(_bc_decal_json)
		{
			BC_JSON_ARRAY(_bc_decal_desc, decals);
		};

		bc_decal_manager::bc_decal_manager(bc_render_system& p_render_system, bc_material_manager& p_material_manager)
			: m_render_system(&p_render_system),
			m_material_manager(&p_material_manager),
			m_decal_instances_pool(2000, core::bc_alloc_type::program)
		{
			m_decals_pool.initialize(100, core::bc_alloc_type::program);
		}

		bc_decal_manager::~bc_decal_manager() = default;

		void bc_decal_manager::read_decal_file(const bcECHAR* p_decal_file)
		{
			core::bc_file_stream l_json_file;
			core::bc_string_frame l_json_file_buffer;

			if (!l_json_file.open_read(p_decal_file))
			{
				core::bc_string_frame l_msg = "Error in reading material file: ";
				l_msg += core::bc_to_string_frame(p_decal_file).c_str();

				throw bc_io_exception(l_msg.c_str());
			}

			core::bc_read_all_lines(l_json_file, l_json_file_buffer);

			core::bc_json_document< _bc_decal_json > l_decal_json;
			l_decal_json.load(l_json_file_buffer.c_str());

			for (core::bc_json_object<_bc_decal_desc>& l_decal : l_decal_json->m_decals)
			{
				auto l_hash = string_hash()(l_decal->m_name->c_str());
				_bc_decal_desc_entry l_desc
				{
					l_decal->m_material_name->c_str(),
					*l_decal->m_u0,
					*l_decal->m_v0,
					*l_decal->m_u1,
					*l_decal->m_v1,
					*l_decal->m_width,
					*l_decal->m_height,
					l_decal->m_group.get_had_value() ? static_cast<bc_actor_group>(*l_decal->m_group) : bc_actor_group::all,
					l_decal->m_auto_remove.get_had_value() ? *l_decal->m_auto_remove : false
				};
				m_decal_descriptions.insert(std::make_pair(l_hash, std::move(l_desc)));
			}
		}

		bc_decal_ptr bc_decal_manager::load_decal(const bcCHAR* p_name) noexcept
		{
			const auto l_hash = string_hash()(p_name);

			{
				core_platform::bc_mutex_guard l_lock(m_decals_mutex);
				const auto l_ite = m_decals.find(l_hash);
				if (l_ite != std::end(m_decals))
				{
					return bc_decal_ptr(l_ite->second, bc_decal_deleter(*this));
				}
			}

			const auto l_desc_ite = m_decal_descriptions.find(l_hash);
			if(l_desc_ite == std::cend(m_decal_descriptions))
			{
				return nullptr;
			}

			auto l_material = m_material_manager->load_mesh_material_throw(l_desc_ite->second.m_material_name.c_str());
			auto* l_decal = m_decals_pool.alloc(_bc_decal_entry
			(
				bc_decal
				(
					std::move(l_material), 
					l_desc_ite->second.m_u0,
					l_desc_ite->second.m_v0,
					l_desc_ite->second.m_u1,
					l_desc_ite->second.m_v1,
					l_desc_ite->second.m_width,
					l_desc_ite->second.m_height,
					l_desc_ite->second.m_group,
					l_desc_ite->second.m_auto_remove
				),
				l_hash
			));

			{
				core_platform::bc_mutex_guard l_lock(m_decals_mutex);
				m_decals.insert(std::make_pair(l_hash, l_decal));
			}

			return bc_decal_ptr(l_decal, bc_decal_deleter(*this));
		}

		bc_decal_ptr bc_decal_manager::load_decal_throw(const bcCHAR* p_name)
		{
			auto l_decal_ptr = load_decal(p_name);
			if(l_decal_ptr == nullptr)
			{
				core::bc_string_frame l_msg = "No entry found for decal with name '";
				l_msg += p_name;
				l_msg += "'";

				throw bc_key_not_found_exception(l_msg.c_str());
			}

			return l_decal_ptr;
		}

		/*bc_decal_instance* bc_decal_manager::create_decal(const bcCHAR* p_decal_name,
			const core::bc_vector3f& p_local_position,
			const core::bc_matrix3f& p_local_rotation,
			bc_mesh_node::node_index_t p_attached_node_index)
		{
			auto l_decal_ptr = load_decal_throw(p_decal_name);

			{
				core_platform::bc_mutex_guard l_lock(m_decals_instances_mutex);

				m_decal_instances_pool.push_back(_bc_decal_instance_entry(bc_decal_instance
				(
					std::move(l_decal_ptr), bc_actor(), p_local_position, p_local_rotation, p_attached_node_index
				)));

				auto l_ite = m_decal_instances_pool.rbegin();
				l_ite->m_iterator = l_ite.base();

				return &*l_ite;
			}
		}*/

		bc_decal_instance_ptr bc_decal_manager::create_decal(const bcCHAR* p_decal_name,
			const bc_actor& p_actor,
			const core::bc_vector3f& p_local_position,
			const core::bc_matrix3f& p_local_rotation,
			bc_mesh_node::node_index_t p_attached_node_index)
		{
			auto l_decal_ptr = load_decal_throw(p_decal_name);

			{
				core_platform::bc_mutex_guard l_lock(m_decals_instances_mutex);

				m_decal_instances_pool.push_back(_bc_decal_instance_entry(bc_decal_instance
				(
					std::move(l_decal_ptr), p_actor, p_local_position, p_local_rotation, p_attached_node_index
				)));

				auto l_ite = m_decal_instances_pool.rbegin();
				l_ite->m_iterator = l_ite.base();

				return bc_decal_instance_ptr(&*l_ite, bc_decal_instance_deleter(*this));
			}
		}

		void bc_decal_manager::update_decal_lifespans() noexcept
		{
			{
				core_platform::bc_mutex_guard l_lock(m_decals_instances_mutex);

				const bcUINT32 l_threshold_size = m_decal_instances_pool.get_memory_pool().capacity() * 0.9f;
				if(m_decal_instances_pool.size() < l_threshold_size)
				{
					return;
				}

				auto l_num_to_free = m_decal_instances_pool.size() - l_threshold_size;
				auto l_begin = std::begin(m_decal_instances_pool);
				const auto l_end = std::end(m_decal_instances_pool);

				while (l_num_to_free && l_begin != l_end)
				{
					bc_decal_instance& l_instance = *l_begin;
					if(!l_instance.get_decal()->get_temporary())
					{
						++l_begin;
						continue;
					}

					if(!l_instance.get_actor().is_valid())
					{
						l_begin = m_decal_instances_pool.erase(l_begin);

						++l_begin;
						--l_num_to_free;
						continue;
					}

					l_instance.get_actor().add_event(bc_remove_decal_actor_event(l_instance));
					++l_begin;
					--l_num_to_free;
				}
			}
		}

		void bc_decal_manager::destroy_decal(bc_decal* p_decal)
		{
			auto* l_entry = static_cast<_bc_decal_entry*>(p_decal);
			
			{
				core_platform::bc_mutex_guard l_lock(m_decals_mutex);
				const auto l_ite = m_decals.find(l_entry->m_hash);

				BC_ASSERT(l_ite != std::end(m_decals));

				m_decals.erase(l_entry->m_hash);
			}

			m_decals_pool.free(l_entry);
		}

		void bc_decal_manager::destroy_decal_instance(bc_decal_instance* p_instance)
		{
			auto* l_entry = static_cast< _bc_decal_instance_entry* >(p_instance);

			//if (l_entry->get_decal()->get_temporary() && !l_entry->get_actor().is_valid())
			//{
			//	return; // Temporary instances with no actor will be removed automatically
			//}

			{
				core_platform::bc_mutex_guard l_lock(m_decals_instances_mutex);
				m_decal_instances_pool.erase(l_entry->m_iterator);
			}
		}
	}	
}