// [04/23/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
#include "Core/Content/bcContent.h"
#include "Core/File/bcPath.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;

		class BC_GAME_DLL bc_scene_checkpoint : public core::bci_content
		{
			BC_CONTENT(scn_chp)

		public:
			explicit bc_scene_checkpoint(bc_scene& p_scene);

			bc_scene_checkpoint(bc_scene_checkpoint&&) noexcept;

			~bc_scene_checkpoint() override;

			bc_scene_checkpoint& operator=(bc_scene_checkpoint&&) noexcept;

			virtual core::bc_vector_frame<bc_actor> export_dynamic_actors() const;

			virtual void remove_dynamic_actors() const;

			static core::bc_path get_checkpoint_path(const bc_scene& p_scene, core::bc_estring_view p_checkpoint_name);

		protected:
			bc_scene* m_scene;
		};
	}	
}