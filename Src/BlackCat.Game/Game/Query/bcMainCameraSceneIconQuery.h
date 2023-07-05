// [22/01/2022 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/Query/bcMainCameraSceneSharedQuery.h"

namespace black_cat::game
{
	struct bc_icon_info
	{
		core::bc_vector3f m_position;
		std::string_view m_name;
		bcUINT16 m_size;
		bc_icon_mode m_mode;
	};

	class bc_main_camera_scene_icon_query : public core::bc_query<bc_scene_query_context>
	{
		BC_QUERY(mcsgq)

	public:
		bc_main_camera_scene_icon_query();

		bc_main_camera_scene_icon_query(bc_main_camera_scene_icon_query&&) noexcept;

		~bc_main_camera_scene_icon_query() override;

		bc_main_camera_scene_icon_query& operator=(bc_main_camera_scene_icon_query&&) noexcept;

		bc_main_camera_scene_icon_query& include_editor_icons() noexcept;

		bc_main_camera_scene_icon_query& include_decal_icons() noexcept;

		core::bc_vector<bc_icon_info> get_icons() noexcept;

	protected:
		void execute(const bc_scene_query_context& p_context) noexcept override;

	private:
		bool m_include_editor_icons;
		bool m_include_decal_icons;
		core::bc_vector<bc_icon_info> m_icons;
	};

	inline bc_main_camera_scene_icon_query::bc_main_camera_scene_icon_query()
		: bc_query(message_name()),
		  m_include_editor_icons(false),
		  m_include_decal_icons(false)
	{
	}

	inline bc_main_camera_scene_icon_query::bc_main_camera_scene_icon_query(bc_main_camera_scene_icon_query&&) noexcept = default;

	inline bc_main_camera_scene_icon_query::~bc_main_camera_scene_icon_query() = default;

	inline bc_main_camera_scene_icon_query& bc_main_camera_scene_icon_query::operator=(bc_main_camera_scene_icon_query&&) noexcept = default;

	inline bc_main_camera_scene_icon_query& bc_main_camera_scene_icon_query::include_editor_icons() noexcept
	{
		m_include_editor_icons = true;
		return *this;
	}

	inline bc_main_camera_scene_icon_query& bc_main_camera_scene_icon_query::include_decal_icons() noexcept
	{
		m_include_decal_icons = true;
		return *this;
	}

	inline core::bc_vector<bc_icon_info> bc_main_camera_scene_icon_query::get_icons() noexcept
	{
		return std::move(m_icons);
	}

	inline void bc_main_camera_scene_icon_query::execute(const bc_scene_query_context& p_context) noexcept
	{
		if (!p_context.m_scene)
		{
			return;
		}

		const auto& l_main_camera_scene_buffer = p_context.get_shared_query<bc_main_camera_scene_shared_query>().get_scene_buffer();

		for (auto& l_actor : l_main_camera_scene_buffer)
		{
			const auto* l_icon_component = l_actor.get_component<bc_icon_component>();
			if(!l_icon_component || !l_icon_component->has_icon())
			{
				continue;
			}

			if(l_icon_component->get_mode() == bc_icon_mode::editor && !m_include_editor_icons)
			{
				continue;
			}

			const auto* l_mediate_component = l_actor.get_component<bc_mediate_component>();

			m_icons.push_back(bc_icon_info
				{
					l_mediate_component->get_position(),
					l_icon_component->get_name(),
					l_icon_component->get_size(),
					l_icon_component->get_mode()
				});
		}

		if(m_include_editor_icons && m_include_decal_icons)
		{
			auto l_decals_iterator = p_context.m_scene->get_decal_manager().get_iterator_buffer();

			{
				platform::bc_lock_guard l_lock(l_decals_iterator);

				for (auto& l_decal_instance : l_decals_iterator)
				{
					if (l_decal_instance.get_decal()->get_auto_remove())
					{
						continue;
					}

					const auto l_decal_transform = l_decal_instance.get_world_transform();
					const auto l_position = l_decal_transform.get_translation() + core::bc_vector3f::normalize(l_decal_transform.get_basis_y());
					m_icons.push_back
					(
						bc_icon_info
						{
							l_position,
							bc_icon_type::decal,
							bc_icon_component::s_default_icon_size,
							bc_icon_mode::editor
						}
					);
				}
			}
		}
	}
}
