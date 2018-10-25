//// [02/16/2016 MRB]
//
//#pragma once
//
//#include "Core/Utility/bcRefCountPtr.h"
//#include "Graphic/bcExport.h"
//#include "Graphic/bcRenderApi.h"
//
//namespace black_cat
//{
//	namespace graphic
//	{
//		template<bc_render_api>
//		class bc_platform_device;
//		using bc_device = bc_platform_device< g_current_render_api >;
//
//		class bc_device_object;
//
//		class BC_GRAPHIC_DLL _bc_resource_ptr_deleter
//		{
//		public:
//			_bc_resource_ptr_deleter();
//
//			_bc_resource_ptr_deleter(bc_device* p_device);
//
//			void operator ()(bc_device_object* p_resource) const;
//
//		protected:
//
//		private:
//			bc_device* m_device;
//		};
//
//		template< typename TResource >
//		using bc_resource_ptr = core::bc_ref_count_ptr< TResource, _bc_resource_ptr_deleter >;
//	}
//}