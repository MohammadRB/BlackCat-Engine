// [12/31/2015 MRB]

#pragma once

#include "CorePlatform/bcCorePlatformUtility.h"
#include "Core/bcCoreUtility.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcVector.h"
#include "Core/Event/bcEvent.h"
#include "Platform/bcPlatformEvents.h"
#include "PlatformImp/Application/bcRenderWindow.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcDeviceObject.h"
#include "Graphic/Device/bcDeviceInfo.h"
#include "Graphic/Resource/bcResource.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_platform_render_api>
		class bc_platform_buffer;
		using bc_buffer = bc_platform_buffer< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_buffer_config;
		using bc_buffer_config = bc_platform_buffer_config< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_texture_config;
		using bc_texture_config = bc_platform_texture_config< g_current_platform_render_api >;

		class bc_subresource_data;

		template<bc_platform_render_api>
		class bc_platform_texture2d;
		using bc_texture2d = bc_platform_texture2d< g_current_platform_render_api >;

		struct bc_sampler_state_config;

		template<bc_platform_render_api>
		class bc_platform_sampler_state;
		using bc_sampler_state = bc_platform_sampler_state< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_vertex_shader;
		using bc_vertex_shader = bc_platform_vertex_shader< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_hull_shader;
		using bc_hull_shader = bc_platform_hull_shader< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_domain_shader;
		using bc_domain_shader = bc_platform_domain_shader< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_geometry_shader;
		using bc_geometry_shader = bc_platform_geometry_shader< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_pixel_shader;
		using bc_pixel_shader = bc_platform_pixel_shader< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_compute_shader;
		using bc_compute_shader = bc_platform_compute_shader< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_device_pipeline;
		using bc_device_pipeline = bc_platform_device_pipeline< g_current_platform_render_api >;

		struct bc_device_pipeline_state_config;

		template<bc_platform_render_api>
		class bc_platform_device_pipeline_state;
		using bc_device_pipeline_state = bc_platform_device_pipeline_state< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_device_command_list;
		using bc_device_command_list = bc_platform_device_command_list< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_device_command_executer;
		using bc_device_command_executer = bc_platform_device_command_executer< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_resource_view_config;
		using bc_resource_view_config = bc_platform_resource_view_config< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_render_target_view_config;
		using bc_render_target_view_config = bc_platform_render_target_view_config< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_depth_stencil_view_config;
		using bc_depth_stencil_view_config = bc_platform_depth_stencil_view_config< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_iresource_view;
		using bc_iresource_view = bc_platform_iresource_view< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_shader_view;
		using bc_shader_view = bc_platform_shader_view< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_depth_stencil_view;
		using bc_depth_stencil_view = bc_platform_depth_stencil_view< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_render_target_view;
		using bc_render_target_view = bc_platform_render_target_view< g_current_platform_render_api >;

		template<class>
		class bc_resource_ptr;

		struct bc_device_parameters
		{
		public:
			bc_device_parameters(bcUINT p_width, bcUINT p_height, bc_format p_format, bc_texture_ms_config p_multi_sample)
				: m_width(p_width),
				m_height(p_height),
				m_format(p_format),
				m_multi_sample(m_multi_sample)
			{
			}

			bcUINT m_width;
			bcUINT m_height;
			bc_format m_format;
			bc_texture_ms_config m_multi_sample;
		};

		class bc_idevice_listener;

		template< bc_platform_render_api TRenderApi >
		struct bc_platform_device_pack
		{
		};

		// TODO complete device parameters
		template< bc_platform_render_api TRenderApi >
		class bc_platform_device 
			: public core::bc_initializable<bcUINT, bcUINT, bc_format, platform::bc_render_window&>,
			public core::bc_object_allocator,
			public core_platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_device_pack<TRenderApi>;

		public:
			bc_platform_device();

			bc_platform_device(bc_platform_device&& p_other);

			~bc_platform_device();

			bc_platform_device& operator=(bc_platform_device&& p_other);

			bool get_vsync() const;

			void set_vsync(bool p_sync);

			bool get_full_screen() const;

			void set_full_screen(bool p_full_screen);

			bcUINT32 get_back_buffer_width() const;

			bcUINT32 get_back_buffer_height() const;

			bc_format get_back_buffer_format() const;

			bc_texture2d* get_back_buffer_texture() const;

			bc_device_info get_device_info() const;

			bcUINT check_multi_sampling(bc_format p_textue_format, bcUINT p_sample_count) const;

			bc_resource_ptr<bc_buffer> create_buffer(bc_buffer_config* p_config, bc_subresource_data* p_data);

			bc_resource_ptr<bc_texture2d> create_texture2d(bc_texture_config* p_config, bc_subresource_data* p_data);

			bc_resource_ptr<bc_texture2d> create_texture2d(bc_texture_config* p_config, const bcECHAR* p_filename);

			bc_resource_ptr<bc_sampler_state> create_sampler_state(bc_sampler_state_config* p_config);

			bc_resource_ptr<bc_vertex_shader> create_vertex_shader(const bcECHAR* p_filepath, const bcCHAR* p_function);

			bc_resource_ptr<bc_hull_shader> create_hull_shader(const bcECHAR* p_filepath, const bcCHAR* p_function);

			bc_resource_ptr<bc_domain_shader> create_domain_shader(const bcECHAR* p_filepath, const bcCHAR* p_function);

			bc_resource_ptr<bc_geometry_shader> create_geometry_shader(const bcECHAR* p_filepath, const bcCHAR* p_function);

			bc_resource_ptr<bc_pixel_shader> create_pixel_shader(const bcECHAR* p_filepath, const bcCHAR* p_function);

			bc_resource_ptr<bc_compute_shader> create_compute_shader(const bcECHAR* p_filepath, const bcCHAR* p_function);

			bc_resource_ptr<bc_shader_view> create_shader_view(bc_iresource* p_resource, bc_resource_view_config* p_view_config);

			bc_resource_ptr<bc_depth_stencil_view> create_depth_stencil_view(bc_iresource* p_resource, bc_depth_stencil_view_config* p_view_config);

			bc_resource_ptr<bc_render_target_view> create_render_target_view(bc_iresource* p_resource, bc_render_target_view_config* p_view_config);

			bc_resource_ptr<bc_device_pipeline_state> create_pipeline_state(bc_device_pipeline_state_config* p_config);

			bc_resource_ptr<bc_device_pipeline> create_pipeline();
			
			bc_resource_ptr<bc_device_command_list> create_command_list();

			bc_resource_ptr<bc_device_command_executer> create_command_executer();

			// Resize back buffer and send event to resize all other resizable resources. 
			// If device is in fullscreen mode change resulotion too.
			void resize_back_buffer(bcUINT p_width, bcUINT p_height);

			void resize_back_buffer(bcUINT p_width, bcUINT p_height, bc_format p_format);

			void resize_texture2d(bc_resource_ptr<bc_texture2d> p_texture, 
				bcUINT p_width, 
				bcUINT p_height, 
				bcUINT p_num_views,
				bc_resource_ptr<bc_iresource_view> p_views[]);

			void present();
			
			void register_listender(bc_idevice_listener* p_listener);

			void unregister_listener(bc_idevice_listener* p_listener);

			void destroy_resource(bc_device_object* p_resource);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			void _initialize(bcUINT p_width, bcUINT p_height, bc_format p_back_buffer_format, platform::bc_render_window& p_output_window) override;

			void _destroy() override;

			template< class TResource >
			void _store_new_resource(core::bc_object_allocator::ptr<TResource>&& p_resource);

			bool _event_handler(core::bc_ievent& p_event);

			// Use non-movale allocator, because we use pointer to internall objects of vector in resource pointers 
			core::bc_vector< core::bc_object_allocator::ptr<bc_device_object> > m_resources;
			core::bc_vector< bc_idevice_listener* > m_listeners;

			platform_pack m_pack;
		};

		using bc_device = bc_platform_device< g_current_platform_render_api >;

		// Interface for device related events
		class bc_idevice_listener
		{
		public:
			virtual ~bc_idevice_listener() = default;

			// Eligible function for every cleanup action that must take place before reseting device for 
			// exmaple if device back buffer was resized all resourced must be released before reseting
			virtual void before_reset(bc_device* p_device, bc_device_parameters& p_old_parameters, bc_device_parameters& p_new_parameters) = 0;

			virtual void after_reset(bc_device* p_device, bc_device_parameters& p_old_parameters, bc_device_parameters& p_new_parameters) = 0;
		};

		template< bc_platform_render_api TRenderApi >
		bcUINT32 bc_platform_device<TRenderApi>::get_back_buffer_width() const
		{
			return get_back_buffer_texture()->get_width();
		}

		template< bc_platform_render_api TRenderApi >
		bcUINT32 bc_platform_device<TRenderApi>::get_back_buffer_height() const
		{
			return get_back_buffer_texture()->get_height();
		}

		template< bc_platform_render_api TRenderApi >
		bc_format bc_platform_device<TRenderApi>::get_back_buffer_format() const
		{
			return get_back_buffer_texture()->get_format();
		}

		template< bc_platform_render_api TRenderApi >
		void bc_platform_device<TRenderApi>::register_listender(bc_idevice_listener* p_listener)
		{
			m_listeners.push_back(p_listener);
		}

		template< bc_platform_render_api TRenderApi >
		void bc_platform_device<TRenderApi>::unregister_listener(bc_idevice_listener* p_listener)
		{
			auto l_listener = std::find(std::begin(m_listeners), std::end(m_listeners), p_listener);

			if (l_listener != std::end(m_listeners))
				m_listeners.erase(l_listener);
		}

		template <bc_platform_render_api TRenderApi >
		void bc_platform_device<TRenderApi>::destroy_resource(bc_device_object* p_resource)
		{
			auto l_item = std::find_if(std::begin(m_resources), std::end(m_resources), [=](ptr<bc_device_object>& p_item)
			{
				
				return p_item.get() == p_resource;

			});

			if (l_item == std::end(m_resources))
			{
				bcAssert(false, "Resource not found");
				return;
			}

			// We set pointer to null because we can use null pointers again 
			(*l_item).reset(nullptr);
		}

		template <bc_platform_render_api TRenderApi >
		template < class TResource >
		void bc_platform_device<TRenderApi>::_store_new_resource(core::bc_object_allocator::ptr<TResource>&& p_resource)
		{
			// Get parameter as template and convert ptr<> to ptr< bc_device_object > manually, because if we let implicit 
			// conversation do the work, bc_delegate< void(*) > will be converted to bc_delegate< void(bc_device_object*) >
			// and this conversation cause problem in bc_delegate class(this conversation will call template ctor in bc_delegate
			// class and produce a bc_delegate that hold another bc_delegate< void(bc_device_object*) >).
			auto l_pointer = core::bc_object_allocator::ptr< bc_device_object >
				(
					p_resource.release(),
					core::bc_delegate< void(bc_device_object*) >(this, &bc_platform_device::deallocate)
				);
			auto l_first_empty = std::find(std::begin(m_resources), std::end(m_resources), ptr<bc_device_object>(nullptr));

			if (l_first_empty != std::end(m_resources))
			{
				(*l_first_empty).swap(l_pointer);
			}
			else
			{
				m_resources.push_back(std::move(l_pointer));
			}
		}

		template <bc_platform_render_api TRenderApi >
		bool bc_platform_device<TRenderApi>::_event_handler(core::bc_ievent& p_event)
		{
			bool l_is_resize = p_event.get_event_hash() == core::bc_ievent::get_hash(platform::bc_app_event_window_resize::event_name());

			if (!l_is_resize)
				return false;

			platform::bc_app_event_window_resize& l_resize_event = static_cast< platform::bc_app_event_window_resize& >(p_event);
			
			resize_back_buffer(l_resize_event.width(), l_resize_event.height());

			return true;
		}
	}
}
