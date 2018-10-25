// [12/22/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"

#include "Graphic/bcDeviceRef.h"
#include "GraphicImp/bcExport.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api TApi>
		class bc_platform_device_command_list;

		template<bc_render_api TApi>
		class bc_platform_device_command_executer;

		template< bc_render_api TRenderApi >
		class bc_platform_device_compute_state;

		template< bc_render_api TRenderApi >
		class bc_platform_device_pipeline;

		template< bc_render_api TRenderApi >
		class bc_platform_device_pipeline_state;

		template<bc_render_api TApi>
		class bc_platform_buffer;

		template<bc_render_api TApi>
		class bc_platform_sampler_state;

		template<bc_render_api TApi>
		class bc_platform_texture2d;

		template<bc_render_api TApi>
		class bc_platform_depth_stencil_view;

		template<bc_render_api TApi>
		class bc_platform_render_target_view;

		template<bc_render_api TApi>
		class bc_platform_resource_view;

		template<bc_render_api TApi>
		class bc_platform_compiled_shader;

		template<bc_render_api TApi>
		class bc_platform_compute_shader;

		template<bc_render_api TApi>
		class bc_platform_domain_shader;

		template<bc_render_api TApi>
		class bc_platform_geometry_shader;

		template<bc_render_api TApi>
		class bc_platform_hull_shader;

		template<bc_render_api TApi>
		class bc_platform_pixel_shader;

		template<bc_render_api TApi>
		class bc_platform_vertex_shader;

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_device_command_list<g_api_dx11>& p_command_list);

		void BC_GRAPHICIMP_DLL _release(bc_platform_device_command_list<g_api_dx11>& p_command_list);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_device_command_executer<g_api_dx11>& p_command_executer);

		void BC_GRAPHICIMP_DLL _release(bc_platform_device_command_executer<g_api_dx11>& p_command_executer);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_device_compute_state<g_api_dx11>& p_compute_state);

		void BC_GRAPHICIMP_DLL _release(bc_platform_device_compute_state<g_api_dx11>& p_compute_state);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_device_pipeline<g_api_dx11>& p_pipeline);

		void BC_GRAPHICIMP_DLL _release(bc_platform_device_pipeline<g_api_dx11>& p_pipeline);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_device_pipeline_state<g_api_dx11>& p_pipeline_state);

		void BC_GRAPHICIMP_DLL _release(bc_platform_device_pipeline_state<g_api_dx11>& p_pipeline_state);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_buffer<g_api_dx11>& p_buffer);

		void BC_GRAPHICIMP_DLL _release(bc_platform_buffer<g_api_dx11>& p_buffer);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_sampler_state<g_api_dx11>& p_sampler_state);

		void BC_GRAPHICIMP_DLL _release(bc_platform_sampler_state<g_api_dx11>& p_sampler_state);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_texture2d<g_api_dx11>& p_texture2);

		void BC_GRAPHICIMP_DLL _release(bc_platform_texture2d<g_api_dx11>& p_texture2);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_depth_stencil_view<g_api_dx11>& p_depth_stencil);

		void BC_GRAPHICIMP_DLL _release(bc_platform_depth_stencil_view<g_api_dx11>& p_depth_stencil);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_render_target_view<g_api_dx11>& p_render_target);

		void BC_GRAPHICIMP_DLL _release(bc_platform_render_target_view<g_api_dx11>& p_render_target);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_resource_view<g_api_dx11>& p_resource_view);

		void BC_GRAPHICIMP_DLL _release(bc_platform_resource_view<g_api_dx11>& p_resource_view);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_compiled_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _release(bc_platform_compiled_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_compute_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _release(bc_platform_compute_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_domain_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _release(bc_platform_domain_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_geometry_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _release(bc_platform_geometry_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_hull_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _release(bc_platform_hull_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_pixel_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _release(bc_platform_pixel_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _add_ref(bc_platform_vertex_shader<g_api_dx11>& p_shader);

		void BC_GRAPHICIMP_DLL _release(bc_platform_vertex_shader<g_api_dx11>& p_shader);

		template< class TObject >
		struct bc_platform_device_ref_pack<g_api_dx11, TObject>
		{
			void add_ref()
			{
				_add_ref(m_object);
			}

			void release()
			{
				_release(m_object);
			}

			TObject m_object;
		};

		template< bc_render_api TApi, class TObject >
		bc_platform_device_ref<TApi, TObject>::bc_platform_device_ref()
		{
		}

		template< bc_render_api TApi, class TObject >
		template< typename >
		bc_platform_device_ref<TApi, TObject>::bc_platform_device_ref(const type& p_object)
		{
			reset(p_object);
		}

		template< bc_render_api TApi, class TObject >
		bc_platform_device_ref<TApi, TObject>::bc_platform_device_ref(const bc_platform_device_ref& p_other) noexcept	
		{
			operator=(p_other);
		}

		template< bc_render_api TApi, class TObject >
		bc_platform_device_ref<TApi, TObject>::bc_platform_device_ref(bc_platform_device_ref&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template< bc_render_api TApi, class TObject >
		bc_platform_device_ref<TApi, TObject>::~bc_platform_device_ref()
		{
			reset();
		}

		template< bc_render_api TApi, class TObject >
		bc_platform_device_ref<TApi, TObject>& bc_platform_device_ref<TApi, TObject>::operator=(const bc_platform_device_ref& p_other) noexcept
		{
			reset(p_other.m_pack.m_object);

			return *this;
		}

		template< bc_render_api TApi, class TObject >
		bc_platform_device_ref<TApi, TObject>& bc_platform_device_ref<TApi, TObject>::operator=(bc_platform_device_ref&& p_other) noexcept
		{
			if(m_pack.m_object.is_valid())
			{
				m_pack.release();
			}

			m_pack.m_object = p_other.m_pack.m_object;
			p_other.m_pack.m_object = type();

			return *this;
		}

		template< bc_render_api TApi, class TObject >
		typename bc_platform_device_ref<TApi, TObject>::type* bc_platform_device_ref<TApi, TObject>::operator->() noexcept
		{
			return &get();
		}

		template< bc_render_api TApi, class TObject >
		const typename bc_platform_device_ref<TApi, TObject>::type* bc_platform_device_ref<TApi, TObject>::operator->() const noexcept
		{
			return &get();
		}

		template< bc_render_api TApi, class TObject >
		typename bc_platform_device_ref<TApi, TObject>::type& bc_platform_device_ref<TApi, TObject>::operator*()
		{
			return get();
		}

		template< bc_render_api TApi, class TObject >
		const typename bc_platform_device_ref<TApi, TObject>::type& bc_platform_device_ref<TApi, TObject>::operator*() const
		{
			return get();
		}

		template< bc_render_api TApi, class TObject >
		typename bc_platform_device_ref<TApi, TObject>::type& bc_platform_device_ref<TApi, TObject>::get()
		{
			return m_pack.m_object;
		}

		template< bc_render_api TApi, class TObject >
		const typename bc_platform_device_ref<TApi, TObject>::type& bc_platform_device_ref<TApi, TObject>::get() const
		{
			return m_pack.m_object;
		}

		template< bc_render_api TApi, class TObject >
		void bc_platform_device_ref<TApi, TObject>::reset()
		{
			if (m_pack.m_object.is_valid())
			{
				m_pack.release();
				m_pack.m_object = type();
			}
		}

		template< bc_render_api TApi, class TObject >
		void bc_platform_device_ref<TApi, TObject>::reset(const type& p_object)
		{
			if (m_pack.m_object.is_valid())
			{
				m_pack.release();
			}

			m_pack.m_object = p_object;

			if(m_pack.m_object.is_valid())
			{
				m_pack.add_ref();
			}
		}

		template< bc_render_api TApi, class TObject >
		bool bc_platform_device_ref<TApi, TObject>::operator==(const bc_platform_device_ref& p_other) const noexcept
		{
			return m_pack.m_object == p_other.m_pack.m_object;
		}

		template< bc_render_api TApi, class TObject >
		bool bc_platform_device_ref<TApi, TObject>::operator!=(const bc_platform_device_ref& p_other) const noexcept
		{
			return m_pack.m_object != p_other.m_pack.m_object;
		}

		template< bc_render_api TApi, class TObject >
		bool bc_platform_device_ref<TApi, TObject>::operator==(std::nullptr_t) const noexcept
		{
			return m_pack.m_object == nullptr;
		}

		template< bc_render_api TApi, class TObject >
		bool bc_platform_device_ref<TApi, TObject>::operator!=(std::nullptr_t) const noexcept
		{
			return m_pack.m_object != nullptr;
		}
	}
}