// [12/17/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/bcUtility.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/Pass/bcIntermediateTextureManager.h"

namespace black_cat
{
	namespace game
	{
		bool _are_configs_equal(const graphic::bc_texture_config& p_config1, const graphic::bc_texture_config& p_config2) noexcept
		{
			return p_config1.get_width() == p_config2.get_width() &&
				p_config1.get_height() == p_config2.get_height() &&
				p_config1.get_format() == p_config2.get_format() &&
				p_config1.get_sample_count().m_count == p_config2.get_sample_count().m_count &&
				p_config1.get_usage() == p_config2.get_usage() &&
				p_config1.get_view_types() == p_config2.get_view_types();
		}

		bc_intermediate_texture_manager::bc_intermediate_texture_manager(graphic::bc_device& p_device) noexcept
			: m_device(&p_device)
		{
		}

		bc_intermediate_texture_manager::bc_intermediate_texture_manager(bc_intermediate_texture_manager&& p_other) noexcept
			: m_device(p_other.m_device),
			m_textures(std::move(p_other.m_textures))
		{
		}

		bc_intermediate_texture_manager& bc_intermediate_texture_manager::operator=(bc_intermediate_texture_manager&& p_other) noexcept
		{
			m_device = p_other.m_device;
			m_textures = std::move(p_other.m_textures);
			return *this;
		}

		bc_intermediate_texture bc_intermediate_texture_manager::get_texture(const graphic::bc_texture_config& p_config)
		{
			{
				platform::bc_shared_mutex_shared_guard l_lock(m_mutex);

				for(auto& l_entry : m_textures)
				{
					if(!l_entry.m_is_in_use && _are_configs_equal(l_entry.m_config, p_config))
					{
						l_entry.m_is_in_use = true;
						return
						{
							*l_entry.m_texture,
							*l_entry.m_resource_view,
							*l_entry.m_unordered_resource_view,
							*l_entry.m_render_target_view
						};
					}
				}
			}

			auto l_texture = m_device->create_texture2d(p_config, nullptr);
			graphic::bc_resource_view_ref l_resource_view;
			graphic::bc_resource_view_ref l_unordered_view;
			graphic::bc_render_target_view_ref l_render_target_view;

			if(core::bc_enum::has(p_config.get_view_types(), graphic::bc_resource_view_type::shader))
			{
				const auto l_resource_view_config = graphic::bc_graphic_resource_builder()
					.as_resource_view()
					.as_texture_view(p_config.get_format())
					.as_tex2d_shader_view(0, 1)
					.on_texture2d();
				l_resource_view = m_device->create_resource_view(*l_texture, l_resource_view_config);
			}

			if (core::bc_enum::has(p_config.get_view_types(), graphic::bc_resource_view_type::unordered))
			{
				const auto l_unordered_view_config = graphic::bc_graphic_resource_builder()
					.as_resource_view()
					.as_texture_view(p_config.get_format())
					.as_tex2d_unordered_shader_view(0)
					.on_texture2d();
				l_unordered_view = m_device->create_resource_view(*l_texture, l_unordered_view_config);
			}

			if (core::bc_enum::has(p_config.get_view_types(), graphic::bc_resource_view_type::render_target))
			{
				const auto l_render_target_view_config = graphic::bc_graphic_resource_builder()
					.as_resource_view()
					.as_texture_view(p_config.get_format())
					.as_tex2d_render_target_view(0);
				l_render_target_view = m_device->create_render_target_view(*l_texture, l_render_target_view_config);
			}

			{
				platform::bc_shared_mutex_guard l_lock(m_mutex);

				m_textures.push_back(_bc_intermediate_texture_entry
				{
					std::move(l_texture),
					std::move(l_resource_view),
					std::move(l_unordered_view),
					std::move(l_render_target_view),
					p_config,
					true
				});

				auto& l_entry = m_textures.back();
				return
				{
					*l_entry.m_texture,
					*l_entry.m_resource_view,
					*l_entry.m_unordered_resource_view,
					*l_entry.m_render_target_view
				};
			}
		}

		void bc_intermediate_texture_manager::free_texture(const bc_intermediate_texture& p_texture)
		{
			{
				platform::bc_shared_mutex_shared_guard l_lock(m_mutex);

				const auto l_ite = std::find_if(std::begin(m_textures), std::end(m_textures), [&](const _bc_intermediate_texture_entry& p_entry)
				{
					return *p_entry.m_texture == p_texture.m_texture;
				});

				BC_ASSERT(l_ite != std::end(m_textures));

				l_ite->m_is_in_use = false;
			}
		}

		void bc_intermediate_texture_manager::before_reset(const bc_render_pass_reset_context& p_context)
		{
			if
			(
				p_context.m_old_parameters.m_width == p_context.m_new_parameters.m_width &&
				p_context.m_old_parameters.m_height == p_context.m_new_parameters.m_height
			)
			{
				return;
			}
			
			m_textures.clear();
		}

		void bc_intermediate_texture_manager::after_reset(const bc_render_pass_reset_context& p_context)
		{
		}

		bc_intermediate_texture_guard::bc_intermediate_texture_guard(bc_intermediate_texture_manager& p_manager, const graphic::bc_texture_config& p_config) noexcept
			: m_manager(&p_manager)
		{
			m_texture = m_manager->get_texture(p_config);
		}

		bc_intermediate_texture_guard::~bc_intermediate_texture_guard()
		{
			m_manager->free_texture(m_texture);
		}
	}
}