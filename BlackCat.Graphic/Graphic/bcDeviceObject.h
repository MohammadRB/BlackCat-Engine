//// [02/16/2016 MRB]
//
//#pragma once
//
//#include "CorePlatform/Utility/bcNoCopy.h"
//#include "Graphic/bcResourcePtr.h"
//
//namespace black_cat
//{
//	namespace graphic
//	{
//		class bc_device_object : 
//			private core::bc_ref_count,
//			public core_platform::bc_no_copy
//		{
//		public:
//			template< class T, class TDeleter >
//			friend class bc_ref_count_ptr;
//
//		public:
//			bc_device_object() = default;
//
//			bc_device_object(bc_device_object&& p_other) noexcept = default;
//
//			virtual ~bc_device_object() = default;
//
//			bc_device_object& operator=(bc_device_object&&) noexcept = default;
//
//		protected:
//
//		private:
//		};
//	}
//}