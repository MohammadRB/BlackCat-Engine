// [01/12/2017 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Content/bcContent.h"
#include "Core/bcConstant.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/SceneGraph/bcScenceGraph.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Render/bcShapeDrawer.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_scene : public core::bc_icontent
		{
			BC_CONTENT(scene)

		public:
			bc_scene(core::bc_string p_name,
				core::bc_vector<core::bc_string> p_loaded_streams,
				bc_scene_graph p_scene_graph, 
				physics::bc_scene_ref p_px_scene);

			bc_scene(bc_scene&&) noexcept;

			~bc_scene();

			bc_scene& operator=(bc_scene&&) noexcept;

			bc_scene_graph& get_scene_graph()
			{
				return m_scene_graph;
			}

			const bc_scene_graph& get_scene_graph() const
			{
				return m_scene_graph;
			}

			physics::bc_scene& get_px_scene()
			{
				return m_px_scene.get();
			}

			const physics::bc_scene& get_px_scene() const
			{
				return m_px_scene.get();
			}

			template<typename TComponent>
			bc_scene_graph_buffer get_actors() const;

			bc_scene_graph_buffer get_actors(const bc_camera_frustum& p_camera_frustum) const;

			void add_actor(bc_actor& p_actor);

			void remove_actor(bc_actor& p_actor);

			void render_debug_shapes(bc_shape_drawer& p_shape_drawer) const;

			void update(bc_physics_system& p_physics, core_platform::bc_clock::update_param p_time);

		protected:

		private:
			core::bc_string m_name;
			core::bc_vector<core::bc_string> m_loaded_streams;
			bc_scene_graph m_scene_graph;
			physics::bc_scene_ref m_px_scene;
		};

		using bc_scene_ptr = core::bc_content_ptr<bc_scene>;

		template< typename TComponent >
		bc_scene_graph_buffer bc_scene::get_actors() const
		{
			return m_scene_graph.get_actors<TComponent>();
		}
	}
}