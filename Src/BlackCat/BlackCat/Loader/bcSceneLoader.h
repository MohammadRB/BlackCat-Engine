// [01/26/2019 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "Game/Application/bcEngineApplicationParameter.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_scene_loader : public core::bc_base_content_loader
	{
	public:
		bc_scene_loader(game::bc_px_scene_builder_factory p_px_scene_builder_factory, game::bc_scene_graph_node_factory p_scene_graph_factory);

		bool support_offline_processing() const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;

		void content_processing(core::bc_content_saving_context& p_context) const override;

	private:
		game::bc_px_scene_builder_factory m_px_scene_builder_factory;
		game::bc_scene_graph_node_factory m_scene_graph_factory;
	};
}