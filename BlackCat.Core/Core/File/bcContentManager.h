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
#include "Core/File/bcContent.h"
#include "Core/File/bcContentLoader.h"
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
			return core::bc_make_unique< TLoader >(bc_alloc_type::program, std::forward< TArgs >()...);
		}

		template< class TContent >
		class _content_wrapper : public bc_iservice
		{
		public:
			static const bcCHAR* service_name()
			{
				return bc_content_traits< TContent >::content_name();
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

		// Thread safe class
		class bc_content_manager : public bc_iservice
		{
		private:
			using string_hash = std::hash<const bcECHAR*>;
			using map_type = bc_unordered_map< string_hash::result_type, bc_object_allocator::ptr< bc_icontent > >;

		public:
			bc_content_manager();

			~bc_content_manager();

			bc_content_manager(bc_content_manager&&);

			bc_content_manager& operator=(bc_content_manager&&);

			template< class TContent, class TLoader >
			void register_loader(bc_cloader_ptr< TLoader >&& p_loader);

			// Load specified content from file or if it's loaded earlier return a pointer to it
			// file name must be relative
			template< class TContent >
			bc_content_ptr< TContent > load(const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter);

			// Load specified content from file or if it's loaded earlier return a pointer to it
			// file name must be relative
			template< class TContent >
			bc_content_ptr< TContent > load(bc_alloc_type p_alloc_type, const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter);

			template< class TContent >
			bc_task< bc_content_ptr< TContent > > load_async(const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter);

			template< class TContent >
			bc_task< bc_content_ptr< TContent > > load_async(bc_alloc_type p_alloc_type, const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter);

			void destroy_content(bc_icontent* p_content);

			static const bcCHAR* service_name()
			{
				return g_srv_content_manager;
			}

		protected:

		private:
			template< class TContent >
			bc_object_allocator::ptr< TContent > _load_content(bc_alloc_type p_alloc_type, const bcECHAR* p_file, const bcECHAR* p_offline_file, bc_content_loader_parameter&& p_parameter, bc_icontent_loader* p_loader);

			template< class TContent >
			void _store_new_content(string_hash::result_type p_content_hash, bc_object_allocator::ptr< TContent > p_content);

			map_type m_contents;
			core_platform::bc_shared_mutex m_contents_mutex;
		};

		inline bc_content_manager::bc_content_manager()
		{
		}

		inline bc_content_manager::~bc_content_manager()
		{
		}

		inline bc_content_manager::bc_content_manager(bc_content_manager&& p_other)
			: m_contents(move(p_other.m_contents))
		{
		}

		inline bc_content_manager& bc_content_manager::operator=(bc_content_manager&& p_other)
		{
			m_contents = move(p_other.m_contents);

			return *this;
		}

		inline void bc_content_manager::destroy_content(bc_icontent* p_content)
		{
			map_type::value_type::second_type l_value;

			core_platform::bc_lock_guard< core_platform::bc_shared_mutex > m_guard(m_contents_mutex);
			{
				auto l_item = std::find_if(begin(m_contents), end(m_contents), [=](map_type::value_type& p_item)
				{
					return p_item.second.get() == p_content;
				});

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

		template< class TContent, class TLoader >
		void bc_content_manager::register_loader(bc_cloader_ptr< TLoader >&& p_loader)
		{
			static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherite from bc_icontent");
			static_assert(std::is_base_of< bc_icontent_loader, TLoader >::value, "Content loader must inherite from bc_icontent_loader");

			bc_service_manager::get().register_service< _content_wrapper< TContent > >
				(
					core::bc_make_unique< _loader_wrapper< TContent > >(bc_alloc_type::program, std::move(p_loader))
				);
		}

		template< class TContent >
		bc_content_ptr< TContent > bc_content_manager::load(const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter)
		{
			return load<TContent>(bc_alloc_type::unknown_movale, p_file, p_parameter);
		}

		template< class TContent >
		bc_content_ptr< TContent > bc_content_manager::load(bc_alloc_type p_alloc_type, const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter)
		{
			static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherite from bc_icontent");

			// Make hash, combination of both file path and content name because some contents like shaders load 
			// from same file
			bc_estring_frame l_offline_file_path = bc_estring_frame(p_file) +
				bcL(".") +
				bc_to_estring_frame(bc_string_frame(bc_content_traits< TContent >::content_name())) +
				bcL(".bcc");
			auto l_hash = std::hash< bc_estring_frame >()(l_offline_file_path);

			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > m_guard(m_contents_mutex);
				{
					auto l_content_ite = m_contents.find(l_hash);

					if (l_content_ite != m_contents.end()) // Content has alreay loaded
					{
						return bc_content_ptr< TContent >(l_content_ite->second.get(), _bc_content_ptr_deleter(this));
					}
				}
			}

			_loader_wrapper< TContent >* l_loader_wrapper = static_cast< _loader_wrapper< TContent >* >
				(
					bc_service_manager::get().get_service< _content_wrapper< TContent > >()
				);

			if (l_loader_wrapper == nullptr)
				throw bc_invalid_argument_exception((bc_string("Loader for ") + _loader_wrapper< TContent >::service_name() + " not found").c_str());

			bc_icontent_loader* l_loader = l_loader_wrapper->m_loader.get();

			bc_object_allocator::ptr< TContent > l_content = _load_content< TContent >(p_alloc_type, p_file, l_offline_file_path.c_str(), std::move(p_parameter), l_loader);
			TContent* l_content_pointer = l_content.get();

			bcAssert(l_content_pointer != nullptr);

			_store_new_content(l_hash, std::move(l_content));

			return bc_content_ptr< TContent >(l_content_pointer, _bc_content_ptr_deleter(this));
		}

		template< class TContent >
		bc_task< bc_content_ptr< TContent > > bc_content_manager::load_async(const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter)
		{
			return load_async<TContent>(bc_alloc_type::unknown_movale, p_file, p_parameter);
		}

		template< class TContent >
		bc_task<bc_content_ptr<TContent>> bc_content_manager::load_async(bc_alloc_type p_alloc_type, const bcECHAR* p_file, bc_content_loader_parameter&& p_parameter)
		{
			auto l_task = core::bc_concurreny::start_task
				(
					[=]()
					{
						return load< TContent >(p_alloc_type, p_file, p_parameter);
					},
					bc_task_creation_option::fairness
				);

			return l_task;
		}

		template< class TContent >
		bc_object_allocator::ptr< TContent > bc_content_manager::_load_content(bc_alloc_type p_alloc_type, const bcECHAR* p_file, const bcECHAR* p_offline_file, bc_content_loader_parameter&& p_parameter, bc_icontent_loader* p_loader)
		{
			bc_estring l_file_path = bc_path::get_absolute_path(p_file);
			bc_estring l_offline_file_path = bc_path::get_absolute_path(p_offline_file);
			core_platform::bc_basic_file_info l_file_info;
			core_platform::bc_basic_file_info l_offline_file_info;
			core_platform::bc_file_info::get_basic_info(l_file_path.c_str(), &l_file_info);
			core_platform::bc_file_info::get_basic_info(l_offline_file_path.c_str(), &l_offline_file_info);

			bool l_need_to_offline_proccessing = !l_offline_file_info.m_exist ||
				l_offline_file_info.m_last_write_time.m_total_milliseconds < l_file_info.m_last_write_time.m_total_milliseconds;

			bc_file_stream l_file_stream;
			bc_file_stream l_offline_file_stream;
			bc_content_loader_context l_context;
			bc_object_allocator::ptr< TContent > l_result;

			l_context.m_file_path = p_file;
			l_context.m_parameter = std::move(p_parameter);
			l_context.set_allocator_alloc_type(p_alloc_type);

			{
				_bc_content_loader_gaurd< TContent > l_gaurd(*p_loader, l_context);

				if (l_need_to_offline_proccessing)
				{
					if (!l_file_stream.open_read(l_file_path.c_str()))
					{
						l_context.m_file.reset(bc_stream(std::move(l_file_stream)));
						p_loader->content_file_open_failed(l_context);
						
						throw bc_io_exception("error in opening content file");
					}

					l_context.m_file.reset(bc_stream(std::move(l_file_stream)));
					p_loader->content_file_open_successed(l_context);

					if (!l_offline_file_stream.open
						(
							l_offline_file_path.c_str(),
							core_platform::bc_file_mode::create_overwrite,
							core_platform::bc_file_access::write,
							core_platform::bc_file_sharing::none
						))
					{
						l_context.m_file.reset(bc_stream(std::move(l_offline_file_stream)));
						p_loader->content_file_open_failed(l_context);
						
						throw bc_io_exception("error in opening content file");
					}

					try
					{
						p_loader->content_offline_processing(l_context);
					}
					catch (...)
					{
						l_offline_file_stream.close();

						bc_path l_offline_file(l_offline_file_path.c_str());
						l_offline_file.delete_path();

						std::abort();
					}
					
					l_context.m_file.reset(bc_stream(l_offline_file_stream));
					p_loader->content_offline_saving(l_context);

					l_context.m_file->close();
				}

				if (!l_offline_file_stream.open_read(l_offline_file_path.c_str()))
				{
					l_context.m_file.reset(bc_stream(std::move(l_offline_file_stream)));
					p_loader->content_file_open_failed(l_context);
					
					throw bc_io_exception("error in opening content file");
				}

				l_context.m_file.reset(bc_stream(std::move(l_offline_file_stream)));
				p_loader->content_offline_file_open_successed(l_context);
				p_loader->content_processing(l_context);

				l_result = p_loader->finish(l_context).get_result<TContent>();
			}

			return l_result;
		}

		template< class TContent >
		void bc_content_manager::_store_new_content(string_hash::result_type p_content_hash, bc_object_allocator::ptr< TContent > p_content)
		{
			auto l_pointer = bc_object_allocator::ptr< bc_icontent >
				(
					p_content.release(),
					reinterpret_cast< void(*)(bc_icontent*) >(p_content.get_deleter())
				);

			core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_guard(m_contents_mutex);
			{
				m_contents.insert(map_type::value_type(p_content_hash, std::move(l_pointer)));
			}
		}
	}
}
