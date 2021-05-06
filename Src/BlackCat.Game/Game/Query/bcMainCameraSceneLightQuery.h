// [04/29/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Core/Utility/bcNullable.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/System/Render/Light/bcLightInstance.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_main_camera_scene_light_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(sc_lgt)

		public:
			bc_main_camera_scene_light_query(bc_light_type p_types) noexcept;

			bc_main_camera_scene_light_query(bc_main_camera_scene_light_query&&) noexcept;

			~bc_main_camera_scene_light_query();

			bc_main_camera_scene_light_query& operator=(bc_main_camera_scene_light_query&&) noexcept;

			core::bc_vector<bc_light_instance> get_lights() noexcept;

		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override;

		private:
			bc_light_type m_types;
			core::bc_nullable<bc_camera_frustum> m_frustum;
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
			m_frustum(std::move(p_other.m_frustum)),
			m_lights(std::move(p_other.m_lights))
		{
		}

		inline bc_main_camera_scene_light_query::~bc_main_camera_scene_light_query() = default;

		inline bc_main_camera_scene_light_query& bc_main_camera_scene_light_query::operator=(bc_main_camera_scene_light_query&& p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_types = p_other.m_types;
			m_frustum = std::move(p_other.m_frustum);
			m_lights = std::move(p_other.m_lights);

			return *this;
		}

		inline core::bc_vector<bc_light_instance> bc_main_camera_scene_light_query::get_lights() noexcept
		{
			return std::move(m_lights);
		}
	}
}