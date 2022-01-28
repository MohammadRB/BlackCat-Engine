// [04/23/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Content/bcContent.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;

		class BC_GAME_DLL bc_scene_check_point : public core::bci_content
		{
			BC_CONTENT(scn_chp)

		public:
			explicit bc_scene_check_point(bc_scene& p_scene);

			bc_scene_check_point(bc_scene_check_point&&) noexcept;

			~bc_scene_check_point() override;

			bc_scene_check_point& operator=(bc_scene_check_point&&) noexcept;

			core::bc_vector_frame<bc_actor> export_dynamic_actors() const;

			void remove_dynamic_actors() const;

		private:
			bc_scene* m_scene;
		};
	}	
}