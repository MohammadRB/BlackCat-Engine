// [04/23/2021 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/bcSceneCheckPoint.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_scene_checkpoint_loader_base
	{
	protected:
		core::bc_vector_frame<game::bc_actor> load_checkpoint_file(game::bc_scene& p_scene, core::bc_string_view p_file_content) const;

		core::bc_string_frame save_checkpoint_file(const core::bc_vector_frame<game::bc_actor>& p_actors) const;
	};

	template<class TCheckPoint = game::bc_scene_checkpoint>
	class bc_scene_checkpoint_loader : bc_scene_checkpoint_loader_base, public core::bc_base_content_loader
	{
	public:
		bool support_offline_processing() const override;
		
		void content_processing(core::bc_content_loading_context& p_context) const override;
		
		void content_processing(core::bc_content_saving_context& p_context) const override;
	};

	template<class TCheckPoint>
	bool bc_scene_checkpoint_loader<TCheckPoint>::support_offline_processing() const
	{
		return false;
	}

	template<class TCheckPoint>
	void bc_scene_checkpoint_loader<TCheckPoint>::content_processing(core::bc_content_loading_context& p_context) const
	{
		static_assert(std::is_base_of_v<game::bc_scene_checkpoint, TCheckPoint>);

		auto* l_scene = const_cast<game::bc_scene*>(p_context.m_instance_parameters.get_value_throw<game::bc_scene*>("scene"));
		if(!l_scene)
		{
			core::bc_log(core::bc_log_type::error, bcL("Cannot load scene checkpoint because scene is not presented in loader parameters"));
			return;
		}

		const core::bc_string_frame l_json_str(p_context.m_file_buffer_size + 1, '\0');
		std::memcpy(const_cast<bcCHAR*>(l_json_str.data()), p_context.m_file_buffer.get(), p_context.m_file_buffer_size);

		TCheckPoint l_check_point(*l_scene);
		l_check_point.remove_dynamic_actors();

		load_checkpoint_file(*l_scene, l_json_str);

		p_context.set_result(std::move(l_check_point));
	}

	template<class TCheckPoint>
	void bc_scene_checkpoint_loader<TCheckPoint>::content_processing(core::bc_content_saving_context& p_context) const
	{
		static_assert(std::is_base_of_v<game::bc_scene_checkpoint, TCheckPoint>);

		const auto* l_check_point = static_cast<TCheckPoint*>(p_context.m_content);
		const auto l_dynamic_actors = l_check_point->export_dynamic_actors();
		const auto l_json_str = save_checkpoint_file(l_dynamic_actors);
		
		p_context.m_file.write(l_json_str.c_str(), sizeof(decltype(l_json_str)::value_type) * l_json_str.size());
	}
}