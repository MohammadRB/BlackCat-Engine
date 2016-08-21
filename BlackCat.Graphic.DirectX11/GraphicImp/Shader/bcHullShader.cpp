// [01/27/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Shader/bcShader.h"
#include "GraphicImp/Shader/bcHullShader.h"

using namespace Microsoft::WRL;

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_hull_shader< g_api_dx11 >::bc_platform_hull_shader()
			: m_pack()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_hull_shader< g_api_dx11 >::~bc_platform_hull_shader()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_hull_shader< g_api_dx11 >::bc_platform_hull_shader(bc_platform_hull_shader&& p_other)
			: bc_ishader(std::move(p_other)),
			m_pack(std::move(p_other.m_pack))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_hull_shader< g_api_dx11 >& bc_platform_hull_shader< g_api_dx11 >::operator=(bc_platform_hull_shader&& p_other)
		{
			bc_ishader::operator=(std::move(p_other));
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_shader_type bc_platform_hull_shader< bc_platform_render_api::directx11 >::get_type() const
		{
			return bc_shader_type::hull;
		};
	}
}
