// [04/12/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Core/Utility/bcNullable.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_scene_decal_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(sc_dcl)

		public:
			bc_scene_decal_query() noexcept;

			bc_scene_decal_query(const core::bc_vector3f& p_camera_position, bc_render_state_buffer p_buffer) noexcept;

			bc_scene_decal_query(bc_scene_decal_query&&) noexcept;

			~bc_scene_decal_query() override;

			bc_scene_decal_query& operator=(bc_scene_decal_query&&) noexcept;

			bc_scene_decal_query& with(const bc_camera_frustum& p_frustum);

			bc_render_state_buffer get_render_state_buffer() noexcept;

		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override;

		private:
			core::bc_vector3f m_camera_position;
			core::bc_nullable<bc_camera_frustum> m_frustum;
			bc_render_state_buffer m_buffer;
		};

		inline bc_scene_decal_query::bc_scene_decal_query() noexcept
			: bc_query(message_name())
		{
		}

		inline bc_scene_decal_query::bc_scene_decal_query(const core::bc_vector3f& p_camera_position, bc_render_state_buffer p_buffer) noexcept
			: bc_query(message_name()),
			m_camera_position(p_camera_position),
			m_buffer(std::move(p_buffer))
		{
		}

		inline bc_scene_decal_query::bc_scene_decal_query(bc_scene_decal_query&& p_other) noexcept = default;

		inline bc_scene_decal_query::~bc_scene_decal_query() = default;

		inline bc_scene_decal_query& bc_scene_decal_query::operator=(bc_scene_decal_query&& p_other) noexcept = default;

		inline bc_scene_decal_query& bc_scene_decal_query::with(const bc_camera_frustum& p_frustum)
		{
			m_frustum = p_frustum;
			return *this;
		}

		inline bc_render_state_buffer bc_scene_decal_query::get_render_state_buffer() noexcept
		{
			return std::move(m_buffer);
		}
	}
}