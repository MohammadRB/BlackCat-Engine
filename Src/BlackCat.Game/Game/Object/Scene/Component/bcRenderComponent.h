// [04/28/2016 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_state_buffer;
		class bc_camera_instance;
		
		struct bc_actor_component_render_context
		{
			bc_actor_component_render_context(const bc_camera_instance& p_camera, bc_render_state_buffer& p_buffer)
				: m_camera(p_camera),
				m_buffer(p_buffer)
			{
			}

			const bc_camera_instance& m_camera;
			bc_render_state_buffer& m_buffer;
		};

		class BC_GAME_DLL bc_render_component : public bci_actor_component
		{
			BC_ABSTRACT_COMPONENT(render)

		public:
			virtual void render(const bc_actor_component_render_context& p_context) const = 0;

		protected:
			bc_render_component(bc_actor_index p_actor_index, bc_actor_component_index p_index);

			bc_render_component(bc_render_component&&) noexcept;

			virtual ~bc_render_component();

			bc_render_component& operator=(bc_render_component&&) noexcept;
		};
	}
}