// [12/01/2017 MRB]

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
		class bc_actor_component_manager_container;
		class bc_entity_manager;
		class bc_sound_manager;
		
		enum class _bc_scene_actor_state
		{
			add, update, remove_from_graph, removed_from_graph
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
				core::bc_vector<core::bc_string> p_material_files,
				core::bc_vector<core::bc_string> p_decal_files,
				core::bc_vector<core::bc_string> p_entity_files,
				core::bc_vector<core::bc_string> p_loaded_streams,
				bc_scene_graph p_scene_graph,
				physics::bc_scene_ref p_px_scene);

			bc_scene(bc_scene&&) noexcept;

			~bc_scene() override;

			bc_scene& operator=(bc_scene&&) noexcept;

			core::bc_estring_view get_path() const noexcept;

			core::bc_string_view get_name() const noexcept;

			const core::bc_vector<core::bc_string>& get_stream_files() const noexcept;

			const core::bc_vector<core::bc_string>& get_material_files() const noexcept;
			
			const core::bc_vector<core::bc_string>& get_decal_files() const noexcept;
			
			const core::bc_vector<core::bc_string>& get_entity_files() const noexcept;
			
			const core::bc_vector<core::bc_string>& get_loaded_streams() const noexcept;

			bcFLOAT get_global_scale() const noexcept;
			
			bc_scene_graph& get_scene_graph() noexcept;

			const bc_scene_graph& get_scene_graph() const noexcept;

			physics::bc_scene& get_px_scene() noexcept;

			const physics::bc_scene& get_px_scene() const noexcept;

			bc_actor_component_manager_container& get_actor_component_manager() noexcept;

			const bc_actor_component_manager_container& get_actor_component_manager() const noexcept;

			bc_bullet_manager& get_bullet_manager() noexcept;

			const bc_bullet_manager& get_bullet_manager() const noexcept;

			bc_light_manager& get_light_manager() noexcept;

			const bc_light_manager& get_light_manager() const noexcept;

			bc_particle_manager_container& get_particle_manager() noexcept;

			const bc_particle_manager_container& get_particle_manager() const noexcept;

			bc_decal_manager_container& get_decal_manager() noexcept;

			const bc_decal_manager_container& get_decal_manager() const noexcept;

			bc_sound_manager& get_sound_manager() noexcept;

			const bc_sound_manager& get_sound_manager() const noexcept;

			void paused() noexcept;

			void resumed() noexcept;

			void enable_bulk_loading(bcSIZE p_hint_size) noexcept;

			void disable_bulk_loading() noexcept;

			/**
			 * \brief ThreadSafe
			 * \param p_entity_name
			 * \param p_world_transform Initial world transform of actor
			 */
			bc_actor create_actor(const bcCHAR* p_entity_name, const core::bc_matrix4f& p_world_transform) noexcept;

			bc_actor create_actor(const bcCHAR* p_entity_name, const core::bc_matrix4f& p_world_transform, const core::bc_json_key_value& p_instance_parameters) noexcept;

			/**
			 * \brief ThreadSafe
			 * \param p_actor
			 */
			void update_actor(bc_actor p_actor) noexcept;

			/**
			 * \brief ThreadSafe
			 * \param p_actor
			 */
			void remove_actor(bc_actor p_actor) noexcept;

			/**
			 * \brief ThreadSafe
			 * \param p_bullet 
			 */
			void add_bullet(const bc_bullet& p_bullet) noexcept;

			void update_physics(const platform::bc_clock::update_param& p_clock, bool p_is_partial_update);
			
			core::bc_task<void> update_physics_async(const platform::bc_clock::update_param& p_clock, bool p_is_partial_update);

			void update_bullets(const platform::bc_clock::update_param& p_clock) noexcept;

			core::bc_task<void> update_bullets_async(const platform::bc_clock::update_param& p_clock) noexcept;
			
			void update_graph(const platform::bc_clock::update_param& p_clock) noexcept;
			
			core::bc_task<void> update_graph_async(const platform::bc_clock::update_param& p_clock) noexcept;

			/**
			 * \brief Process actor events to apply initial transforms and update graph.
			 * \n This function must be called after scene or checkpoint load to process all changes in actors and scene graph
			 * \param p_clock 
			 */
			void process_actor_events_and_reform_graph(const platform::bc_clock::update_param& p_clock) noexcept;

			void draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const;

		private:
			void _add_actor(bc_actor& p_actor);

			void _update_actor(bc_actor& p_actor);
			
			void _remove_actor(_bc_scene_actor_state p_state, bc_actor& p_actor);
			
			core::bc_estring m_path;
			core::bc_string m_name;
			core::bc_vector<core::bc_string> m_stream_files;
			core::bc_vector<core::bc_string> m_material_files;
			core::bc_vector<core::bc_string> m_decal_files;
			core::bc_vector<core::bc_string> m_entity_files;
			core::bc_vector<core::bc_string> m_loaded_streams;

			bool m_loading_mode;
			bcFLOAT m_global_scale;
			bc_entity_manager* m_entity_manager;
			bc_physics_system* m_physics;
			bc_scene_graph m_scene_graph;
			physics::bc_scene_ref m_px_scene;
			core::bc_unique_ptr<bc_actor_component_manager_container> m_actor_component_manager;
			core::bc_unique_ptr<bc_bullet_manager> m_bullet_manager;
			core::bc_unique_ptr<bc_light_manager> m_light_manager;
			core::bc_unique_ptr<bc_particle_manager_container> m_particle_manager;
			core::bc_unique_ptr<bc_decal_manager_container> m_decal_manager;
			core::bc_unique_ptr<bc_sound_manager> m_sound_manager;

			platform::bc_spin_mutex m_actors_to_add_lock;
			platform::bc_spin_mutex m_actors_to_remove_lock;
			platform::bc_hybrid_mutex m_changed_actors_lock;
			core::bc_vector_movable<bc_actor> m_actors_to_add;
			core::bc_vector_movable<std::tuple<_bc_scene_actor_state, bc_actor>> m_actors_to_remove;
			core::bc_vector_movable<std::tuple<_bc_scene_actor_state, bc_actor>> m_changed_actors;
		};

		using bc_scene_ptr = core::bc_content_ptr<bc_scene>;

		inline core::bc_estring_view bc_scene::get_path() const noexcept
		{
			return m_path.c_str();
		}

		inline core::bc_string_view bc_scene::get_name() const noexcept
		{
			return m_name.c_str();
		}

		inline const core::bc_vector<core::bc_string>& bc_scene::get_stream_files() const noexcept
		{
			return m_stream_files;
		}

		inline const core::bc_vector<core::bc_string>& bc_scene::get_material_files() const noexcept
		{
			return m_material_files;
		}

		inline const core::bc_vector<core::bc_string>& bc_scene::get_decal_files() const noexcept
		{
			return m_decal_files;
		}

		inline const core::bc_vector<core::bc_string>& bc_scene::get_entity_files() const noexcept
		{
			return m_entity_files;
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

		inline bc_actor_component_manager_container& bc_scene::get_actor_component_manager() noexcept
		{
			return *m_actor_component_manager;
		}

		inline const bc_actor_component_manager_container& bc_scene::get_actor_component_manager() const noexcept
		{
			return *m_actor_component_manager;
		}

		inline bc_bullet_manager& bc_scene::get_bullet_manager() noexcept
		{
			return *m_bullet_manager;
		}

		inline const bc_bullet_manager& bc_scene::get_bullet_manager() const noexcept
		{
			return *m_bullet_manager;
		}

		inline bc_light_manager& bc_scene::get_light_manager() noexcept
		{
			return *m_light_manager;
		}

		inline const bc_light_manager& bc_scene::get_light_manager() const noexcept
		{
			return *m_light_manager;
		}

		inline bc_particle_manager_container& bc_scene::get_particle_manager() noexcept
		{
			return *m_particle_manager;
		}

		inline const bc_particle_manager_container& bc_scene::get_particle_manager() const noexcept
		{
			return *m_particle_manager;
		}

		inline bc_decal_manager_container& bc_scene::get_decal_manager() noexcept
		{
			return *m_decal_manager;
		}

		inline const bc_decal_manager_container& bc_scene::get_decal_manager() const noexcept
		{
			return *m_decal_manager;
		}

		inline bc_sound_manager& bc_scene::get_sound_manager() noexcept
		{
			return *m_sound_manager;
		}

		inline const bc_sound_manager& bc_scene::get_sound_manager() const noexcept
		{
			return *m_sound_manager;
		}
	}
}