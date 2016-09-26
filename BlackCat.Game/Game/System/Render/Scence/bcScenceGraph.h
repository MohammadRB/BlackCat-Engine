// [07/14/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcVector.h"
#include "Game/bcExport.h"
#include "Game/Object/Scence/bcActor.hpp"

namespace black_cat
{
	namespace game
	{
		class bc_input_system;
		class bc_render_system;

		class BC_GAME_DLL bc_scence_graph
		{
		public:
			bc_scence_graph();

			bc_scence_graph(bc_scence_graph&&) = default;

			~bc_scence_graph();

			bc_scence_graph& operator=(bc_scence_graph&&) = default;

			void add_object(bc_actor p_actor);

			bc_actor remove_object(bc_actor p_actor);

			core::bc_vector_frame< bc_actor > get_heightmaps() const;

			void update(const bc_input_system& p_input_system, core_platform::bc_clock::update_param p_clock_update_param);

			void render_heightmaps(bc_render_system& p_render_system);

			void render_meshes(bc_render_system& p_render_system, bool p_preserve_render_instances);

			void clear();

		protected:

		private:
			core::bc_vector< bc_actor > m_objects;
		};
	}
}