// [05/09/2016 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Content/bcContent.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcDataDrivenParameter.h"
#include "Core/Utility/bcDelegate.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_task;

		class bc_content_stream_manager;

		template< class TContent, class TLoader >
		void bc_register_loader(const bcCHAR* p_data_driven_name, bc_cloader_ptr< TLoader >&& p_loader)
		{
			core::bc_get_service< bc_content_stream_manager >()->register_loader< TContent, TLoader >(p_data_driven_name, std::move(p_loader));
		}

		struct _bc_content_stream_file
		{
			bc_string_program m_title;
			bc_string_program m_name;
			bc_estring_program m_file;
			bc_data_driven_parameter m_parameters;
		};

		/**
		 * \brief Make bcContentManager data driven.
		 * ThreadSafe
		 */
		class BC_CORE_DLL bc_content_stream_manager : public bc_iservice
		{
			BC_SERVICE(c_s_mng)

		private:
			using string_hash = std::hash< const bcCHAR* >;
			using content_load_delegate = bc_delegate< bc_icontent_ptr(bc_alloc_type, const bcECHAR*, bc_content_loader_parameter&&) >;
			using content_loader_map_type = bc_unordered_map_program< string_hash::result_type, content_load_delegate >;
			using content_stream_map_type = bc_unordered_map_program< string_hash::result_type, bc_vector_program< _bc_content_stream_file >>;
			using content_map_type = bc_unordered_map_program< string_hash::result_type, bc_vector< bc_icontent_ptr > >;

		public:
			bc_content_stream_manager(bc_content_manager& p_content_manager) noexcept;

			bc_content_stream_manager(bc_content_stream_manager&&) noexcept = delete;

			~bc_content_stream_manager();

			bc_content_stream_manager& operator=(bc_content_stream_manager&&) noexcept = delete;

			bc_content_manager& get_content_manager() const noexcept
			{
				return m_content_manager;
			}

			template< class TContent, class TLoader >
			void register_loader(const bcCHAR* p_data_driven_name, bc_cloader_ptr< TLoader >&& p_loader);

			/**
			 * \brief Read content streams from a non-unicode json file
			 * \param p_json_file_path 
			 */
			void read_stream_file(const bcECHAR* p_json_file_path);

			/**
			 * \brief Load contents in the stream concurrent
			 * \ThreadSafe
			 * \param p_alloc_type 
			 * \param p_stream_name 
			 */
			void load_content_stream(bc_alloc_type p_alloc_type, const bcCHAR* p_stream_name);

			bc_task<void> load_content_stream_async(bc_alloc_type p_alloc_type, const bcCHAR* p_stream_name);

			/**
			 * \brief 
			 * \ThreadSafe
			 * \param p_stream_name 
			 */
			void unload_content_stream(const bcCHAR* p_stream_name);

			/**
			 * \brief Return Content with specified title otherwise return nullptr
			 * \ThreadSafe
			 * \param p_content_name 
			 * \return 
			 */
			bc_icontent_ptr find_content(const bcCHAR* p_content_name) const;

			bc_icontent_ptr find_content_throw(const bcCHAR* p_content_name) const;

			/**
			 * \brief Return Content with specified title otherwise return nullptr
			 * \ThreadSafe
			 * \tparam TContent 
			 * \param p_content_name 
			 * \return 
			 */
			template< class TContent >
			bc_content_ptr< TContent > find_content(const bcCHAR* p_content_name) const;

			template< class TContent >
			bc_content_ptr< TContent > find_content_throw(const bcCHAR* p_content_name) const;

		private:
			template< class TContent >
			void _register_content_loader(const bcCHAR* p_data_driven_name);

			bc_content_manager& m_content_manager;

			content_loader_map_type m_content_loader_delegates;
			content_stream_map_type m_streams;
			mutable core_platform::bc_shared_mutex m_contents_mutex;
			content_map_type m_contents;
		};

		template< class TContent, class TLoader >
		void bc_content_stream_manager::register_loader(const bcCHAR* p_data_driven_name, bc_cloader_ptr<TLoader>&& p_loader)
		{
			_register_content_loader<TContent>(p_data_driven_name);

			m_content_manager.register_loader<TContent, TLoader>(std::move(p_loader));
		}

		template< class TContent >
		void bc_content_stream_manager::_register_content_loader(const bcCHAR* p_data_driven_name)
		{
			auto l_data_driven_hash = string_hash()(p_data_driven_name);
			content_load_delegate l_load_delegate([this](bc_alloc_type p_alloc_type, const bcECHAR* p_file_name, bc_content_loader_parameter&& p_parameters)
			{
				return m_content_manager.load< TContent >(p_alloc_type, p_file_name, std::move(p_parameters));
			});

			m_content_loader_delegates.insert(content_loader_map_type::value_type(l_data_driven_hash, std::move(l_load_delegate)));
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