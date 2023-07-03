// [27/02/2017 MRB]

#pragma once

#include "GraphicImp/Resource/View/bcResourceView.h"
#include "App/Loader/bcHeightMapLoaderDx11.h"

namespace black_cat
{
	namespace editor
	{
		class bc_editor_height_map_dx11 : public bc_height_map_dx11
		{
		public:
			bc_editor_height_map_dx11(bc_height_map_dx11&& p_height_map,
				graphic::bc_resource_view_ref& p_height_map_unordered_view,
				graphic::bc_resource_view_ref& p_texture_map_unordered_view)
				: bc_height_map_dx11(std::move(p_height_map)),
				m_height_map_unordered_view(std::move(p_height_map_unordered_view)),
				m_texture_map_unordered_view(std::move(p_texture_map_unordered_view))
			{
			}

			bc_editor_height_map_dx11(bc_editor_height_map_dx11&&) = default;

			~bc_editor_height_map_dx11() override = default;

			bc_editor_height_map_dx11& operator=(bc_editor_height_map_dx11&&) = default;

			graphic::bc_resource_view get_height_map_unordered_view() const
			{
				return m_height_map_unordered_view.get();
			}

			graphic::bc_resource_view get_texture_map_unordered_view() const
			{
				return m_texture_map_unordered_view.get();
			}

		private:
			graphic::bc_resource_view_ref m_height_map_unordered_view;
			graphic::bc_resource_view_ref m_texture_map_unordered_view;
		};

		class bc_editor_height_map_loader_dx11 : public bc_height_map_loader_dx11
		{
		public:
			void content_processing(core::bc_content_loading_context& p_context) const override;

		protected:
			std::pair< graphic::bc_texture_config, graphic::bc_resource_view_config > get_height_map_texture_config(bcUINT32 p_width, bcUINT32 p_height) const override;

			std::pair< graphic::bc_texture_config, graphic::bc_resource_view_config > get_texture_map_texture_config(bcUINT32 p_width, bcUINT32 p_height) const override;
		};
	}
}