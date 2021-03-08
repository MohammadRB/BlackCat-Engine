// [01/17/2021 MRB]

#pragma once

#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_hierarchy_transform_actor_event : public bc_actor_event
		{
			BC_EVENT(a_h_t)

		public:
			explicit bc_hierarchy_transform_actor_event(const bc_sub_mesh_mat4_transform& p_transforms);
			
			explicit bc_hierarchy_transform_actor_event(const bc_sub_mesh_px_transform& p_px_transforms);
			
			bc_hierarchy_transform_actor_event(const bc_hierarchy_transform_actor_event& p_other);

			~bc_hierarchy_transform_actor_event();

			bc_hierarchy_transform_actor_event& operator=(const bc_hierarchy_transform_actor_event& p_other);

			const bc_sub_mesh_mat4_transform* get_mat4_transforms() const noexcept;
			
			const bc_sub_mesh_px_transform* get_px_transforms() const noexcept;

		private:
			const bc_sub_mesh_mat4_transform* m_mat4_transforms;
			const bc_sub_mesh_px_transform* m_px_transforms;
		};

		inline bc_hierarchy_transform_actor_event::bc_hierarchy_transform_actor_event(const bc_sub_mesh_mat4_transform& p_transforms)
			: bc_actor_event(message_name()),
			m_mat4_transforms(&p_transforms),
			m_px_transforms(nullptr)
		{
		}

		inline bc_hierarchy_transform_actor_event::bc_hierarchy_transform_actor_event(const bc_sub_mesh_px_transform& p_px_transforms)
			: bc_actor_event(message_name()),
			m_mat4_transforms(nullptr),
			m_px_transforms(&p_px_transforms)
		{
		}

		inline bc_hierarchy_transform_actor_event::bc_hierarchy_transform_actor_event(const bc_hierarchy_transform_actor_event& p_other) = default;

		inline bc_hierarchy_transform_actor_event::~bc_hierarchy_transform_actor_event() = default;

		inline bc_hierarchy_transform_actor_event& bc_hierarchy_transform_actor_event::operator=(const bc_hierarchy_transform_actor_event& p_other) = default;

		inline const bc_sub_mesh_mat4_transform* bc_hierarchy_transform_actor_event::get_mat4_transforms() const noexcept
		{
			return m_mat4_transforms;
		}

		inline const bc_sub_mesh_px_transform* bc_hierarchy_transform_actor_event::get_px_transforms() const noexcept
		{
			return m_px_transforms;
		}
	}
}