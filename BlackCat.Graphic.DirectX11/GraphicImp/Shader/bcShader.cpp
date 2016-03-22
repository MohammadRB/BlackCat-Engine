// [01/27/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "Core/Container/bcAllocator.h"
#include "PlatformImp/bcUtility.h"
#include "GraphicImp/Shader/bcShader.h"

using namespace Microsoft::WRL;

namespace black_cat
{
	namespace graphic
	{
		ComPtr<ID3D10Blob> compile_shader(const bcECHAR* p_path,
			const bcCHAR* p_function_name,
			const bcCHAR* p_profile,
			const D3D_SHADER_MACRO* p_defines,
			ID3DInclude* p_includes)
		{
			// Loop until we succeed, or an exception is thrown
			while (true)
			{
				
				bcUINT l_flags = 0;
#if defined( BC_DEBUG )
				l_flags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION/*|D3D10_SHADER_WARNINGS_ARE_ERRORS*/;
#endif

				ComPtr<ID3D10Blob> l_compiled_shader;
				ComPtr<ID3D10Blob> l_error_messages;
				HRESULT l_hr = D3DCompileFromFile(p_path,
					p_defines,
					p_includes,
					p_function_name,
					p_profile,
					l_flags,
					0,
					l_compiled_shader.GetAddressOf(),
					l_error_messages.GetAddressOf());

				if (FAILED(l_hr))
				{
					DWORD l_error_code;
					platform::win32_from_hresult(l_hr, &l_error_code);
					core::bc_string l_full_message = "Error compiling shader file \"";

					if (l_error_messages)
					{
						bcCHAR l_message[3072];
						l_message[0] = NULL;
						bcCHAR* l_blobdata = reinterpret_cast<char*>(l_error_messages->GetBufferPointer());

						std::strcpy(l_message, l_blobdata);

						auto l_asci_path = core::bc_to_string<core::bc_allocator_frame>(core::bc_wstring_a<core::bc_allocator_frame>(p_path));
						l_full_message += l_asci_path.c_str();
						l_full_message += "\" - ";
						l_full_message += l_message;

						throw bc_graphic_exception(static_cast<bcINT>(l_error_code), l_full_message);
					}
					else
					{
						bcAssert(false);
						throw bc_graphic_exception(static_cast<bcINT>(l_error_code), l_full_message);
					}
				}

				return l_compiled_shader;
			}
		}

		template< >
		BC_GRAPHICIMP_DLL_EXP 
		bc_platform_ishader<bc_platform_render_api::directx11>::bc_platform_ishader()
			: m_pack()
		{
		}

		template< >
		BC_GRAPHICIMP_DLL_EXP 
		bc_platform_ishader< bc_platform_render_api::directx11 >::~bc_platform_ishader()
		{
		}

		template< >
		BC_GRAPHICIMP_DLL_EXP 
		bc_platform_ishader<bc_platform_render_api::directx11>::bc_platform_ishader(bc_platform_ishader&& p_other)
			: bc_device_object(std::move(p_other)),
			m_pack(std::move(p_other.m_pack))
		{
		}

		template< >
		BC_GRAPHICIMP_DLL_EXP 
		bc_platform_ishader<bc_platform_render_api::directx11>& bc_platform_ishader<bc_platform_render_api::directx11>::operator=(bc_platform_ishader&& p_other)
		{
			bc_device_object::operator=(std::move(p_other));
			m_pack = std::move(p_other.m_pack);

			return *this;
		}
	}
}