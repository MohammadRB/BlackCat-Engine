// [10/10/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
#include "Platform/Script/bcScriptRef.h"
#include "Platform/Script/bcScriptContext.h"
#include "Platform/Script/bcScriptVariable.h"
#include "Platform/Script/bcScriptPrototype.h"

namespace black_cat
{
	namespace platform
	{
		template< core_platform::bc_platform TPlatform >
		struct bc_platform_script_object_pack
		{
		};

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_object : public bc_platform_script_reference< TPlatform >
		{
		public:
			using platform_pack = bc_platform_script_object_pack< TPlatform >;
			friend bc_script_context;

		public:
			bc_platform_script_object();

			bc_platform_script_object(const bc_platform_script_object&) noexcept;

			~bc_platform_script_object();

			bc_platform_script_object& operator=(const bc_platform_script_object&) noexcept;

			/*void property(const bcCHAR* p_name, bc_script_getter_delegate< void*, bool > p_bool_read, bc_script_setter_delegate< void*, bool > p_bool_write);

			void property(const bcCHAR* p_name, bc_script_getter_delegate< void*, bcINT > p_int_read, bc_script_setter_delegate< void*, bcINT > p_int_write);

			void property(const bcCHAR* p_name, bc_script_getter_delegate< void*, bcDOUBLE > p_double_read, bc_script_setter_delegate< void*, bcDOUBLE > p_double_write);

			void property(const bcCHAR* p_name, bc_script_getter_delegate< void*, core::bc_string > p_string_read, bc_script_setter_delegate< void*, core::bc_string > p_string_write);

			void property(const bcCHAR* p_name, bc_script_getter_delegate< void*, bc_script_object > p_object_read, bc_script_setter_delegate< void*, bc_script_object > p_object_write);

			template< typename TR, typename ...TA >
			void function(const bcCHAR* p_name, bc_script_function< TR(void*, TA...) > p_function);*/

			bool has_index(bcUINT32 p_index) const;

			bc_script_variable get_index(bcUINT32 p_index) const;

			bool has_property(const bcWCHAR* p_property) const;

			bc_script_variable get_property(const bcWCHAR* p_property) const;

			core::bc_vector_frame< core::bc_wstring > get_own_property_names() const;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			template< typename T >
			bc_platform_script_object(bc_script_context& p_context, bc_script_prototype<T>& p_prototype, T&& p_native_object) noexcept;

			platform_pack m_pack;
		};

		using bc_script_object = bc_platform_script_object< core_platform::g_current_platform >;
		using bc_script_object_ref = bc_script_ref< bc_script_object >;
	}
}