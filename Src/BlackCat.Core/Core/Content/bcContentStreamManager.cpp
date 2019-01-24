// [05/09/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcList.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/File/bcFileStream.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Content/bcContent.h"
#include "Core/Content/bcContentManager.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/bcException.h"

namespace black_cat
{
	namespace core
	{
		BC_JSON_STRUCTURE(_bc_content_stream_content)
		{
			BC_JSON_VALUE(bc_string_frame, content_title);
			BC_JSON_VALUE(bc_string_frame, content_name);
			BC_JSON_VALUE(bc_string_frame, content_file);
			BC_JSON_VALUE(bc_json_key_value, content_parameter);
		};

		BC_JSON_STRUCTURE(_bc_content_stream)
		{
			BC_JSON_VALUE(bc_string_frame, stream_name);
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

		bc_content_stream_manager::~bc_content_stream_manager()
		{
		}

		void bc_content_stream_manager::read_stream_file(const bcECHAR* p_json_file_path)
		{
			bc_file_stream l_json_file;
			bc_string_frame l_buffer;

			if(!l_json_file.open_read(p_json_file_path))
			{
				bc_string_frame l_msg = "Error in reading content stream file: ";
				l_msg += bc_to_exclusive_string(p_json_file_path).c_str();

				throw bc_io_exception(l_msg.c_str());
			}

			bc_string_frame l_line;
			while (core::bc_get_line(l_json_file, l_line))
			{
				l_buffer.append(l_line);
			}

			bc_json_document< _bc_content_stream_json > l_content_stream;
			l_content_stream.load(l_buffer.c_str());

			for (bc_json_object< _bc_content_stream >& l_stream : *l_content_stream->m_streams)
			{
				string_hash::result_type l_stream_hash = string_hash()(l_stream->m_stream_name->c_str());
				bc_vector_program< _bc_content_stream_file > l_stream_files;

				l_stream_files.reserve(l_stream->m_stream_content->size());

				for (bc_json_object< _bc_content_stream_content >& l_stream_content : *l_stream->m_stream_content)
				{
					_bc_content_stream_file l_stream_file
					{
						bc_string_program(l_stream_content->m_content_title->c_str()),
						bc_string_program(l_stream_content->m_content_name->c_str()),
						bc_estring_program(bc_to_estring_program(*l_stream_content->m_content_file)),
					};

					auto* l_content_params = &*l_stream_content->m_content_parameter;

					std::for_each
					(
						std::begin(l_content_params->m_key_values),
						std::end(l_content_params->m_key_values),
						[&l_stream_file](bc_json_key_value::key_value_array_t::value_type& p_parameter)
						{
							l_stream_file.m_parameters.add_value(p_parameter.first.c_str(), std::move(p_parameter.second));
						}
					);

					l_stream_files.push_back(std::move(l_stream_file));
				}

				m_streams.insert(streams_map_type::value_type(l_stream_hash, std::move(l_stream_files)));
			}
		}

		void bc_content_stream_manager::load_content_stream(bc_alloc_type p_alloc_type, const bcCHAR* p_stream_name)
		{
			const auto l_hash = string_hash()(p_stream_name);
			auto l_stream_entry = m_streams.find(l_hash);

			if(l_stream_entry == std::end(m_streams))
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
					return bc_list_frame<contents_map_type::value_type>();
				},
				[this, p_alloc_type](bc_list_frame<contents_map_type::value_type>& p_local, _bc_content_stream_file& l_content_file)
				{
					auto l_content_hash = string_hash()(l_content_file.m_title.c_str());
					contents_map_type::iterator l_content_entry;
					contents_map_type::iterator l_content_end;

					{
						core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_lock_guard(m_contents_mutex);

						l_content_entry = m_contents.find(l_content_hash);
						l_content_end = std::end(m_contents);
					}

					// Content already loaded from another stream
					if (l_content_entry != l_content_end)
					{
						core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_lock_guard(m_contents_mutex);

						// Make a copy of content pointer to increase it's reference counter
						l_content_entry->second.push_back(*std::begin(l_content_entry->second));
						return;
					}

					const auto l_loader_hash = string_hash()(l_content_file.m_name.c_str());
					auto l_loader_entry = m_content_types.find(l_loader_hash);

					if(l_loader_entry == std::end(m_content_types))
					{
						throw bc_key_not_found_exception(("There isn't any registered loader for " + l_content_file.m_name).c_str());
					}

					bc_content_loader_parameter l_loader_parameter = l_content_file.m_parameters;

					bc_icontent_ptr l_content = l_loader_entry->second
					(
						p_alloc_type,
						l_content_file.m_file.c_str(),
						std::move(l_loader_parameter)
					);

					contents_map_type::value_type l_new_content_entry = contents_map_type::value_type(l_content_hash, contents_map_type::value_type::second_type());
					l_new_content_entry.second.push_back(std::move(l_content));

					p_local.push_back(std::move(l_new_content_entry));
				},
				[this](bc_list_frame<contents_map_type::value_type>& p_locals)
				{
					{
						core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_lock_guard(m_contents_mutex);

						for (auto& l_content_entry : p_locals)
						{
							m_contents.insert(std::move(l_content_entry));
						}
					}
				}
			);
		}

		bc_task< void > bc_content_stream_manager::load_content_stream_async(bc_alloc_type p_alloc_type, const bcCHAR* p_stream_name)
		{
			return bc_concurrency::start_task< void >([this, p_alloc_type, p_stream_name]()
				{
					this->load_content_stream(p_alloc_type, p_stream_name);
				});
		}

		void bc_content_stream_manager::unload_content_stream(const bcCHAR* p_stream_name)
		{
			const auto l_hash = string_hash()(p_stream_name);
			auto l_stream_entry = m_streams.find(l_hash);

			if (l_stream_entry == std::end(m_streams))
			{
				bc_string_frame l_msg = "Invalid content stream name: ";
				l_msg += p_stream_name;

				throw bc_invalid_argument_exception(l_msg.c_str());
			}

			auto& l_contents = l_stream_entry->second;
			for (_bc_content_stream_file& l_content_file : l_contents)
			{
				// TODO remove content pointer to decrease reference count or explicitly unload via content manager

				auto l_content_hash = string_hash()(l_content_file.m_title.c_str());
				contents_map_type::iterator l_content_entry;
				contents_map_type::iterator l_content_end;

				{
					core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_lock_guard(m_contents_mutex);

					l_content_entry = m_contents.find(l_content_hash);
					l_content_end = std::end(m_contents);
				}

				if (l_content_entry != l_content_end)
				{
					bcAssert(l_content_entry->second.size() > 0);

					l_content_entry->second.pop_back();

					// Remove content entry if it's references reach zero
					if (l_content_entry->second.size() == 0)
					{
						{
							core_platform::bc_lock_guard< core_platform::bc_shared_mutex > l_lock_guard(m_contents_mutex);

							m_contents.erase(l_content_entry);
						}
					}
				}
			}
		}

		bc_icontent_ptr bc_content_stream_manager::find_content(const bcCHAR* p_content_name) const
		{
			const auto l_content_hash = string_hash()(p_content_name);
			contents_map_type::const_iterator l_content_entry;
			contents_map_type::const_iterator l_content_end;

			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_lock_guard(m_contents_mutex);
				
				l_content_entry = m_contents.find(l_content_hash);
				l_content_end = std::end(m_contents);
			}

			if (l_content_entry != l_content_end)
			{
				bcAssert(l_content_entry->second.size() > 0);

				return *std::begin(l_content_entry->second);
			}

			return nullptr;
		}

		bc_icontent_ptr bc_content_stream_manager::find_content_throw(const bcCHAR* p_content_name) const
		{
			auto l_content = find_content(p_content_name);

			if(!l_content)
			{
				throw bc_key_not_found_exception(std::string("There is no content named ") + p_content_name);
			}

			return l_content;
		}
	}
}