// [01/22/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcString.h"
#include "Graphic/bcEvent.h"
#include "GraphicImp/Resource/Texture/bcTextureConfig.h"
#include "Game/System/Render/Pass/bcRenderPassResourceManager.h"
#include "Game/System/Render/Pass/bcIntermediateTextureManager.h"
#include "Game/System/Input/bcCameraInstance.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace core
	{
		class bc_query_manager;
	}
	
	namespace game
	{
		class bc_render_system;
		class bc_frame_renderer;
		class bc_render_thread;
		class bc_default_render_thread;
		class bc_scene;
		class bc_render_pass_manager;
		class bc_global_config;
		
		template<typename TPass>
		struct bc_render_pass_trait
		{
		public:
			static constexpr const bcCHAR* render_pass_name()
			{
				return TPass::render_pass_name();
			}
			
			static constexpr core::bc_string_cmp_hash render_pass_hash()
			{
				return TPass::render_pass_hash();
			}
		};

		class bc_render_pass_update_context
		{
		public:
			bc_render_pass_update_context(const platform::bc_clock::update_param& p_clock,
				const bc_camera_instance& p_camera)
				: m_clock(p_clock),
				m_camera(p_camera)
			{
			}

			platform::bc_clock::update_param m_clock;
			bc_camera_instance m_camera;
		};

		class bc_render_pass_render_context
		{
		public:
			bc_render_pass_render_context(const platform::bc_clock::update_param& p_clock,
				core::bc_query_manager& p_query_manager,
				const bc_camera_instance& p_update_camera,
				const bc_camera_instance& p_render_camera,
				bc_render_system& p_render_system,
				bc_frame_renderer& p_frame_renderer,
				bc_default_render_thread& p_render_thread)
				: m_clock(p_clock),
				m_query_manager(p_query_manager),
				m_update_camera(p_update_camera),
				m_render_camera(p_render_camera),
				m_render_system(p_render_system),
				m_frame_renderer(p_frame_renderer),
				m_render_thread(p_render_thread)
			{
			}

			platform::bc_clock::update_param m_clock;
			core::bc_query_manager& m_query_manager;
			bc_camera_instance m_update_camera;
			bc_camera_instance m_render_camera;
			bc_render_system& m_render_system;
			bc_frame_renderer& m_frame_renderer;
			bc_default_render_thread& m_render_thread;
		};

		class bc_render_pass_reset_context
		{
		public:
			bc_render_pass_reset_context(bc_render_system& p_render_system,
				graphic::bc_device& p_device,
				graphic::bc_device_swap_buffer& p_device_swap_buffer,
				const graphic::bc_device_parameters& p_old_parameters,
				const graphic::bc_device_parameters& p_new_parameters)
				: m_render_system(p_render_system),
				m_device(p_device),
				m_device_swap_buffer(p_device_swap_buffer),
				m_old_parameters(p_old_parameters),
				m_new_parameters(p_new_parameters)
			{
			}

			bc_render_system& m_render_system;
			graphic::bc_device& m_device;
			graphic::bc_device_swap_buffer& m_device_swap_buffer;
			const graphic::bc_device_parameters& m_old_parameters;
			const graphic::bc_device_parameters& m_new_parameters;
		};

		class bc_render_pass_config_change_context
		{
		public:
			bc_render_pass_config_change_context(bc_render_system& p_render_system,
				graphic::bc_device& p_device,
				graphic::bc_device_swap_buffer& p_device_swap_buffer,
				const bc_global_config& p_global_config)
				: m_render_system(p_render_system),
				m_device(p_device),
				m_device_swap_buffer(p_device_swap_buffer),
				m_global_config(p_global_config)
			{
			}

			bc_render_system& m_render_system;
			graphic::bc_device& m_device;
			graphic::bc_device_swap_buffer& m_device_swap_buffer;
			const bc_global_config& m_global_config;
		};

		/**
		 * \brief Represent a whole rendering pass that do all tasks that required to render a scene with a specified configuration 
		 */
		class bci_render_pass : public platform::bc_no_copy
		{
			friend class bc_render_pass_manager;
			
		public:
			bci_render_pass() noexcept;

			bci_render_pass(bci_render_pass&&) = default;

			virtual ~bci_render_pass() = default;

			bci_render_pass& operator=(bci_render_pass&&) = default;

			/**
			 * \brief This function will be called during app initialization
			 * \param p_render_system 
			 */
			virtual void initialize_resources(bc_render_system& p_render_system) = 0;

			/**
			 * \brief This function will be called during app update phase
			 * \param p_context 
			 */
			virtual void update(const bc_render_pass_update_context& p_context) = 0;

			/**
			 * \brief This function will be called in the start of frame draw phase.
			 * Threading: This function will be executed concurrently by a cpu worker thread.
			 * \param p_context
			 */
			virtual void initialize_frame(const bc_render_pass_render_context& p_context) = 0;

			/**
			 * \brief This function will be called in frame draw phase.
			 * Threading: This function will be executed concurrently by a cpu worker thread.
			 * \param p_context
			 */
			virtual void execute(const bc_render_pass_render_context& p_context) = 0;

			/**
			 * \brief This function will be called in the end of frame draw phase.
			 * \param p_context
			 */
			virtual void cleanup_frame(const bc_render_pass_render_context& p_context);

			/**
			 * \brief This function will be called when device duo to some parameter changes and buffer resize need reset
			 * \param p_context
			 */
			virtual void before_reset(const bc_render_pass_reset_context& p_context) = 0;

			/**
			 * \brief This function will be called when device duo to some parameter changes and buffer resize need reset
			 * \param p_context
			 */
			virtual void after_reset(const bc_render_pass_reset_context& p_context) = 0;

			/**
			 * \brief This function will be called when global config file changes and render pass parameters may need update
			 * \param p_context 
			 */
			virtual void config_changed(const bc_render_pass_config_change_context& p_context);

			/**
			 * \brief This function will be called when pass is going to be destroy.
			 * Before device destruction this function must be called.
			 * \param p_render_system 
			 */
			virtual void destroy(bc_render_system& p_render_system) = 0;

		protected:
			template<typename T>
			void share_resource(bc_render_pass_variable_t p_variable, T&& p_value);

			void unshare_resource(bc_render_pass_variable_t p_variable);

			template<typename T>
			T* get_shared_resource(bc_render_pass_variable_t p_variable) const noexcept;

			template<typename T>
			T& get_shared_resource_throw(bc_render_pass_variable_t p_variable) const;

			bc_intermediate_texture_guard get_intermediate_texture(const graphic::bc_texture_config& p_texture_config);

		private:
			void _set_private_fields(bc_render_pass_resource_manager& p_resource_manager, bc_intermediate_texture_manager& p_texture_manager);
			
			bc_render_pass_resource_manager* m_resource_manager;
			bc_intermediate_texture_manager* m_texture_manager;
		};

		inline bci_render_pass::bci_render_pass() noexcept
			: m_resource_manager(nullptr),
			m_texture_manager(nullptr)
		{
		}

		inline void bci_render_pass::cleanup_frame(const bc_render_pass_render_context& p_context)
		{
		}

		inline void bci_render_pass::config_changed(const bc_render_pass_config_change_context& p_context)
		{
		}

		template<typename T>
		void bci_render_pass::share_resource(bc_render_pass_variable_t p_variable, T&& p_value)
		{
			m_resource_manager->share_resource(p_variable, std::forward<T>(p_value));
		}

		inline void bci_render_pass::unshare_resource(bc_render_pass_variable_t p_variable)
		{
			m_resource_manager->unshare_resource(p_variable);
		}

		template<typename T>
		T* bci_render_pass::get_shared_resource(bc_render_pass_variable_t p_variable) const noexcept
		{
			return m_resource_manager->get_resource<T>(p_variable);
		}

		template<typename T>
		T& bci_render_pass::get_shared_resource_throw(bc_render_pass_variable_t p_variable) const
		{
			auto* l_resource = get_shared_resource<T>(p_variable);
			if(l_resource)
			{
				return *l_resource;
			}

			throw bc_key_not_found_exception("No shared resource were found in render passes with the given key");
		}

		inline bc_intermediate_texture_guard bci_render_pass::get_intermediate_texture(const graphic::bc_texture_config& p_texture_config)
		{
			return bc_intermediate_texture_guard(*m_texture_manager, p_texture_config);
		}

		inline void bci_render_pass::_set_private_fields(bc_render_pass_resource_manager& p_resource_manager, bc_intermediate_texture_manager& p_texture_manager)
		{
			m_resource_manager = &p_resource_manager;
			m_texture_manager = &p_texture_manager;
		}
	}
}