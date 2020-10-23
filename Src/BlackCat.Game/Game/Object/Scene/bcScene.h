// [01/12/2017 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
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
		enum class _bc_scene_actor_operation
		{
			add, update, remove
		};
		
		class BC_GAME_DLL bc_scene : public core::bc_icontent
		{
			BC_CONTENT(scene)

		public:
			bc_scene(core::bc_estring p_path,
				core::bc_string p_name,
				core::bc_vector<core::bc_string> p_loaded_streams,
				bc_scene_graph p_scene_graph, 
				physics::bc_scene_ref p_px_scene);

			bc_scene(bc_scene&&) noexcept;

			~bc_scene();

			bc_scene& operator=(bc_scene&&) noexcept;

			const core::bc_estring& get_path() const;

			const core::bc_string& get_name() const;

			const core::bc_vector<core::bc_string>& get_loaded_streams() const;

			bc_scene_graph& get_scene_graph();

			const bc_scene_graph& get_scene_graph() const;

			physics::bc_scene& get_px_scene();

			const physics::bc_scene& get_px_scene() const;
				
			/**
			 * \brief ThreadSafe
			 * \param p_actor 
			 */
			void add_actor(bc_actor& p_actor);

			/**
			 * \brief ThreadSafe
			 * \param p_actor
			 */
			void update_actor(bc_actor& p_actor);

			/**
			 * \brief ThreadSafe
			 * \param p_actor
			 */
			void remove_actor(bc_actor& p_actor);

			void add_debug_shapes(bc_shape_drawer& p_shape_drawer) const;

			void update_px(bc_physics_system& p_physics, core_platform::bc_clock::update_param p_time);

			void update_graph();
			
		private:
			void _add_actor(bc_actor& p_actor);

			void _update_actor(bc_actor& p_actor);

			void _remove_actor(bc_actor& p_actor);
			
			core::bc_estring m_path;
			core::bc_string m_name;
			core::bc_vector<core::bc_string> m_loaded_streams;
			bc_scene_graph m_scene_graph;
			physics::bc_scene_ref m_px_scene;

			core_platform::bc_hybrid_mutex m_new_actors_lock;
			core::bc_vector_frame<std::tuple<_bc_scene_actor_operation, bc_actor>> m_new_actors;
		};

		using bc_scene_ptr = core::bc_content_ptr<bc_scene>;

		inline const core::bc_estring& bc_scene::get_path() const
		{
			return m_path;
		}

		inline const core::bc_string& bc_scene::get_name() const
		{
			return m_name;
		}

		inline const core::bc_vector<core::bc_string>& bc_scene::get_loaded_streams() const
		{
			return m_loaded_streams;
		}

		inline bc_scene_graph& bc_scene::get_scene_graph()
		{
			return m_scene_graph;
		}

		inline const bc_scene_graph& bc_scene::get_scene_graph() const
		{
			return m_scene_graph;
		}

		inline physics::bc_scene& bc_scene::get_px_scene()
		{
			return m_px_scene.get();
		}
		
		inline const physics::bc_scene& bc_scene::get_px_scene() const
		{
			return m_px_scene.get();
		}
	}
}