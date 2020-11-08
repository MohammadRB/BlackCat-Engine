// [01/22/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Graphic/bcEvent.h"
#include "Game/bcConstant.h"
#include "Game/System/Render/Pass/bcRenderPassResourceShare.h"
#include "Game/System/Input/bcCameraInstance.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_system;
		class bc_frame_renderer;
		class bc_render_thread;
		class bc_scene;

		template<typename TPass>
		struct bc_render_pass_trait
		{
		public:
			static constexpr const bcCHAR* render_pass_name()
			{
				return TPass::render_pass_name();
			}
			
			static constexpr bcUINT32 render_pass_hash()
			{
				return TPass::render_pass_hash();
			}
		};

		class bc_render_pass_update_param
		{
		public:
			bc_render_pass_update_param(const core_platform::bc_clock::update_param& p_clock,
				const bc_camera_instance& p_camera)
				: m_clock(p_clock),
				m_camera(p_camera)
			{
			}

			core_platform::bc_clock::update_param m_clock;
			bc_camera_instance m_camera;
		};

		class bc_render_pass_render_param
		{
		public:
			bc_render_pass_render_param(const core_platform::bc_clock::update_param& p_clock,
				const bc_camera_instance& p_current_camera,
				const bc_camera_instance& p_render_camera,
				bc_render_system& p_render_system,
				bc_frame_renderer& p_frame_renderer,
				bc_render_thread& p_render_thread)
				: m_clock(p_clock),
				m_current_camera(p_current_camera),
				m_render_camera(p_render_camera),
				m_render_system(p_render_system),
				m_frame_renderer(p_frame_renderer),
				m_render_thread(p_render_thread)
			{
			}

			core_platform::bc_clock::update_param m_clock;
			bc_camera_instance m_current_camera;
			bc_camera_instance m_render_camera;
			bc_render_system& m_render_system;
			bc_frame_renderer& m_frame_renderer;
			bc_render_thread& m_render_thread;
		};

		class bc_render_pass_reset_param
		{
		public:
			bc_render_pass_reset_param(bc_render_system& p_render_system,
				graphic::bc_device& p_device,
				graphic::bc_device_parameters& p_old_parameters,
				graphic::bc_device_parameters& p_new_parameters)
				: m_render_system(p_render_system),
				m_device(p_device),
				m_old_parameters(p_old_parameters),
				m_new_parameters(p_new_parameters)
			{
			}

			bc_render_system& m_render_system;
			graphic::bc_device& m_device;
			graphic::bc_device_parameters& m_old_parameters;
			graphic::bc_device_parameters& m_new_parameters;
		};

		/**
		 * \brief Represent a whole rendering pass that do all tasks that required to render a scene with a specified configuration 
		 */
		class bc_irender_pass : public core_platform::bc_no_copy
		{
		public:
			bc_irender_pass() = default;

			bc_irender_pass(bc_irender_pass&&) = default;

			virtual ~bc_irender_pass() = default;

			bc_irender_pass& operator=(bc_irender_pass&&) = default;

			/**
			 * \brief This function will be called during app initialization
			 * \param p_render_system 
			 */
			virtual void initialize_resources(bc_render_system& p_render_system) = 0;

			/**
			 * \brief This function will be called during app update phase
			 * \param p_param 
			 */
			virtual void update(const bc_render_pass_update_param& p_param) = 0;

			/**
			 * \brief This function will be called in the start of frame draw phase.
			 * Threading: This function will be executed concurrently by a cpu worker thread.
			 * \param p_param
			 */
			virtual void initialize_frame(const bc_render_pass_render_param& p_param) = 0;

			/**
			 * \brief This function will be called in frame draw phase.
			 * Threading: This function will be executed concurrently by a cpu worker thread.
			 * \param p_param
			 */
			virtual void execute(const bc_render_pass_render_param& p_param) = 0;

			/**
			 * \brief This function will be called in the end of frame draw phase.
			 * \param p_param
			 */
			virtual void cleanup_frame(const bc_render_pass_render_param& p_param);

			/**
			 * \brief This function will be called when device duo to some parameter changes and buffer resize need reset
			 * \param p_param
			 */
			virtual void before_reset(const bc_render_pass_reset_param& p_param) = 0;

			/**
			 * \brief This function will be called when device duo to some parameter changes and buffer resize need reset
			 * \param p_param
			 */
			virtual void after_reset(const bc_render_pass_reset_param& p_param) = 0;

			/**
			 * \brief This function will be called when pass is going to be destroy.
			 * Before device destruction this function must be called.
			 * \param p_render_system 
			 */
			virtual void destroy(bc_render_system& p_render_system) = 0;

			void _set_pass_resource_share(bc_render_pass_resource_share* p_state_share);

		protected:
			template< typename T >
			void share_resource(constant::bc_render_pass_variable_t p_variable, T&& p_value);

			void unshare_resource(constant::bc_render_pass_variable_t p_variable);

			template< typename T >
			T* get_shared_resource(constant::bc_render_pass_variable_t p_variable) const noexcept;

			template< typename T >
			T& get_shared_resource_throw(constant::bc_render_pass_variable_t p_variable) const;

		private:
			bc_render_pass_resource_share* m_resource_share;
		};

		inline void bc_irender_pass::cleanup_frame(const bc_render_pass_render_param& p_param)
		{
		}

		inline void bc_irender_pass::_set_pass_resource_share(bc_render_pass_resource_share* p_state_share)
		{
			m_resource_share = p_state_share;
		}

		template< typename T >
		void bc_irender_pass::share_resource(constant::bc_render_pass_variable_t p_variable, T&& p_value)
		{
			m_resource_share->share_resource(p_variable, std::forward<T>(p_value));
		}

		inline void bc_irender_pass::unshare_resource(constant::bc_render_pass_variable_t p_variable)
		{
			m_resource_share->unshare_resource(p_variable);
		}

		template< typename T >
		T* bc_irender_pass::get_shared_resource(constant::bc_render_pass_variable_t p_variable) const noexcept
		{
			return m_resource_share->get_resource< T >(p_variable);
		}

		template< typename T >
		T& bc_irender_pass::get_shared_resource_throw(constant::bc_render_pass_variable_t p_variable) const
		{
			auto* l_resource = get_shared_resource<T>(p_variable);
			if(l_resource)
			{
				return *l_resource;
			}

			throw bc_key_not_found_exception("No shared resource were found in render passes with the given key");
		}
	}
}