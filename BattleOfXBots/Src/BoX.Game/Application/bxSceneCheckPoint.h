// [24/02/2022 MRB]

#pragma once

#include "Game/Object/Scene/bcSceneCheckPoint.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_scene_checkpoint : public game::bc_scene_checkpoint
	{
		BC_CONTENT(scn_chp)

	public:
		explicit bx_scene_checkpoint(game::bc_scene& p_scene);

		bx_scene_checkpoint(bx_scene_checkpoint&&) noexcept = default;

		~bx_scene_checkpoint() override = default;

		bx_scene_checkpoint& operator=(bx_scene_checkpoint&&) noexcept = default;

		core::bc_vector_frame<game::bc_actor> export_dynamic_actors() const override;

		void remove_dynamic_actors() const override;
	};
}
