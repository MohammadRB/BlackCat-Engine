// [05/09/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcList.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/File/bcFileStream.h"
#include "Core/File/bcPath.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Content/bcContent.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Utility/bcLogger.h"
#include "Core/bcException.h"

namespace black_cat
{
	namespace core
	{
		BC_JSON_STRUCTURE(_bc_content_stream_content)
		{
			BC_JSON_VALUE(bc_string_frame, content_title);
			BC_JSON_VALUE(bc_string_frame, content_loader);
			BC_JSON_VALUE(bc_string_frame, content_file);
			BC_JSON_VALUE(bc_json_key_value, content_parameter);
		};

		BC_JSON_STRUCTURE(_bc_content_stream)
		{
			BC_JSON_VALUE(bc_string, stream_name);
			BC_JSON_ARRAY(_bc_content_stream_content, stream_content);
		};

		BC_JSON_STRUCTURE(_bc_content_stream_json)
		{
			BC_JSON_ARRAY(_bc_content_stream, streams);
		};

		bc_content_stream_manager::bc_content_stream_manager(bc_content_manager& p_content_manager) noexcept
			: m_content_manager(p_content_manager)
		{
		}

		bc_content_stream_manager::~bc_content_stream_manager() = default;

		void bc_content_stream_manager::read_stream_file(bc_estring_view p_json_file_path)
		{
			bc_file_stream l_json_file;
			bc_string_frame l_json_file_content;

			if(!l_json_file.open_read(p_json_file_path))
			{
				const auto l_msg = bc_string_stream_frame() << "Error in reading content stream file: " << p_json_file_path;
				throw bc_io_exception(l_msg.str().c_str());
			}

			bc_read_all_lines(l_json_file, l_json_file_content);

			bc_json_document<_bc_content_stream_json> l_content_stream;
			l_content_stream.load(l_json_file_content.c_str());
			
			for (bc_json_object<_bc_content_stream>& l_stream : l_content_stream->m_streams)
			{
				bc_vector_program<_bc_content_stream_file> l_stream_files;
				l_stream_files.reserve(l_stream->m_stream_content.size());

				for (bc_json_object<_bc_content_stream_content>& l_stream_content : l_stream->m_stream_content)
				{
					auto l_file_path = bc_path::get_absolute_path(bc_to_estring_frame(*l_stream_content->m_content_file).c_str());

					_bc_content_stream_file l_stream_file
					{
						bc_string_program(*l_stream_content->m_content_title),
						bc_string_program(*l_stream_content->m_content_loader),
						bc_estring_program(l_file_path.get_string_frame()),
						std::move(*l_stream_content->m_content_parameter)
					};
					
					l_stream_files.push_back(std::move(l_stream_file));
				}

				auto l_ite = m_stream_descriptions.find(*l_stream->m_stream_name);
				if (l_ite == std::end(m_stream_descriptions))
				{
					m_stream_descriptions.insert(content_stream_map_type::value_type(std::move(*l_stream->m_stream_name), std::move(l_stream_files)));
				}
				else
				{
					l_ite->second = std::move(l_stream_files);
					core::bc_log(bc_log_type::warning) << "content stream description with name '" << *l_stream->m_stream_name << "' already exist. old stream will be replaced." << core::bc_lend;
				}
			}
		}

		void bc_content_stream_manager::load_content_stream(bc_alloc_type p_alloc_type, bc_string_view p_stream_name)
		{
			const auto l_stream_entry = m_stream_descriptions.find(bc_string(p_stream_name));

			if(l_stream_entry == std::end(m_stream_descriptions))
			{
				bc_string_frame l_msg = "Invalid content stream name: ";
				l_msg += p_stream_name;

				throw bc_invalid_argument_exception(l_msg.c_str());
			}

			auto& l_contents = l_stream_entry->second;

			bc_concurrency::concurrent_for_each
			(
				std::begin(l_contents),
				std::end(l_contents),
				[]()
				{
					return bc_list_frame<content_map_type::value_type>();
				},
				[this, p_alloc_type](bc_list_frame<content_map_type::value_type>& p_local, const _bc_content_stream_file& l_content_file)
				{
					content_map_type::iterator l_content_entry;
					content_map_type::iterator l_content_end;

					{
						platform::bc_shared_mutex_shared_guard l_lock_guard(m_contents_mutex);

						l_content_entry = m_contents.find(l_content_file.m_title);
						l_content_end = std::end(m_contents);
					}

					// Content already loaded from another stream
					if (l_content_entry != l_content_end)
					{
						{
							platform::bc_shared_mutex_guard l_lock_guard(m_contents_mutex);

							// Make a copy of content pointer to increase it's reference counter
							l_content_entry->second.push_back(*std::begin(l_content_entry->second));
							return;
						}
					}

					const auto l_loader_entry = m_content_loader_delegates.find(l_content_file.m_loader);
					if(l_loader_entry == std::end(m_content_loader_delegates))
					{
						const auto l_error_msg = "There isn't any registered loader for " + l_content_file.m_loader;
						throw bc_key_not_found_exception(l_error_msg.c_str());
					}

					bc_icontent_ptr l_content;

					try
					{
						l_content = l_loader_entry->second
						(
							p_alloc_type,
							l_content_file.m_file.c_str(),
							bc_to_estring_frame(l_content_file.m_title).c_str(),
							l_content_file.m_parameters
						);
					}
					catch (const std::exception& l_exception)
					{
						// Let other contents in the list be loaded.
						bc_log(bc_log_type::error) << "Error in loading content '" << l_content_file.m_title << "'. " << l_exception.what() << bc_lend;
						return;
					}

					content_map_type::value_type l_new_content_entry = content_map_type::value_type(l_content_file.m_title, content_map_type::value_type::second_type());
					l_new_content_entry.second.push_back(std::move(l_content));

					p_local.push_back(std::move(l_new_content_entry));
				},
				[this](bc_list_frame<content_map_type::value_type>& p_locals)
				{
					{
						platform::bc_shared_mutex_guard l_lock_guard(m_contents_mutex);

						for (auto& l_content_entry : p_locals)
						{
							m_contents.insert(std::move(l_content_entry));
						}
					}
				}
			);

			core::bc_log(bc_log_type::info) << "content stream '" << p_stream_name << "' loaded." << core::bc_lend;
		}

		bc_task<void> bc_content_stream_manager::load_content_stream_async(bc_alloc_type p_alloc_type, bc_string_view p_stream_name)
		{
			return bc_concurrency::start_task<void>([this, p_alloc_type, p_stream_name]()
			{
				this->load_content_stream(p_alloc_type, p_stream_name);
			});
		}

		void bc_content_stream_manager::unload_content_stream(bc_string_view p_stream_name)
		{
			const auto l_stream_entry = m_stream_descriptions.find(bc_string(p_stream_name));
			if (l_stream_entry == std::end(m_stream_descriptions))
			{
				bc_string_frame l_msg = "Invalid content stream name: ";
				l_msg += p_stream_name;

				throw bc_invalid_argument_exception(l_msg.c_str());
			}

			auto& l_contents = l_stream_entry->second;
			for (_bc_content_stream_file& l_content_file : l_contents)
			{
				content_map_type::iterator l_content_entry;
				content_map_type::iterator l_content_end;

				{
					platform::bc_shared_mutex_shared_guard l_lock_guard(m_contents_mutex);

					l_content_entry = m_contents.find(l_content_file.m_title);
					l_content_end = std::end(m_contents);
				}

				if (l_content_entry != l_content_end)
				{
					BC_ASSERT(!l_content_entry->second.empty());

					l_content_entry->second.pop_back();

					// Remove content entry if it's references reach zero
					if (l_content_entry->second.empty())
					{
						{
							platform::bc_shared_mutex_guard l_lock_guard(m_contents_mutex);

							m_contents.erase(l_content_entry);
						}
					}
				}
			}

			core::bc_log(bc_log_type::info) << "content stream '" << p_stream_name << "' unloaded." << core::bc_lend;
		}

		bc_icontent_ptr bc_content_stream_manager::find_content(bc_string_view p_content_name) const
		{
			content_map_type::const_iterator l_content_entry;
			content_map_type::const_iterator l_content_end;

			{
				platform::bc_shared_mutex_shared_guard l_lock_guard(m_contents_mutex);
				
				l_content_entry = m_contents.find(p_content_name);
				l_content_end = std::end(m_contents);
			}

			if (l_content_entry != l_content_end)
			{
				BC_ASSERT(!l_content_entry->second.empty());

				return *std::begin(l_content_entry->second);
			}

			return nullptr;
		}

		bc_icontent_ptr bc_content_stream_manager::find_content_throw(bc_string_view p_content_name) const
		{
			auto l_content = find_content(p_content_name);

			if(!l_content)
			{
				throw bc_key_not_found_exception(std::string("No content was found with key ") + p_content_name.data());
			}

			return l_content;
		}
	}
}