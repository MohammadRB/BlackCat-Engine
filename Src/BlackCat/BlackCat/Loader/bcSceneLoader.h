// [01/26/2019 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphNode.h"

#pragma once

namespace black_cat
{
	class bc_scene_loader : public core::bc_base_content_loader
	{
	public:
		explicit bc_scene_loader(game::bc_iscene_graph_node_factory p_scene_graph_factory);

		void content_offline_processing(core::bc_content_loading_context& p_context) const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;

		void content_processing(core::bc_content_saving_context& p_context) const override;

	private:
		game::bc_iscene_graph_node_factory m_scene_graph_factory;
	};
}