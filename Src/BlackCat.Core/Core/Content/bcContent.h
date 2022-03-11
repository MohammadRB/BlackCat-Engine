// [03/28/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"
#include "Core/Utility/bcRefCountPtr.h"

namespace black_cat
{
	namespace core
	{
		class bci_content;
		class bc_content_manager;

		class BC_CORE_DLL _bc_content_ptr_deleter
		{
		public:
			_bc_content_ptr_deleter();

			_bc_content_ptr_deleter(bc_content_manager* p_content_manager);

			void operator ()(bci_content* p_content) const;

		private:
			bc_content_manager* m_content_manager;
		};

		template<typename TContent>
		using bc_content_ptr = bc_ref_count_ptr<TContent, _bc_content_ptr_deleter>;

		template< class TContent >
		class bc_content_traits
		{
		public:
			static constexpr const bcCHAR* content_name()
			{
				return TContent::content_name();
			}

			static constexpr bc_string_cmp_hash content_hash()
			{
				return TContent::content_hash();
			}
		};

		class bci_content : public bc_ref_count, public platform::bc_no_copy
		{
		public:
			friend class bc_content_manager;

		public:
			virtual ~bci_content() = default;

		protected:
			bci_content() = default;

			bci_content(bci_content&&) = default;

			bci_content& operator=(bci_content&&) = default;

		private:
			bc_estring_view _get_key() const
			{
				return m_key;
			}

			void _set_key(bc_estring_view p_key)
			{
				m_key = p_key;
			}

			bc_estring_view m_key{};
		};

		using bc_icontent_ptr = bc_content_ptr<bci_content>;
	}
}