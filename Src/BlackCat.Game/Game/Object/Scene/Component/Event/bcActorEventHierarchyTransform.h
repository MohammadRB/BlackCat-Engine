// [01/17/2021 MRB]

#pragma once

#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_actor_event_hierarchy_transform : public bc_actor_event
		{
			BC_EVENT(a_h_t)

		public:
			explicit bc_actor_event_hierarchy_transform(const bc_sub_mesh_mat4_transform& p_transforms);
			
			explicit bc_actor_event_hierarchy_transform(const bc_sub_mesh_px_transform& p_px_transforms);
			
			bc_actor_event_hierarchy_transform(const bc_actor_event_hierarchy_transform& p_other);

			~bc_actor_event_hierarchy_transform();

			bc_actor_event_hierarchy_transform& operator=(const bc_actor_event_hierarchy_transform& p_other);

			const bc_sub_mesh_mat4_transform* get_mat4_transforms() const noexcept;
			
			const bc_sub_mesh_px_transform* get_px_transforms() const noexcept;

		private:
			const bc_sub_mesh_mat4_transform* m_mat4_transforms;
			const bc_sub_mesh_px_transform* m_px_transforms;
		};

		inline bc_actor_event_hierarchy_transform::bc_actor_event_hierarchy_transform(const bc_sub_mesh_mat4_transform& p_transforms)
			: bc_actor_event(message_name()),
			m_mat4_transforms(&p_transforms),
			m_px_transforms(nullptr)
		{
		}

		inline bc_actor_event_hierarchy_transform::bc_actor_event_hierarchy_transform(const bc_sub_mesh_px_transform& p_px_transforms)
			: bc_actor_event(message_name()),
			m_mat4_transforms(nullptr),
			m_px_transforms(&p_px_transforms)
		{
		}

		inline bc_actor_event_hierarchy_transform::bc_actor_event_hierarchy_transform(const bc_actor_event_hierarchy_transform& p_other) = default;

		inline bc_actor_event_hierarchy_transform::~bc_actor_event_hierarchy_transform() = default;

		inline bc_actor_event_hierarchy_transform& bc_actor_event_hierarchy_transform::operator=(const bc_actor_event_hierarchy_transform& p_other) = default;

		inline const bc_sub_mesh_mat4_transform* bc_actor_event_hierarchy_transform::get_mat4_transforms() const noexcept
		{
			return m_mat4_transforms;
		}

		inline const bc_sub_mesh_px_transform* bc_actor_event_hierarchy_transform::get_px_transforms() const noexcept
		{
			return m_px_transforms;
		}
	}
}