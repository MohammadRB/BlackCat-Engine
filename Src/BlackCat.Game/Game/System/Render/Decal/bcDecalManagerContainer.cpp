// [03/02/2022 MRB]

#include "Game/GamePCH.h"

#include "Core/Concurrency/bcConcurrency.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/System/Render/Decal/bcDecalManagerContainer.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/Event/bcRemoveDecalActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_decal_manager_container::bc_decal_manager_container(bc_decal_manager& p_decal_manager) noexcept
			: m_decal_manager(&p_decal_manager),
			m_update_interval_seconds(0),
			m_decal_instances_pool(1000)
		{
		}

		bc_decal_manager_container::bc_decal_manager_container(bc_decal_manager_container&& p_other) noexcept
			: m_decal_manager(p_other.m_decal_manager),
			m_update_interval_seconds(p_other.m_update_interval_seconds),
			m_decal_instances_pool(std::move(p_other.m_decal_instances_pool))
		{
		}

		bc_decal_manager_container& bc_decal_manager_container::operator=(bc_decal_manager_container&& p_other) noexcept
		{
			m_decal_manager = p_other.m_decal_manager;
			m_update_interval_seconds = p_other.m_update_interval_seconds;
			m_decal_instances_pool = std::move(p_other.m_decal_instances_pool);

			return *this;
		}

		bool bc_decal_manager_container::is_decal_auto_remove(core::bc_string_view p_name) const noexcept
		{
			return m_decal_manager->is_decal_auto_remove(p_name);
		}

		bc_decal_manager_container::iterator_buffer bc_decal_manager_container::get_iterator_buffer() const noexcept
		{
			return iterator_buffer(m_decal_instances_mutex, m_decal_instances_pool);
		}

		bc_decal_instance* bc_decal_manager_container::create_decal(core::bc_string_view p_name,
			const core::bc_vector3f& p_local_position,
			const core::bc_matrix3f& p_local_rotation)
		{
			auto l_ptr = create_decal(p_name, bc_actor(), p_local_position, p_local_rotation, bc_mesh_node::s_invalid_index);
			return l_ptr.release();
		}

		bc_decal_instance* bc_decal_manager_container::create_decal(core::bc_string_view p_name,
			const core::bc_vector3f& p_local_position,
			const core::bc_matrix3f& p_local_rotation,
			bc_actor_render_group p_render_group)
		{
			auto l_ptr = create_decal(p_name, bc_actor(), p_local_position, p_local_rotation, p_render_group, bc_mesh_node::s_invalid_index);
			return l_ptr.release();
		}

		bc_decal_instance_ptr bc_decal_manager_container::create_decal(core::bc_string_view p_name,
			const bc_actor& p_actor,
			const core::bc_vector3f& p_local_position,
			const core::bc_matrix3f& p_local_rotation,
			bc_mesh_node::node_index_t p_attached_node_index)
		{
			auto l_decal_ptr = m_decal_manager->load_decal(p_name);
			if (!l_decal_ptr)
			{
				return nullptr;
			}

			{
				platform::bc_mutex_guard l_lock(m_decal_instances_mutex);

				m_decal_instances_pool.push_back(_bc_decal_instance_entry(bc_decal_instance
				(
					std::move(l_decal_ptr), p_actor, p_local_position, p_local_rotation, p_attached_node_index
				)));

				const auto l_ite = m_decal_instances_pool.rbegin();
				l_ite->m_iterator = l_ite.base();

				return bc_decal_instance_ptr(&*l_ite, bc_decal_instance_deleter(*this));
			}
		}

		bc_decal_instance_ptr bc_decal_manager_container::create_decal(core::bc_string_view p_name,
			const bc_actor& p_actor,
			const core::bc_vector3f& p_local_position,
			const core::bc_matrix3f& p_local_rotation,
			bc_actor_render_group p_render_group,
			bc_mesh_node::node_index_t p_attached_node_index)
		{
			auto l_decal_ptr = m_decal_manager->load_decal(p_name);
			if (!l_decal_ptr)
			{
				return nullptr;
			}

			{
				platform::bc_mutex_guard l_lock(m_decal_instances_mutex);

				m_decal_instances_pool.push_back(_bc_decal_instance_entry(bc_decal_instance
				(
					std::move(l_decal_ptr), p_actor, p_local_position, p_local_rotation, p_render_group, p_attached_node_index
				)));

				const auto l_ite = m_decal_instances_pool.rbegin();
				l_ite->m_iterator = l_ite.base();

				return bc_decal_instance_ptr(&*l_ite, bc_decal_instance_deleter(*this));
			}
		}

		void bc_decal_manager_container::update_decal_lifespans(const platform::bc_clock::update_param& p_clock) noexcept
		{
			m_update_interval_seconds += p_clock.m_elapsed;
			if (m_update_interval_seconds < 500)
			{
				return;
			}

			m_update_interval_seconds = 0;

			{
				platform::bc_mutex_guard l_lock(m_decal_instances_mutex);

				const bcUINT32 l_threshold_size = m_decal_instances_pool.get_memory_pool().capacity() * 0.9f;
				if (m_decal_instances_pool.size() < l_threshold_size)
				{
					return;
				}

				auto l_num_to_free = m_decal_instances_pool.size() - l_threshold_size;
				auto l_begin = std::begin(m_decal_instances_pool);
				const auto l_end = std::end(m_decal_instances_pool);

				while (l_num_to_free && l_begin != l_end)
				{
					bc_decal_instance& l_instance = *l_begin;
					if (!l_instance.get_decal()->get_auto_remove())
					{
						++l_begin;
						continue;
					}

					if (!l_instance.get_actor().is_valid())
					{
						l_begin = m_decal_instances_pool.erase(l_begin);

						//++l_begin;
						--l_num_to_free;
						continue;
					}

					l_instance.get_actor().add_event(bc_remove_decal_actor_event(l_instance));
					++l_begin;
					--l_num_to_free;
				}
			}
		}

		core::bc_task<void> bc_decal_manager_container::update_decal_lifespans_async(const platform::bc_clock::update_param& p_clock) noexcept
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate<void()>
				(
					[&]()
					{
						update_decal_lifespans(p_clock);
					}
				)
			);

			return l_task;
		}

		void bc_decal_manager_container::destroy_decal_instance(bc_decal_instance* p_instance)
		{
			const auto* l_entry = static_cast<_bc_decal_instance_entry*>(p_instance);

			//if (l_entry->get_decal()->get_temporary() && !l_entry->get_actor().is_valid())
			//{
			//	return; // Temporary instances with no actor will be removed automatically
			//}

			{
				platform::bc_mutex_guard l_lock(m_decal_instances_mutex);
				m_decal_instances_pool.erase(l_entry->m_iterator);
			}
		}
	}
}
