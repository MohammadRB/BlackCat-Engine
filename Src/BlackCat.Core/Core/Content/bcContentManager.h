// [03/28/2016 MRB]

#pragma once

#include "CorePlatformImp/File/bcFileInfo.h"
#include "CorePlatformImp/File/bcFile.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/bcConstant.h"
#include "Core/bcException.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/File/bcPath.h"
#include "Core/File/bcStream.h"
#include "Core/File/bcFileStream.h"
#include "Core/Content/bcContent.h"
#include "Core/Content/bcContentLoader.h"
#include "Core/bcException.h"

namespace black_cat
{
	namespace core
	{
		template< typename TLoader >
		using bc_cloader_ptr = bc_service_ptr< TLoader >;

		template< typename TLoader, typename ...TArgs >
		bc_cloader_ptr< TLoader > bc_make_loader(TArgs&&... p_args)
		{
			return core::bc_make_unique< TLoader >(bc_alloc_type::program, std::forward< TArgs >(p_args)...);
		}

		template< class TContent >
		class _content_wrapper : public bc_iservice
		{
		public:
			static constexpr const bcCHAR* service_name()
			{
				return bc_content_traits< TContent >::content_name();
			}

			static constexpr bcUINT32 service_hash()
			{
				return bc_content_traits< TContent >::content_hash();
			}
		};

		// Because in service manager registered instance must be a derived type of registered type
		// we make loader derived type of content type through a wrapper class
		template< class TContent >
		class _loader_wrapper : public _content_wrapper< TContent >
		{
		public:
			_loader_wrapper(bc_cloader_ptr< bc_icontent_loader > p_loader)
				: m_loader(std::move(p_loader))
			{
			}

			bc_cloader_ptr< bc_icontent_loader > m_loader;
		};

		class _bc_content_entry
		{
		public:
			_bc_content_entry() = default;

			_bc_content_entry(const bcECHAR* p_file_path, bc_unique_ptr< bc_icontent >&& p_content)
				: m_file_path(p_file_path),
				m_content(std::move(p_content))
			{	
			}

			bc_wstring m_file_path;
			bc_unique_ptr< bc_icontent > m_content;
		};

		/**
		 * \brief 
		 * \ThreadSafe
		 */
		class bc_content_manager : public bc_iservice, protected bc_object_allocator
		{
			BC_SERVICE(content_manager)

		private:
			using string_hash = std::hash< const bcCHAR* >;
			using map_type = bc_unordered_map< bc_content_hash_t::result_type, _bc_content_entry >;

		public:
			bc_content_manager();

			bc_content_manager(bc_content_manager&&) noexcept = delete;

			~bc_content_manager();

			bc_content_manager& operator=(bc_content_manager&&) noexcept = delete;

			template< class TContent, class TLoader >
			void register_loader(bc_cloader_ptr< TLoader >&& p_loader);

			/**
			 * \brief Load specified content from file or if it's loaded earlier return a pointer to it.
			 * file name must be absolute path
			 * \tparam TContent 
			 * \param p_file 
			 * \param p_parameter 
			 * \return 
			 */
			template< class TContent >
			bc_content_ptr< TContent > load(const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter);

			/**
			 * \brief Load specified content from file or if it's loaded earlier return a pointer to it.
			 * file name must be absolute path
			 * \tparam TContent 
			 * \param p_alloc_type 
			 * \param p_file 
			 * \param p_parameter 
			 * \return 
			 */
			template< class TContent >
			bc_content_ptr< TContent > load(bc_alloc_type p_alloc_type, const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter);

			template< class TContent >
			bc_task< bc_content_ptr< TContent > > load_async(const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter);

			template< class TContent >
			bc_task< bc_content_ptr< TContent > > load_async(bc_alloc_type p_alloc_type, const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter);
						
			template< class TContent >
			void save(TContent& p_content);

			template< class TContent >
			bc_task<void> save_async(TContent& p_content);

			/**
			 * \brief Import content to manager so a content ptr can be constructed from it.
			 * \tparam TContent 
			 * \param p_content_name 
			 * \param p_content 
			 * \return 
			 */
			template< class TContent >
			bc_content_ptr<TContent> store_content(const bcCHAR* p_content_name, TContent&& p_content);

			/**
			* \brief Import content to manager so a content ptr can be constructed from it.
			* \tparam TContent
			* \param p_alloc_type
			* \param p_content_name
			* \param p_content
			* \return
			*/
			template< class TContent >
			bc_content_ptr<TContent> store_content(bc_alloc_type p_alloc_type, const bcCHAR* p_content_name, TContent&& p_content);

			void destroy_content(bc_icontent* p_content);

		protected:

		private:
			template< class TContent >
			bc_icontent_loader* _get_loader();

			template< class TContent >
			bc_estring_frame _get_offline_file_path(const bcECHAR* p_file);

			template< class TContent >
			bc_unique_ptr< TContent > _load_content(bc_alloc_type p_alloc_type,
				const bcECHAR* p_file,
				const bcECHAR* p_offline_file,
				bc_content_loader_parameter&& p_parameter,
				bc_icontent_loader* p_loader);

			template< class TContent >
			bc_content_ptr<TContent> _store_new_content(bc_content_hash_t::result_type p_content_hash, 
				const bcECHAR* p_content_file, 
				bc_unique_ptr< TContent > p_content);

			map_type m_contents;
			core_platform::bc_shared_mutex m_contents_mutex;
		};

		inline bc_content_manager::bc_content_manager() = default;

		inline bc_content_manager::~bc_content_manager() = default;

		inline void bc_content_manager::destroy_content(bc_icontent* p_content)
		{
			map_type::value_type::second_type l_value;

			{
				core_platform::bc_lock_guard< core_platform::bc_shared_mutex > m_guard(m_contents_mutex);
				{
					auto l_item = m_contents.find(p_content->_get_hash());

					if (l_item == end(m_contents))
					{
						bcAssert(false, "Content not found");
						return;
					}

					// Move value from container because some times content have inner content and destroying
					// content while lock is acquired cause deadlock
					l_value = std::move(l_item->second);
					m_contents.erase(l_item);
				}
			}

			l_value.m_content.reset(nullptr);
		}

		template< class TContent, class TLoader >
		void bc_content_manager::register_loader(bc_cloader_ptr< TLoader >&& p_loader)
		{
			static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherite from bc_icontent");
			static_assert(std::is_base_of< bc_icontent_loader, TLoader >::value, "Content loader must inherite from bc_icontent_loader");

			bc_service_manager::get().register_service< _content_wrapper< TContent > >
			(
				core::bc_make_service< _loader_wrapper< TContent > >(std::move(p_loader))
			);
		}

		template< class TContent >
		bc_content_ptr< TContent > bc_content_manager::load(const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter)
		{
			return load<TContent>(bc_alloc_type::unknown_movable, p_file, std::move(p_parameter));
		}

		template< class TContent >
		bc_content_ptr< TContent > bc_content_manager::load(bc_alloc_type p_alloc_type, const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter)
		{
			static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherite from bc_icontent");

			// Make hash, combination of both file path and content name because some contents like shaders load from same file
			bc_estring_frame l_offline_file_path = _get_offline_file_path<TContent>(p_file);
			auto l_hash = std::hash< bc_estring_frame >()(l_offline_file_path);

			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > m_guard(m_contents_mutex);
				auto l_content_ite = m_contents.find(l_hash);

				if (l_content_ite != m_contents.end()) // Content has already loaded
				{
					return bc_content_ptr< TContent >(l_content_ite->second.m_content.get(), _bc_content_ptr_deleter(this));
				}
			}

			bc_icontent_loader* l_loader = _get_loader<TContent>();

			bc_unique_ptr< TContent > l_content = _load_content< TContent >(p_alloc_type, p_file, l_offline_file_path.c_str(), std::move(p_parameter), l_loader);

			bcAssert(l_content != nullptr);

			return _store_new_content(l_hash, p_file, std::move(l_content));
		}

		template< class TContent >
		bc_task< bc_content_ptr< TContent > > bc_content_manager::load_async(const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter)
		{
			return load_async<TContent>(bc_alloc_type::unknown_movable, p_file, p_parameter);
		}

		template< class TContent >
		bc_task<bc_content_ptr<TContent>> bc_content_manager::load_async(bc_alloc_type p_alloc_type, const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter)
		{
			auto l_task = bc_concurrency::start_task
			(
				[=]()
				{
					return load< TContent >(p_alloc_type, p_file, p_parameter);
				},
				bc_task_creation_option::policy_fairness
			);

			return l_task;
		}

		template< class TContent >
		void bc_content_manager::save(TContent& p_content)
		{
			static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherite from bc_icontent");

			const bcECHAR* l_file_path;

			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > m_guard(m_contents_mutex);

				auto l_content_ite = m_contents.find(p_content._get_hash());
				if (l_content_ite == m_contents.end())
				{
					throw bc_invalid_argument_exception("Content is not valid");
				}

				l_file_path = l_content_ite->second.m_file_path.c_str();
			}

			bc_file_stream l_file_stream;
			bc_icontent_loader* l_loader = _get_loader<TContent>();
			bc_content_saving_context l_context;
			l_context.m_file_path = l_file_path;
			l_context.m_content = &p_content;
			
			bc_estring_frame l_file_to_open = l_loader->support_offline_processing() ? _get_offline_file_path<TContent>(l_file_path) : l_file_path;

			{
				_bc_content_loader_guard< TContent > l_guard(*l_loader, l_context);

				if (!l_file_stream.open_write(l_file_to_open.c_str()))
				{
					l_context.m_file.reset(bc_stream(std::move(l_file_stream)));
					l_loader->content_file_open_failed(l_context);

					auto l_file_name = bc_to_exclusive_string(l_file_to_open.c_str());
					auto l_error_msg = bc_string_frame("error in opening content file: ") + l_file_name.c_str();

					throw bc_io_exception(l_error_msg.c_str());
				}

				l_context.m_file.reset(bc_stream(std::move(l_file_stream)));
				l_loader->content_processing(l_context);

				l_context.m_file->close();
			}
		}

		template< class TContent >
		bc_task<void> bc_content_manager::save_async(TContent& p_content)
		{
			auto l_task = bc_concurrency::start_task<void>
			(
				[&]()
				{
					save< TContent >(p_content);
				},
				bc_task_creation_option::policy_fairness
			);

			return l_task;
		}

		template< class TContent >
		bc_content_ptr<TContent> bc_content_manager::store_content(const bcCHAR* p_content_name, TContent&& p_content)
		{
			return store_content(bc_alloc_type::unknown, p_content_name, std::move(p_content));
		}

		template< class TContent >
		bc_content_ptr<TContent> bc_content_manager::store_content(bc_alloc_type p_alloc_type, const bcCHAR* p_content_name, TContent&& p_content)
		{
			auto l_prev_alloc_type = bc_object_allocator::set_allocator_alloc_type(p_alloc_type);

			bc_unique_ptr<TContent> l_content = bc_object_allocator::allocate<TContent>(std::move(p_content));

			bc_object_allocator::set_allocator_alloc_type(l_prev_alloc_type);

			auto l_content_hash = string_hash()(p_content_name);
			
			return _store_new_content(l_content_hash, bcL("non-file content"), std::move(l_content));
		}

		template< class TContent >
		bc_icontent_loader* bc_content_manager::_get_loader()
		{
			_loader_wrapper< TContent >* l_loader_wrapper = static_cast< _loader_wrapper< TContent >* >
			(
				bc_get_service< _content_wrapper< TContent > >()
			);

			if (l_loader_wrapper == nullptr)
			{
				throw bc_invalid_argument_exception((bc_string("Loader for ") + _loader_wrapper< TContent >::service_name() + " not found").c_str());
			}

			bc_icontent_loader* l_loader = l_loader_wrapper->m_loader.get();

			return l_loader;
		}

		template <class TContent >
		bc_estring_frame bc_content_manager::_get_offline_file_path(const bcECHAR* p_file)
		{
			bc_estring_frame l_offline_file_path = bc_estring_frame(p_file) +
				bcL(".") +
				bc_to_estring_frame(bc_string_frame(bc_content_traits< TContent >::content_name())) +
				bcL(".bcc");

			return l_offline_file_path;
		}

		template< class TContent >
		bc_unique_ptr< TContent > bc_content_manager::_load_content(bc_alloc_type p_alloc_type,
			const bcECHAR* p_file, 
			const bcECHAR* p_offline_file, 
			bc_content_loader_parameter&& p_parameter, 
			bc_icontent_loader* p_loader)
		{
			core_platform::bc_basic_file_info l_file_info;
			core_platform::bc_basic_file_info l_offline_file_info;
			core_platform::bc_file_info::get_basic_info(p_file, &l_file_info);
			core_platform::bc_file_info::get_basic_info(p_offline_file, &l_offline_file_info);

			const bool l_need_offline_processing = p_loader->support_offline_processing() &&
			(
				!l_offline_file_info.m_exist ||
				l_offline_file_info.m_last_write_time.m_total_milliseconds < l_file_info.m_last_write_time.m_total_milliseconds
			);

			bc_content_loading_context l_context;
			bc_unique_ptr< TContent > l_result;

			l_context.m_file_path = p_file;
			l_context.m_parameter = std::move(p_parameter);
			l_context.set_allocator_alloc_type(p_alloc_type);

			{
				_bc_content_loader_guard< TContent > l_guard(*p_loader, l_context);

				if (l_need_offline_processing)
				{
					bc_file_stream l_file_stream;
					bc_file_stream l_offline_file_stream;

					if (!l_file_stream.open_read(p_file))
					{
						l_context.m_file.reset(bc_stream(std::move(l_file_stream)));
						p_loader->content_file_open_failed(l_context);
						
						auto l_file_name = bc_to_exclusive_string(p_file);
						auto l_error_msg = bc_string_frame("error in opening content file: ") + l_file_name.c_str();

						throw bc_io_exception(l_error_msg.c_str());
					}

					l_context.m_file.reset(bc_stream(std::move(l_file_stream)));
					p_loader->content_file_open_succeeded(l_context);

					if (!l_offline_file_stream.open
						(
							p_offline_file,
							core_platform::bc_file_mode::create_overwrite,
							core_platform::bc_file_access::write,
							core_platform::bc_file_sharing::none
						))
					{
						l_context.m_file.reset(bc_stream(std::move(l_offline_file_stream)));
						p_loader->content_file_open_failed(l_context);

						auto l_file_name = bc_to_exclusive_string(p_offline_file);
						auto l_error_msg = bc_string_frame("error in opening content file: ") + l_file_name.c_str();

						throw bc_io_exception(l_error_msg.c_str());
					}

					try
					{
						l_context.m_file.reset(bc_stream(std::move(l_offline_file_stream)));
						p_loader->content_offline_processing(l_context);
					}
					catch (const std::exception& p_exception)
					{
						l_context.m_file->close();

						bc_path l_offline_file(p_offline_file);
						l_offline_file.delete_path();

						bc_string_frame l_message = bc_string_frame("Error in loading content file: ") + 
							bc_to_exclusive_string(p_offline_file).c_str() + 
							". " + 
							p_exception.what();
						throw bc_io_exception(l_message.c_str());
					}
					
					l_context.m_file->close();
				}

				const bcECHAR* l_file_to_open = p_loader->support_offline_processing() ? p_offline_file : p_file;
				bc_file_stream l_file_stream;

				if (!l_file_stream.open_read(l_file_to_open))
				{
					l_context.m_file.reset(bc_stream(std::move(l_file_stream)));
					p_loader->content_file_open_failed(l_context);
					
					auto l_file_name = bc_to_exclusive_string(l_file_to_open);
					auto l_error_msg = bc_string_frame("error in opening content file: ") + l_file_name.c_str();

					throw bc_io_exception(l_error_msg.c_str());
				}

				l_context.m_file.reset(bc_stream(std::move(l_file_stream)));
				if(p_loader->support_offline_processing())
				{
					p_loader->content_offline_file_open_succeeded(l_context);
				}
				else
				{
					p_loader->content_file_open_succeeded(l_context);
				}
				
				p_loader->content_processing(l_context);

				l_context.m_file->close();

				l_result = p_loader->finish(l_context).get_result<TContent>();
			}

			return l_result;
		}

		template< class TContent >
		bc_content_ptr<TContent>  bc_content_manager::_store_new_content(bc_content_hash_t::result_type p_content_hash, 
			const bcECHAR* p_content_file, 
			bc_unique_ptr< TContent > p_content)
		{
			auto l_content_ptr = p_content.get();
			auto l_pointer = bc_unique_ptr< bc_icontent >(std::move(p_content));

			l_pointer->_set_hash(p_content_hash);

			{
				core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_guard(m_contents_mutex);

				_bc_content_entry l_entry(p_content_file, std::move(l_pointer));
				m_contents.insert(map_type::value_type(p_content_hash, std::move(l_entry)));
			}

			return bc_content_ptr<TContent>(l_content_ptr, _bc_content_ptr_deleter(this));
		}
	}
}