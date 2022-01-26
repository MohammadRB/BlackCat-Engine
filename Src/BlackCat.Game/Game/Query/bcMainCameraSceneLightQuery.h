// [04/29/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/System/Render/Light/bcLightInstance.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/Query/bcMainCameraSceneLightSharedQuery.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_main_camera_scene_light_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(sc_lgt)

		public:
			bc_main_camera_scene_light_query(bc_light_type p_types) noexcept;

			bc_main_camera_scene_light_query(bc_main_camera_scene_light_query&&) noexcept;

			~bc_main_camera_scene_light_query() override;

			bc_main_camera_scene_light_query& operator=(bc_main_camera_scene_light_query&&) noexcept;

			core::bc_vector<bc_light_instance> get_lights() noexcept;

		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override;

		private:
			bc_light_type m_types;
			core::bc_vector<bc_light_instance> m_lights;
		};

		inline bc_main_camera_scene_light_query::bc_main_camera_scene_light_query(bc_light_type p_types) noexcept
			: bc_query(message_name()),
			m_types(p_types)
		{
		}

		inline bc_main_camera_scene_light_query::bc_main_camera_scene_light_query(bc_main_camera_scene_light_query&& p_other) noexcept
			: bc_query(p_other),
			m_types(p_other.m_types),
			m_lights(std::move(p_other.m_lights))
		{
		}

		inline bc_main_camera_scene_light_query::~bc_main_camera_scene_light_query() = default;

		inline bc_main_camera_scene_light_query& bc_main_camera_scene_light_query::operator=(bc_main_camera_scene_light_query&& p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_types = p_other.m_types;
			m_lights = std::move(p_other.m_lights);

			return *this;
		}

		inline core::bc_vector<bc_light_instance> bc_main_camera_scene_light_query::get_lights() noexcept
		{
			return std::move(m_lights);
		}

		inline void bc_main_camera_scene_light_query::execute(const bc_scene_query_context& p_context) noexcept
		{
			const bool l_need_direct = core::bc_enum::has(m_types, bc_light_type::direct);
			const bool l_need_spot = core::bc_enum::has(m_types, bc_light_type::spot);
			const bool l_need_point = core::bc_enum::has(m_types, bc_light_type::point);
			const auto l_light_instances = p_context.get_shared_query<bc_main_camera_scene_light_shared_query>().get_lights();

			for (const auto& l_light : l_light_instances)
			{
				const bool l_is_direct = l_need_direct && l_light.get_type() == bc_light_type::direct;
				if (l_is_direct)
				{
					m_lights.push_back(bc_light_instance(l_light));
					continue;
				}

				const bool l_is_spot = l_need_spot && l_light.get_type() == bc_light_type::spot;
				if (l_is_spot)
				{
					m_lights.push_back(bc_light_instance(l_light));
					continue;
				}

				const bool l_is_point = l_need_point && l_light.get_type() == bc_light_type::point;
				if (l_is_point)
				{
					m_lights.push_back(bc_light_instance(l_light));
					continue;
				}
			}
		}
	}
}