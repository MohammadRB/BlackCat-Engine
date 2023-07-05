// [21/01/2022 MRB]

#pragma once

#include "Core/Container/bcUnorderedMap.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/Query/bcMainCameraSceneIconQuery.h"
#include "App/bcExport.h"

namespace black_cat
{
	struct _bc_sprite_texcoord
	{
		bcUINT32 m_index;
		bcFLOAT m_u0, m_v0, m_u1, m_v1;
	};

	struct _bc_sprite_icon
	{
		core::bc_vector3f m_position;
		bcUINT32 m_size;
		bcFLOAT m_u0, m_v0, m_u1, m_v1;
	};

	class BC_DLL bc_icon_draw_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(icn_drw)

	public:
		bc_icon_draw_pass(game::bc_render_pass_variable_t p_render_target_texture, 
			game::bc_render_pass_variable_t p_render_target_view,
			std::initializer_list<std::string_view> p_icons,
			const bcECHAR* p_sprites_texture_name,
			bcUINT32 p_sprites_width,
			bcUINT32 p_sprites_count) noexcept;

		bc_icon_draw_pass(bc_icon_draw_pass&&) noexcept;

		~bc_icon_draw_pass() override = default;

		bc_icon_draw_pass& operator=(bc_icon_draw_pass&&) noexcept;

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

		void draw_decal_icons(bool p_value);

	private:
		void _event_handler(core::bci_event& p_event);

		void _resize_sprites_buffer(graphic::bc_device& p_device);

		_bc_sprite_texcoord _map_icon_index_to_uv_coordinates(bcUINT32 p_sprite_icon_index, bcUINT32 p_sprites_texture_size) const;

		game::bc_render_pass_variable_t m_render_target_texture;
		game::bc_render_pass_variable_t m_render_target_view;
		const bcECHAR* m_sprites_texture_name;
		core::bc_unordered_map<std::string_view, _bc_sprite_texcoord> m_sprites_map;
		bcUINT32 m_sprites_width;
		bcUINT32 m_sprites_count_per_row;

		graphic::bc_texture2d_content_ptr m_sprites_texture;
		graphic::bc_resource_view_ref m_sprites_texture_view;
		graphic::bc_buffer_ref m_sprites_buffer;
		graphic::bc_resource_view_ref m_sprites_buffer_view;
		graphic::bc_shader_parameter_link m_sprites_buffer_view_link;
		graphic::bc_sampler_state_ref m_linear_sampler;
		graphic::bc_device_pipeline_state_ref m_device_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;

		bool m_editor_mode;
		bool m_draw_decal_icons;
		core::bc_query_result<game::bc_main_camera_scene_icon_query> m_icons_query;
		core::bc_vector<_bc_sprite_icon> m_sprites;

		core::bc_event_listener_handle m_editor_event_handle;
	};
}
