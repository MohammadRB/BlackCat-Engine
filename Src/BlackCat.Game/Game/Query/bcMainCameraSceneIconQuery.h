// [01/22/2022 MRB]

#pragma once

#include "Core/Messaging/Query/bcQuery.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/Query/bcMainCameraSceneSharedQuery.h"

namespace black_cat
{
	namespace game
	{
		struct bc_icon_info
		{
			core::bc_vector3f m_position;
			std::string_view m_name;
			bcUINT16 m_size;
			bc_icon_type m_type;
		};

		class bc_main_camera_scene_icon_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(mcsgq)

		public:
			bc_main_camera_scene_icon_query();

			bc_main_camera_scene_icon_query(bc_main_camera_scene_icon_query&&) noexcept;

			~bc_main_camera_scene_icon_query() override;

			bc_main_camera_scene_icon_query& operator=(bc_main_camera_scene_icon_query&&) noexcept;

			core::bc_vector<bc_icon_info> get_icons() noexcept;

		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override;

		private:
			core::bc_vector<bc_icon_info> m_icons;
		};

		inline bc_main_camera_scene_icon_query::bc_main_camera_scene_icon_query()
			: bc_query(message_name())
		{
		}

		inline bc_main_camera_scene_icon_query::bc_main_camera_scene_icon_query(bc_main_camera_scene_icon_query&&) noexcept = default;

		inline bc_main_camera_scene_icon_query::~bc_main_camera_scene_icon_query() = default;

		inline bc_main_camera_scene_icon_query& bc_main_camera_scene_icon_query::operator=(bc_main_camera_scene_icon_query&&) noexcept = default;

		inline core::bc_vector<bc_icon_info> bc_main_camera_scene_icon_query::get_icons() noexcept
		{
			return std::move(m_icons);
		}

		inline void bc_main_camera_scene_icon_query::execute(const bc_scene_query_context& p_context) noexcept
		{
			const auto& l_main_camera_scene_buffer = p_context.get_shared_query<bc_main_camera_scene_shared_query>().get_scene_buffer();

			for (auto& l_actor : l_main_camera_scene_buffer)
			{
				const auto* l_icon_component = l_actor.get_component<bc_icon_component>();
				if(!l_icon_component)
				{
					continue;
				}

				const auto* l_mediate_component = l_actor.get_component<bc_mediate_component>();

				m_icons.push_back(bc_icon_info
				{
					l_mediate_component->get_position(),
					l_icon_component->get_name(),
					l_icon_component->get_size(),
					l_icon_component->get_type()
				});
			}
		}
	}
}
