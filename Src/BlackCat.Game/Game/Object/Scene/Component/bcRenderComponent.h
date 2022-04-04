// [04/28/2016 MRB]

#pragma once

#include "Game/System/Input/bcCameraInstance.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_state_buffer;

		struct bc_actor_render_camera
		{
			bc_camera_instance m_main_camera;
			bc_camera_instance m_render_camera;

			explicit bc_actor_render_camera(const bc_camera_instance& p_main_camera)
				: m_main_camera(p_main_camera),
				m_render_camera(p_main_camera)
			{
			}
			
			bc_actor_render_camera(const bc_camera_instance& p_main_camera, const bc_camera_instance& p_render_camera)
				: m_main_camera(p_main_camera),
				m_render_camera(p_render_camera)
			{
			}
		};
		
		struct bc_actor_component_render_context
		{
			bc_actor_component_render_context(const bc_actor_render_camera& p_camera, bc_render_state_buffer& p_buffer)
				: m_camera(p_camera),
				m_buffer(p_buffer)
			{
			}

			const bc_actor_render_camera& m_camera;
			bc_render_state_buffer& m_buffer;
		};

		class BC_GAME_DLL bc_render_component : public bci_actor_abstract_component
		{
			BC_ABSTRACT_COMPONENT(render)

		public:
			virtual void render(const bc_actor_component_render_context& p_context) const = 0;

		protected:
			bc_render_component();

			bc_render_component(bc_render_component&&) noexcept;

			~bc_render_component() override;

			bc_render_component& operator=(bc_render_component&&) noexcept;
		};

		inline bc_render_component::bc_render_component()
			: bci_actor_abstract_component()
		{
		}

		inline bc_render_component::bc_render_component(bc_render_component&& p_other) noexcept = default;

		inline bc_render_component::~bc_render_component() = default;

		inline bc_render_component& bc_render_component::operator=(bc_render_component&& p_other) noexcept = default;
	}
}