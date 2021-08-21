// [01/12/2017 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Content/bcContent.h"
#include "Core/bcConstant.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Render/bcShapeDrawer.h"
#include "Game/System/Render/Light/bcLightManager.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/Object/Scene/Bullet/bcBulletManager.h"
#include "Game/Object/Scene/SceneGraph/bcScenceGraph.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		template<typename T>
		class bc_task;
	}
	
	namespace game
	{
		class bc_entity_manager;
		
		enum class _bc_scene_actor_operation
		{
			add, update, remove
		};

		enum class _bc_scene_actor_remove_state
		{
			to_remove_from_graph,
			removed_from_graph
		};
		
		class BC_GAME_DLL bc_scene : public core::bci_content
		{
			BC_CONTENT(scene)

		public:
			bc_scene(bc_entity_manager& p_entity_manager,
				bc_game_system& p_game_system,
				core::bc_estring p_path,
				core::bc_string p_name,
				core::bc_vector<core::bc_string> p_stream_files,
				core::bc_vector<core::bc_string> p_entity_files,
				core::bc_vector<core::bc_string> p_material_files,
				core::bc_vector<core::bc_string> p_decal_files,
				core::bc_vector<core::bc_string> p_loaded_streams,
				bc_scene_graph p_scene_graph,
				physics::bc_scene_ref p_px_scene);

			bc_scene(bc_scene&&) noexcept;

			~bc_scene() override;

			bc_scene& operator=(bc_scene&&) noexcept;

			const bcECHAR* get_path() const noexcept;

			const bcCHAR* get_name() const noexcept;

			const core::bc_vector<core::bc_string>& get_stream_files() const noexcept;

			const core::bc_vector<core::bc_string>& get_entity_files() const noexcept;

			const core::bc_vector<core::bc_string>& get_material_files() const noexcept;
			
			const core::bc_vector<core::bc_string>& get_loaded_streams() const noexcept;

			bcFLOAT get_global_scale() const noexcept;
			
			bc_scene_graph& get_scene_graph() noexcept;

			const bc_scene_graph& get_scene_graph() const noexcept;

			physics::bc_scene& get_px_scene() noexcept;

			const physics::bc_scene& get_px_scene() const noexcept;

			bc_light_manager& get_light_manager() noexcept;

			const bc_light_manager& get_light_manager() const noexcept;

			bc_particle_manager& get_particle_manager() noexcept;

			const bc_particle_manager& get_particle_manager() const noexcept;

			bc_decal_manager& get_decal_manager() noexcept;

			const bc_decal_manager& get_decal_manager() const noexcept;
			
			/**
			 * \brief ThreadSafe
			 * \param p_entity_name
			 * \param p_world_transform Initial world transform of actor
			 */
			bc_actor create_actor(const bcCHAR* p_entity_name, const core::bc_matrix4f& p_world_transform);

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

			/**
			 * \brief ThreadSafe
			 * \param p_bullet 
			 */
			void add_bullet(const bc_bullet& p_bullet);
			
			void draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const;

			void update_physics(const core_platform::bc_clock::update_param& p_clock);
			
			core::bc_task<void> update_physics_async(const core_platform::bc_clock::update_param& p_clock);

			void update_bullets(const core_platform::bc_clock::update_param& p_clock);

			core::bc_task<void> update_bullets_async(const core_platform::bc_clock::update_param& p_clock);
			
			void update_graph(const core_platform::bc_clock::update_param& p_clock);
			
			core::bc_task<void> update_graph_async(const core_platform::bc_clock::update_param& p_clock);
			
		private:
			void _add_actor(bc_actor& p_actor);

			void _update_actor(bc_actor& p_actor);
			
			void _remove_actor_event(_bc_scene_actor_remove_state p_state, bc_actor& p_actor);

			void _remove_actor(_bc_scene_actor_remove_state p_state, bc_actor& p_actor);
			
			core::bc_estring m_path;
			core::bc_string m_name;
			core::bc_vector<core::bc_string> m_stream_files;
			core::bc_vector<core::bc_string> m_entity_files;
			core::bc_vector<core::bc_string> m_material_files;
			core::bc_vector<core::bc_string> m_decal_files;
			core::bc_vector<core::bc_string> m_loaded_streams;

			bcFLOAT m_global_scale;
			bc_entity_manager* m_entity_manager;
			bc_physics_system* m_physics;
			bc_scene_graph m_scene_graph;
			physics::bc_scene_ref m_px_scene;
			core::bc_unique_ptr<bc_bullet_manager> m_bullet_manager;
			core::bc_unique_ptr<bc_light_manager> m_light_manager;
			core::bc_unique_ptr<bc_particle_manager> m_particle_manager;
			core::bc_unique_ptr<bc_decal_manager> m_decal_manager;

			core_platform::bc_hybrid_mutex m_changed_actors_lock;
			core::bc_vector_movable<std::tuple<_bc_scene_actor_operation, bc_actor>> m_changed_actors;
			core::bc_vector_movable<std::tuple<_bc_scene_actor_remove_state, bc_actor>> m_to_remove_actors;
		};

		using bc_scene_ptr = core::bc_content_ptr<bc_scene>;

		inline const bcECHAR* bc_scene::get_path() const noexcept
		{
			return m_path.c_str();
		}

		inline const bcCHAR* bc_scene::get_name() const noexcept
		{
			return m_name.c_str();
		}

		inline const core::bc_vector<core::bc_string>& bc_scene::get_stream_files() const noexcept
		{
			return m_stream_files;
		}

		inline const core::bc_vector<core::bc_string>& bc_scene::get_entity_files() const noexcept
		{
			return m_entity_files;
		}

		inline const core::bc_vector<core::bc_string>& bc_scene::get_material_files() const noexcept
		{
			return m_material_files;
		}
		
		inline const core::bc_vector<core::bc_string>& bc_scene::get_loaded_streams() const noexcept
		{
			return m_loaded_streams;
		}

		inline bcFLOAT bc_scene::get_global_scale() const noexcept
		{
			return m_global_scale;
		}
		
		inline bc_scene_graph& bc_scene::get_scene_graph() noexcept
		{
			return m_scene_graph;
		}

		inline const bc_scene_graph& bc_scene::get_scene_graph() const noexcept
		{
			return m_scene_graph;
		}

		inline physics::bc_scene& bc_scene::get_px_scene() noexcept
		{
			return m_px_scene.get();
		}
		
		inline const physics::bc_scene& bc_scene::get_px_scene() const noexcept
		{
			return m_px_scene.get();
		}

		inline bc_light_manager& bc_scene::get_light_manager() noexcept
		{
			return *m_light_manager;
		}

		inline const bc_light_manager& bc_scene::get_light_manager() const noexcept
		{
			return *m_light_manager;
		}

		inline bc_particle_manager& bc_scene::get_particle_manager() noexcept
		{
			return *m_particle_manager;
		}

		inline const bc_particle_manager& bc_scene::get_particle_manager() const noexcept
		{
			return *m_particle_manager;
		}

		inline bc_decal_manager& bc_scene::get_decal_manager() noexcept
		{
			return *m_decal_manager;
		}

		inline const bc_decal_manager& bc_scene::get_decal_manager() const noexcept
		{
			return *m_decal_manager;
		}
	}
}