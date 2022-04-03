// [03/28/2016 MRB]

#pragma once

#include "CorePlatformImp/File/bcFileInfo.h"
#include "CorePlatformImp/File/bcFile.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcStringStream.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/File/bcPath.h"
#include "Core/File/bcStream.h"
#include "Core/File/bcFileStream.h"
#include "Core/Content/bcContent.h"
#include "Core/Content/bcContentLoader.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcLogger.h"
#include "Core/bcExport.h"
#include "Core/bcException.h"
#include "Core/bcConstant.h"

namespace black_cat
{
	namespace core
	{
		template<typename TLoader>
		using bc_cloader_ptr = bc_service_ptr<TLoader>;

		template<typename TLoader, typename ...TArgs>
		bc_cloader_ptr<TLoader> bc_make_loader(TArgs&&... p_args)
		{
			return core::bc_make_unique<TLoader>(bc_alloc_type::program, std::forward<TArgs>(p_args)...);
		}

		template<class TContent>
		class _content_wrapper : public bci_service
		{
		public:
			static constexpr const bcCHAR* service_name()
			{
				return bc_content_traits<TContent>::content_name();
			}

			static constexpr bcUINT32 service_hash()
			{
				return bc_content_traits<TContent>::content_hash();
			}
		};

		// Because in service manager registered instance must be a derived type of registered type
		// we make loader derived type of content type through a wrapper class
		template<class TContent>
		class _loader_wrapper : public _content_wrapper<TContent>
		{
		public:
			_loader_wrapper(bc_cloader_ptr<bci_content_loader> p_loader)
				: m_loader(std::move(p_loader))
			{
			}

			bc_cloader_ptr<bci_content_loader> m_loader;
		};

		class _bc_content_entry
		{
		public:
			_bc_content_entry() = default;

			_bc_content_entry(bc_estring p_key, bc_estring p_file, bc_estring p_file_variant, bc_unique_ptr<bci_content>&& p_content)
				: m_file(std::move(p_file)),
				m_file_variant(std::move(p_file_variant)),
				m_content(std::move(p_content))
			{
				constexpr auto l_min_key_size = sizeof(decltype(m_key)) + 1;

				if(p_key.size() < l_min_key_size)
				{
					// because we use this string as key for our contents map which must prevent string to use SSO by forcing it
					// to allocate memory from heap
					m_key.reserve(l_min_key_size);
					m_key.append(p_key);
				}
				else
				{
					m_key = std::move(p_key);
				}
			}

			bc_estring m_key;
			bc_estring m_file;
			bc_estring m_file_variant;
			bc_unique_ptr<bci_content> m_content;
		};

		/**
		 * \brief 
		 * \ThreadSafe
		 */
		class BC_CORE_DLL bc_content_manager : public bci_service, protected bc_object_allocator
		{
			BC_SERVICE(cnt_mng)

		private:
			using map_type = bc_unordered_map<bc_estring_view, _bc_content_entry>;

		public:
			bc_content_manager();

			bc_content_manager(bc_content_manager&&) noexcept = delete;

			~bc_content_manager() override;

			bc_content_manager& operator=(bc_content_manager&&) noexcept = delete;

			template<class TContent, class TLoader>
			void register_loader(bc_cloader_ptr<TLoader> p_loader);

			/**
			 * \brief Load specified content from file or if it's loaded earlier return a pointer to it.
			 * file name must be absolute path
			 * \tparam TContent 
			 * \param p_file
			 * \param p_file_variant Used in offline file name so different offline files can be created from one input content file
			 * \param p_parameters
			 * \param p_instance_parameters 
			 * \return 
			 */
			template<class TContent>
			bc_content_ptr<TContent> load(bc_estring_view p_file,
				bc_estring_view p_file_variant,
				const bc_content_loader_parameter& p_parameters,
				bc_content_loader_parameter p_instance_parameters = bc_content_loader_parameter(bc_alloc_type::frame));

			/**
			 * \brief Load specified content from file or if it's loaded earlier return a pointer to it.
			 * file name must be absolute path
			 * \tparam TContent 
			 * \param p_alloc_type 
			 * \param p_file
			 * \param p_file_variant Used in offline file name so different offline files can be created from one input content file
			 * \param p_parameters
			 * \param p_instance_parameters 
			 * \return 
			 */
			template<class TContent>
			bc_content_ptr<TContent> load(bc_alloc_type p_alloc_type,
				bc_estring_view p_file,
				bc_estring_view p_file_variant,
				const bc_content_loader_parameter& p_parameters,
				bc_content_loader_parameter p_instance_parameters = bc_content_loader_parameter(bc_alloc_type::frame));

			template<class TContent>
			bc_task<bc_content_ptr<TContent>> load_async(bc_estring_view p_file,
				bc_estring_view p_file_variant,
				const bc_content_loader_parameter& p_parameters,
				bc_content_loader_parameter p_instance_parameters = bc_content_loader_parameter(bc_alloc_type::frame));

			template<class TContent>
			bc_task<bc_content_ptr<TContent>> load_async(bc_alloc_type p_alloc_type,
				bc_estring_view p_file,
				bc_estring_view p_file_variant,
				const bc_content_loader_parameter& p_parameters,
				bc_content_loader_parameter p_instance_parameters = bc_content_loader_parameter(bc_alloc_type::frame));
						
			template<class TContent>
			void save(TContent& p_content);

			template<class TContent>
			void save_as(TContent& p_content, bc_estring_view p_file_path, bc_estring_view p_file_variant);

			template<class TContent>
			bc_task<void> save_async(TContent& p_content);

			template<class TContent>
			bc_task<void> save_as_async(TContent& p_content, bc_estring_view p_file_path, bc_estring_view p_file_variant);

			/**
			 * \brief Import content to manager so a content ptr can be constructed from it.
			 * \tparam TContent 
			 * \param p_content_name 
			 * \param p_content 
			 * \return 
			 */
			template<class TContent>
			bc_content_ptr<TContent> store_content(bc_estring_view p_content_name, TContent&& p_content);

			/**
			* \brief Import content to manager so a content ptr can be constructed from it.
			* \tparam TContent
			* \param p_alloc_type
			* \param p_content_name
			* \param p_content
			* \return
			*/
			template<class TContent>
			bc_content_ptr<TContent> store_content(bc_alloc_type p_alloc_type, bc_estring_view p_content_name, TContent&& p_content);

			void destroy_content(const bci_content* p_content);

		private:
			template<class TContent>
			bci_content_loader* _get_loader();

			template<class TContent>
			bc_estring_frame _get_offline_file_path(bc_estring_view p_file, bc_estring_view p_file_variant);

			template<class TContent>
			bc_unique_ptr<TContent> _load_content(bc_alloc_type p_alloc_type,
				bc_estring_view p_file,
				bc_estring_view p_file_variant,
				bc_estring_view p_offline_file,
				const bc_content_loader_parameter& p_parameter,
				bc_content_loader_parameter p_instance_parameters,
				bci_content_loader* p_loader);

			template<class TContent>
			bc_content_ptr<TContent> _store_new_content(bc_estring p_key,
				bc_estring_view p_file,
				bc_estring_view p_file_variant,
				bc_unique_ptr<TContent> p_content);

			platform::bc_shared_mutex m_contents_mutex;
			map_type m_contents;
		};

		template<class TContent, class TLoader>
		void bc_content_manager::register_loader(bc_cloader_ptr<TLoader> p_loader)
		{
			static_assert(std::is_base_of_v<bci_content, TContent>, "Content must inherit from bc_icontent");
			static_assert(std::is_base_of_v<bci_content_loader, TLoader>, "Content loader must inherit from bc_icontent_loader");

			auto* l_loader_wrapper = static_cast<_loader_wrapper<TContent>*>
			(
				bc_get_service<_content_wrapper<TContent>>()
			);
			
			if(!l_loader_wrapper)
			{
				bc_register_service<_content_wrapper<TContent>>
				(
					core::bc_make_service<_loader_wrapper<TContent>>(std::move(p_loader))
				);
			}
			else
			{
				l_loader_wrapper->m_loader = std::move(p_loader);
			}
		}

		template<class TContent>
		bc_content_ptr<TContent> bc_content_manager::load(bc_estring_view p_file,
			bc_estring_view p_file_variant,
			const bc_content_loader_parameter& p_parameters, 
			bc_content_loader_parameter p_instance_parameters)
		{
			return load<TContent>(bc_alloc_type::unknown, p_file, p_file_variant, std::move(p_parameters), std::move(p_instance_parameters));
		}

		template<class TContent>
		bc_content_ptr<TContent> bc_content_manager::load(bc_alloc_type p_alloc_type,
			bc_estring_view p_file,
			bc_estring_view p_file_variant,
			const bc_content_loader_parameter& p_parameters,
			bc_content_loader_parameter p_instance_parameters)
		{
			static_assert(std::is_base_of_v<bci_content, TContent>, "Content must inherit from bc_icontent");

			// Make hash, combination of both file path and content name because some contents like shaders load from same file
			const bc_estring_frame l_offline_file_path = _get_offline_file_path<TContent>(p_file, p_file_variant);
			auto l_key = bc_path(l_offline_file_path).get_filename();

			{
				platform::bc_shared_mutex_shared_guard l_guard(m_contents_mutex);
				
				const auto l_content_ite = m_contents.find(l_key);
				if (l_content_ite != m_contents.end()) // Content has already loaded
				{
					return bc_content_ptr<TContent>(l_content_ite->second.m_content.get(), _bc_content_ptr_deleter(this));
				}
			}

			bci_content_loader* l_loader = _get_loader<TContent>();
			bc_unique_ptr<TContent> l_content = _load_content<TContent>
			(
				p_alloc_type,
				p_file,
				p_file_variant,
				l_offline_file_path.c_str(),
				p_parameters,
				std::move(p_instance_parameters),
				l_loader
			);

			BC_ASSERT(l_content != nullptr);

			return _store_new_content(l_key, p_file, p_file_variant, std::move(l_content));
		}

		template<class TContent>
		bc_task<bc_content_ptr<TContent>> bc_content_manager::load_async(bc_estring_view p_file,
			bc_estring_view p_file_variant,
			const bc_content_loader_parameter& p_parameters, 
			bc_content_loader_parameter p_instance_parameters)
		{
			return load_async<TContent>(bc_alloc_type::unknown, p_file, p_file_variant, p_parameters, std::move(p_instance_parameters));
		}

		template<class TContent>
		bc_task<bc_content_ptr<TContent>> bc_content_manager::load_async(bc_alloc_type p_alloc_type,
			bc_estring_view p_file,
			bc_estring_view p_file_variant,
			const bc_content_loader_parameter& p_parameters, 
			bc_content_loader_parameter p_instance_parameters)
		{
			auto l_task = bc_concurrency::start_task
			(
				[=]()
				{
					return load<TContent>(p_alloc_type, p_file, p_file_variant, p_parameters, std::move(p_instance_parameters));
				}
			);

			return l_task;
		}

		template<class TContent>
		void bc_content_manager::save(TContent& p_content)
		{
			static_assert(std::is_base_of_v<bci_content, TContent>, "Content must inherit from bc_icontent");

			bc_estring_view l_file_path;
			bc_estring_view l_file_variant;

			{
				platform::bc_shared_mutex_shared_guard l_guard(m_contents_mutex);

				auto l_content_ite = m_contents.find(p_content._get_key());
				if (l_content_ite == m_contents.end())
				{
					throw bc_invalid_argument_exception("Content not found");
				}

				l_file_path = l_content_ite->second.m_file;
				l_file_variant = l_content_ite->second.m_file_variant;
			}

			save_as<TContent>(p_content, l_file_path, l_file_variant);
		}

		template<class TContent>
		void bc_content_manager::save_as(TContent& p_content, bc_estring_view p_file_path, bc_estring_view p_file_variant)
		{
			static_assert(std::is_base_of_v<bci_content, TContent>, "Content must inherit from bc_icontent");

			bci_content_loader* l_loader = _get_loader<TContent>();
			bc_content_saving_context l_context(*this, p_file_path, p_file_variant, p_content);

			const bc_estring_frame l_file_to_open = l_loader->support_offline_processing()
				                                        ? _get_offline_file_path<TContent>(p_file_path, p_file_variant)
				                                        : bc_estring_frame(p_file_path);

			{
				_bc_content_loader_guard<TContent> l_guard(*l_loader, l_context);
				bc_file_stream l_file_stream;
				
				if (!l_file_stream.open_write(l_file_to_open.c_str()))
				{
					l_context.m_file = bc_stream(std::move(l_file_stream));
					l_loader->content_file_open_failed(l_context);

					const auto l_file_name = bc_to_exclusive_string(l_file_to_open.c_str());
					const auto l_error_msg = bc_string_frame("Error in opening content file '") + l_file_name.c_str() + "'";

					throw bc_io_exception(l_error_msg.c_str());
				}

				l_context.m_file = bc_stream(std::move(l_file_stream));
				l_loader->content_processing(l_context);

				l_context.m_file.close();
			}
		}

		template<class TContent>
		bc_task<void> bc_content_manager::save_async(TContent& p_content)
		{
			auto l_task = bc_concurrency::start_task<void>
			(
				[=, &p_content]()
				{
					save<TContent>(p_content);
				}
			);

			return l_task;
		}

		template<class TContent>
		bc_task<void> bc_content_manager::save_as_async(TContent& p_content, bc_estring_view p_file_path, bc_estring_view p_file_variant)
		{
			auto l_task = bc_concurrency::start_task<void>
			(
				[=, &p_content]()
				{
					save_as<TContent>(p_content, p_file_path, p_file_variant);
				}
			);

			return l_task;
		}

		template<class TContent>
		bc_content_ptr<TContent> bc_content_manager::store_content(bc_estring_view p_content_name, TContent&& p_content)
		{
			return store_content(bc_alloc_type::unknown, p_content_name, std::move(p_content));
		}

		template<class TContent>
		bc_content_ptr<TContent> bc_content_manager::store_content(bc_alloc_type p_alloc_type, bc_estring_view p_content_name, TContent&& p_content)
		{
			const auto l_prev_alloc_type = bc_object_allocator::set_allocator_alloc_type(p_alloc_type);

			bc_unique_ptr<TContent> l_content = bc_object_allocator::allocate<TContent>(std::move(p_content));

			bc_object_allocator::set_allocator_alloc_type(l_prev_alloc_type);

			return _store_new_content(bc_estring(p_content_name), bcL("non-file content"), bcL(""), std::move(l_content));
		}

		template<class TContent>
		bci_content_loader* bc_content_manager::_get_loader()
		{
			auto* l_loader_wrapper = static_cast<_loader_wrapper<TContent>*>
			(
				bc_get_service<_content_wrapper<TContent>>()
			);

			if (l_loader_wrapper == nullptr)
			{
				const auto l_msg = bc_string_stream_frame() << "Loader for " << _loader_wrapper<TContent>::service_name() << " not found";
				throw bc_invalid_argument_exception(l_msg.str().c_str());
			}

			bci_content_loader* l_loader = l_loader_wrapper->m_loader.get();

			return l_loader;
		}

		template<class TContent>
		bc_estring_frame bc_content_manager::_get_offline_file_path(bc_estring_view p_file, bc_estring_view p_file_variant)
		{
			bc_path l_file_path(p_file);
			bc_estring_frame l_file_variant;

			if(p_file_variant.empty())
			{
				l_file_variant = l_file_path.get_filename_without_extension_frame();
			}
			else
			{
				l_file_variant += p_file_variant;
			}

			auto l_offline_file_name = bc_estring_stream_frame()
					<< l_file_variant
					<< bcL(".")
					<< bc_to_estring_frame(bc_content_traits<TContent>::content_name())
					<< bcL(".bcc");

			l_file_path.set_filename(l_offline_file_name.str().c_str());
			
			return l_file_path.get_string_frame();
		}

		template<class TContent>
		bc_unique_ptr<TContent> bc_content_manager::_load_content(bc_alloc_type p_alloc_type,
			bc_estring_view p_file,
			bc_estring_view p_file_variant,
			bc_estring_view p_offline_file,
			const bc_content_loader_parameter& p_parameter,
			bc_content_loader_parameter p_instance_parameters,
			bci_content_loader* p_loader)
		{
			platform::bc_basic_file_info l_file_info;
			platform::bc_basic_file_info l_offline_file_info;
			platform::bc_file_info::get_basic_info(p_file.data(), &l_file_info);
			platform::bc_file_info::get_basic_info(p_offline_file.data(), &l_offline_file_info);

			const bool l_need_offline_processing = p_loader->support_offline_processing() &&
			(
				!l_offline_file_info.m_exist ||
				l_offline_file_info.m_last_write_time.m_total_milliseconds <l_file_info.m_last_write_time.m_total_milliseconds ||
				l_offline_file_info.m_size == 0
			);

			bc_unique_ptr<TContent> l_result;
			bc_content_loading_context l_context(*this, p_file, p_file_variant, p_parameter, std::move(p_instance_parameters));
			l_context.set_allocator_alloc_type(p_alloc_type);

			{
				_bc_content_loader_guard<TContent> l_guard(*p_loader, l_context);

				if (l_need_offline_processing)
				{
					bc_file_stream l_file_stream;
					bc_file_stream l_offline_file_stream;

					if (!l_file_stream.open_read(p_file))
					{
						l_context.m_file = bc_stream(std::move(l_file_stream));
						p_loader->content_file_open_failed(l_context);
						
						auto l_file_name = bc_to_exclusive_string(p_file.data());
						auto l_error_msg = bc_string_frame("Error in opening content file '") + l_file_name.c_str() + "'";

						throw bc_io_exception(l_error_msg.c_str());
					}

					l_context.m_file = bc_stream(std::move(l_file_stream));
					p_loader->content_file_open_succeeded(l_context);

					if
					(
						!l_offline_file_stream.open
						(
							p_offline_file,
							platform::bc_file_mode::create_overwrite,
							platform::bc_file_access::write,
							platform::bc_file_sharing::none
						)
					)
					{
						l_context.m_file = bc_stream(std::move(l_offline_file_stream));
						p_loader->content_file_open_failed(l_context);

						auto l_file_name = bc_to_exclusive_string(p_offline_file.data());
						auto l_error_msg = bc_string_frame("Error in opening content file '") + l_file_name.c_str() + "'";

						throw bc_io_exception(l_error_msg.c_str());
					}

					try
					{
						l_context.m_file = bc_stream(std::move(l_offline_file_stream));
						p_loader->content_offline_processing(l_context);
					}
					catch (const std::exception& l_exception)
					{
						l_context.m_file.close();

						bc_path l_offline_file(p_offline_file);
						l_offline_file.delete_path();

						auto l_message = bc_string_frame("Error in loading content file '") + 
							bc_to_exclusive_string(p_offline_file.data()).c_str() + 
							"'. " + 
							l_exception.what();
						throw bc_io_exception(l_message.c_str());
					}
					
					l_context.m_file.close();
				}

				auto l_file_to_open = p_loader->support_offline_processing() ? p_offline_file : p_file;
				bc_file_stream l_file_stream;

				if (!l_file_stream.open_read(l_file_to_open))
				{
					l_context.m_file = bc_stream(std::move(l_file_stream));
					p_loader->content_file_open_failed(l_context);
					
					auto l_file_name = bc_to_exclusive_string(l_file_to_open.data());
					auto l_error_msg = bc_string_frame("Error in opening content file '") + l_file_name.c_str() + "'";

					throw bc_io_exception(l_error_msg.c_str());
				}

				l_context.m_file = bc_stream(std::move(l_file_stream));
				if(p_loader->support_offline_processing())
				{
					p_loader->content_offline_file_open_succeeded(l_context);
				}
				else
				{
					p_loader->content_file_open_succeeded(l_context);
				}

				try
				{
					p_loader->content_processing(l_context);
				}
				catch (const std::exception& l_exception)
				{
					l_context.m_file.close();

					auto l_message = bc_string_frame("Error in loading content file '") +
						bc_to_exclusive_string(l_file_to_open.data()).c_str() +
						"'. " +
						l_exception.what();
					throw bc_io_exception(l_message.c_str());
				}

				l_context.m_file.close();

				l_result = p_loader->finish(l_context).release_result<TContent>();
			}

			return l_result;
		}

		template<class TContent>
		bc_content_ptr<TContent> bc_content_manager::_store_new_content(bc_estring p_key,
			bc_estring_view p_file,
			bc_estring_view p_file_variant,
			bc_unique_ptr<TContent> p_content)
		{
			auto l_content_ptr = p_content.get();

			{
				platform::bc_lock_guard l_guard(m_contents_mutex);

				const auto l_ite = m_contents.find(p_key);
				if(l_ite == std::end(m_contents))
				{
					auto l_entry = _bc_content_entry(std::move(p_key), bc_estring(p_file), bc_estring(p_file_variant), bc_unique_ptr<bci_content>(std::move(p_content)));
					auto l_entry_key = bc_estring_view(l_entry.m_key);
					const auto l_insertion_result = m_contents.insert(map_type::value_type(l_entry_key, std::move(l_entry)));

					l_content_ptr->_set_key(l_insertion_result.first->second.m_key);
				}
				else
				{
					bc_log(bc_log_type::warning) << "content with key '" << p_key << "' already exist in the contents map. newly loaded content will be discarded." << bc_lend;
				}
			}

			return bc_content_ptr<TContent>(l_content_ptr, _bc_content_ptr_deleter(this));
		}
	}
}