// [02/03/2022 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Concurrency/bcTask.h"
#include "Core/Container/bcListPool.h"
#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Game/System/Render/Decal/bcDecalIteratorBuffer.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_decal_manager;

	class _bc_decal_instance_entry : public bc_decal_instance
	{
	public:
		using iterator_type = core::bc_list_pool<_bc_decal_instance_entry>::iterator;

	public:
		_bc_decal_instance_entry(bc_decal_instance p_instance)
			: bc_decal_instance(std::move(p_instance)),
			  m_iterator(nullptr, nullptr)
		{
		}

		iterator_type m_iterator;
	};

	class BC_GAME_DLL bc_decal_manager_container
	{
	private:
		using decal_instance_container = core::bc_list_pool<_bc_decal_instance_entry>;

	public:
		using iterator_buffer = bc_decal_iterator_buffer<decal_instance_container>;

	public:
		explicit bc_decal_manager_container(bc_decal_manager& p_decal_manager) noexcept;

		bc_decal_manager_container(bc_decal_manager_container&&) noexcept;

		~bc_decal_manager_container() = default;

		bc_decal_manager_container& operator=(bc_decal_manager_container&&) noexcept;

		bool is_decal_auto_remove(core::bc_string_view p_name) const noexcept;

		iterator_buffer get_iterator_buffer() const noexcept;

		/**
		 * \brief Create a decal without a reference to owning actor.
		 * Decals without reference will be removed automatically.
		 * \n ThreadSafe
		 * \param p_name
		 * \param p_local_position
		 * \param p_local_rotation
		 * \return
		 */
		bc_decal_instance* create_decal(core::bc_string_view p_name,
		                                const core::bc_vector3f& p_local_position,
		                                const core::bc_matrix3f& p_local_rotation);

		bc_decal_instance* create_decal(core::bc_string_view p_name,
		                                const core::bc_vector3f& p_local_position,
		                                const core::bc_matrix3f& p_local_rotation,
		                                bc_actor_render_group p_render_group);

		/**
		 * \brief Create a decal with a strong reference to owning actor.
		 * Decals with strong reference will be removed when their reference count reach to zero.
		 * \n ThreadSafe
		 * \param p_name
		 * \param p_actor
		 * \param p_local_position
		 * \param p_local_rotation
		 * \param p_attached_node_index
		 * \return
		 */
		bc_decal_instance_ptr create_decal(core::bc_string_view p_name,
		                                   const bc_actor& p_actor,
		                                   const core::bc_vector3f& p_local_position,
		                                   const core::bc_matrix3f& p_local_rotation,
		                                   bc_mesh_node::node_index_t p_attached_node_index = bc_mesh_node::s_invalid_index);

		bc_decal_instance_ptr create_decal(core::bc_string_view p_name,
		                                   const bc_actor& p_actor,
		                                   const core::bc_vector3f& p_local_position,
		                                   const core::bc_matrix3f& p_local_rotation,
		                                   bc_actor_render_group p_render_group,
		                                   bc_mesh_node::node_index_t p_attached_node_index = bc_mesh_node::s_invalid_index);

		void update_decal_lifespans(const platform::bc_clock::update_param& p_clock) noexcept;

		core::bc_task<void> update_decal_lifespans_async(const platform::bc_clock::update_param& p_clock) noexcept;

		void clear_temp_decals();

		void destroy_decal_instance(bc_decal_instance* p_instance);

	private:
		bc_decal_manager* m_decal_manager;
		bcFLOAT m_update_interval_seconds;

		decal_instance_container m_decal_instances_pool;
		mutable platform::bc_mutex m_decal_instances_mutex;
	};
}
