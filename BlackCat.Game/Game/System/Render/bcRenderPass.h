// [01/22/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Game/System/Render/bcRenderThread.h"
#include "Game/System/Render/bcRenderPassResourceShare.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_system;
		struct bc_render_system_update_param;

		// Represent a whole rendering pass that do all tasks that required to render a scene with
		// a specified configuration 
		class bc_irender_pass : public core_platform::bc_no_copy
		{
		public:
			bc_irender_pass() = default;

			virtual ~bc_irender_pass() = default;

			// This function will be called during app initialization
			virtual void initialize_resources(bc_render_system& p_render_system, graphic::bc_device* p_device) = 0;

			// This function will be called during app update phase
			virtual void update(const bc_render_system_update_param& p_update_param) = 0;

			// This function will be called in start of frame
			// (Default implementation call set_render_parameters function)
			virtual void initialize_frame(bc_render_system& p_render_system, bc_render_thread& p_thread) = 0;

			// This function will be called in draw frme phase
			virtual void execute(bc_render_system& p_render_system, bc_render_thread& p_thread) = 0;

			// This function will be called when device duo to some parameter changes and buffer resize need reset
			virtual void before_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) = 0;

			// This function will be called when device duo to some parameter changes and buffer resize need reset
			virtual void after_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) = 0;

			// This function will be called when pass is going to be destroy
			// (before device destroying this function must be called) 
			virtual void destroy(graphic::bc_device* p_device) = 0;

			virtual core::bc_string get_name() = 0;

			void _set_pass_resource_share(bc_render_pass_resource_share* p_state_share);

		protected:
			template< typename T >
			void share_resource(bc_render_pass_resource_variable p_variable, T&& p_value);

			void unshare_resource(bc_render_pass_resource_variable p_variable);

			template< typename T >
			T* get_shared_resource(bc_render_pass_resource_variable p_variable) const noexcept;

		private:
			bc_render_pass_resource_share* m_resource_share;
		};

		inline void bc_irender_pass::_set_pass_resource_share(bc_render_pass_resource_share* p_state_share)
		{
			m_resource_share = p_state_share;
		}

		template< typename T >
		void bc_irender_pass::share_resource(bc_render_pass_resource_variable p_variable, T&& p_value)
		{
			m_resource_share->share_resource(p_variable, std::forward<T>(p_value));
		}

		inline void bc_irender_pass::unshare_resource(bc_render_pass_resource_variable p_variable)
		{
			m_resource_share->unshare_resource(p_variable);
		}

		template< typename T >
		T* bc_irender_pass::get_shared_resource(bc_render_pass_resource_variable p_variable) const noexcept
		{
			return m_resource_share->get_resource< T >(p_variable);
		}
	}
}