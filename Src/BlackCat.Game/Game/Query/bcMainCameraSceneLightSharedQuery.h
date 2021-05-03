// [04/29/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Container/bcSpan.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/System/Render/Light/bcLightInstance.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_main_camera_scene_light_shared_query : public core::bc_query<bc_light_instances_query_context>
		{
			BC_SHARED_QUERY(sc_lgt)

		public:
			explicit bc_main_camera_scene_light_shared_query(const bc_camera_frustum& p_frustum) noexcept;

			bc_main_camera_scene_light_shared_query(bc_main_camera_scene_light_shared_query&&) noexcept;

			~bc_main_camera_scene_light_shared_query();

			bc_main_camera_scene_light_shared_query& operator=(bc_main_camera_scene_light_shared_query&&) noexcept;

			core::bc_const_span<bc_light_instance> get_lights() noexcept;

		protected:
			void execute(const bc_light_instances_query_context& p_context) noexcept override;

		private:
			bc_camera_frustum m_frustum;
			core::bc_vector<bc_light_instance> m_lights;
		};

		inline bc_main_camera_scene_light_shared_query::bc_main_camera_scene_light_shared_query(const bc_camera_frustum& p_frustum) noexcept
			: bc_query(message_name()),
			m_frustum(p_frustum)
		{
		}

		inline bc_main_camera_scene_light_shared_query::bc_main_camera_scene_light_shared_query(bc_main_camera_scene_light_shared_query&& p_other) noexcept
			: bc_query(p_other),
			m_frustum(p_other.m_frustum),
			m_lights(std::move(p_other.m_lights))
		{
		}

		inline bc_main_camera_scene_light_shared_query::~bc_main_camera_scene_light_shared_query() = default;

		inline bc_main_camera_scene_light_shared_query& bc_main_camera_scene_light_shared_query::operator=(bc_main_camera_scene_light_shared_query&& p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_frustum = p_other.m_frustum;
			m_lights = std::move(p_other.m_lights);

			return *this;
		}

		inline core::bc_const_span<bc_light_instance> bc_main_camera_scene_light_shared_query::get_lights() noexcept
		{
			return core::bc_make_span(m_lights);
		}
	}
}