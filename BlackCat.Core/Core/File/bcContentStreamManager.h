// [05/09/2016 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/File/bcContent.h"
#include "Core/File/bcContentManager.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcDataDrivenParameter.h"
#include "Core/Utility/bcDelegate.h"

namespace black_cat
{
	namespace core
	{
		struct _bc_content_stream_file
		{
			bc_string_program m_title;
			bc_string_program m_name;
			bc_estring_program m_file;
			bc_data_driven_parameter m_parameters;
		};

		// Make bcContentManager data driven
		// Thread safe class
		class BC_CORE_DLL bc_content_stream_manager : public bc_iservice
		{
			BC_SERVICE(content_stream_manager)

		private:
			using string_hash = std::hash< const bcCHAR* >;
			using content_load_delegate = bc_delegate< bc_icontent_ptr(bc_alloc_type, const bcECHAR*, bc_content_loader_parameter&&) >;
			using content_types_map_type = bc_unordered_map_program< string_hash::result_type, content_load_delegate >;
			using streams_map_type = bc_unordered_map_program< string_hash::result_type, bc_vector_program< _bc_content_stream_file >>;
			using contents_map_type = bc_unordered_map_program< string_hash::result_type, bc_vector< bc_icontent_ptr > >;

		public:
			bc_content_stream_manager() noexcept(false);

			bc_content_stream_manager(bc_content_stream_manager&&) noexcept(
				std::is_nothrow_move_constructible< content_types_map_type >::value &&
				std::is_nothrow_move_constructible< streams_map_type >::value &&
				std::is_nothrow_move_constructible< contents_map_type >::value);

			~bc_content_stream_manager();

			bc_content_stream_manager& operator=(bc_content_stream_manager&&) noexcept(
				std::is_nothrow_move_assignable< content_types_map_type >::value &&
				std::is_nothrow_move_assignable< streams_map_type >::value &&
				std::is_nothrow_move_assignable< contents_map_type >::value);

			template< class TContent, class TLoader >
			void register_loader(bc_cloader_ptr< TLoader >&& p_loader);

			/**
			 * \brief Read content streams from a non-unicode json file
			 * \param p_json_file_path 
			 */
			void read_stream_file(const bcECHAR* p_json_file_path);

			/**
			 * \brief Load contents in the stream concurrent (TSFunc)
			 * \param p_alloc_type 
			 * \param p_stream_name 
			 */
			void load_content_stream(bc_alloc_type p_alloc_type, const bcCHAR* p_stream_name);

			/**
			 * \brief (TSFunc)
			 * \param p_stream_name 
			 */
			void unload_content_stream(const bcCHAR* p_stream_name);

			/**
			 * \brief Return Content with specified title otherwise return nullptr (TSFunc)
			 * \param p_content_name 
			 * \return 
			 */
			bc_icontent_ptr find_content(const bcCHAR* p_content_name) const;

			bc_icontent_ptr find_content_throw(const bcCHAR* p_content_name) const;

			/**
			 * \brief Return Content with specified title otherwise return nullptr (TSFunc)
			 * \tparam TContent 
			 * \param p_content_name 
			 * \return 
			 */
			template< class TContent >
			bc_content_ptr< TContent > find_content(const bcCHAR* p_content_name) const;

			template< class TContent >
			bc_content_ptr< TContent > find_content_throw(const bcCHAR* p_content_name) const;

		protected:

		private:
			template< class TContent >
			void _register_content_type();

			mutable core_platform::bc_shared_mutex m_contents_mutex;

			content_types_map_type m_content_types;
			streams_map_type m_streams;
			contents_map_type m_contents;
		};

		template< class TContent, class TLoader >
		void bc_content_stream_manager::register_loader(bc_cloader_ptr<TLoader>&& p_loader)
		{
			_register_content_type<TContent>();

			bc_content_manager* l_content_manager = bc_service_manager::get().get_service< bc_content_manager >();

			l_content_manager->register_loader<TContent, TLoader>(std::move(p_loader));
		}

		template< class TContent >
		void bc_content_stream_manager::_register_content_type()
		{
			auto* l_content_manager = bc_service_manager::get().get_service< bc_content_manager >();

			auto l_hash = string_hash()(bc_content_traits< TContent >::content_name());
			content_load_delegate l_load_delegate([l_content_manager](bc_alloc_type p_alloc_type, const bcECHAR* p_file_name, bc_content_loader_parameter&& p_parameters)
			{
				return l_content_manager->load< TContent >(p_alloc_type, p_file_name, std::move(p_parameters));
			});

			m_content_types.insert(content_types_map_type::value_type(l_hash, std::move(l_load_delegate)));
		}

		template< class TContent >
		bc_content_ptr<TContent> bc_content_stream_manager::find_content(const bcCHAR* p_content_name) const
		{
			static_assert(std::is_base_of< bc_icontent, TContent >::value, "TContent must be a content type");

			bc_icontent_ptr l_content = find_content(p_content_name);

			if (l_content != nullptr)
			{
				return static_cast< bc_content_ptr< TContent > >(l_content);
			}

			return nullptr;
		}

		template< class TContent >
		bc_content_ptr<TContent> bc_content_stream_manager::find_content_throw(const bcCHAR* p_content_name) const
		{
			static_assert(std::is_base_of< bc_icontent, TContent >::value, "TContent must be a content type");

			bc_icontent_ptr l_content = find_content_throw(p_content_name);

			if (l_content != nullptr)
			{
				return static_cast< bc_content_ptr< TContent > >(l_content);
			}

			return nullptr;
		}
	}
}