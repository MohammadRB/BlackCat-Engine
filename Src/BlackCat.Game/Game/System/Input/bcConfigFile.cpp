// [12/20/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcString.h"
#include "Core/File/bcPath.h"
#include "Game/System/Input/bcConfigFile.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		bc_config_file::bc_config_file(const bcECHAR* p_content_path, const bcECHAR* p_config_name)
		{
			const auto l_config_path = core::bc_path(p_content_path).combine(core::bc_path(p_config_name));
			const bool l_config_exist = l_config_path.exist();
			
			if(!m_stream.open
			(
				l_config_path.get_string().c_str(),
				core_platform::bc_file_mode::open_create, 
				core_platform::bc_file_access::read_write, 
				core_platform::bc_file_sharing::read
			))
			{
				const auto l_error_message = "Cannot open config file: " + core::bc_to_string_frame(l_config_path.get_string().c_str());
				throw bc_io_exception(l_error_message.c_str());
			}

			core::bc_string_frame l_file_content;
			if(l_config_exist && m_stream.length() > 0)
			{
				core::bc_read_all_lines(m_stream, l_file_content);
			}
			else
			{
				l_file_content = "{\"key_values\":{}}";
			}

			m_stream.close();
			m_json = core::bc_make_unique<core::bc_json_document< bc_config_layout >>();
			m_json->load(l_file_content.c_str());
		}

		bc_config_file& bc_config_file::add_or_update_config_key(const bcCHAR* p_key, core::bc_any p_value) noexcept
		{
			auto& l_key_values = *(*m_json)->m_key_values;
			auto l_entry = l_key_values.find(p_key);
			
			if(l_entry == std::end(l_key_values))
			{
				l_entry = l_key_values.add(std::make_pair(core::bc_string(p_key), std::move(p_value)));
			}
			else
			{
				l_entry->second = std::move(p_value);
			}
			
			return *this;
		}

		bc_config_file& bc_config_file::read_config_key(const bcCHAR* p_key, core::bc_any& p_value) noexcept
		{
			bool l_succeeded;
			return read_config_key(p_key, p_value, l_succeeded);
		}

		bc_config_file& bc_config_file::read_config_key(const bcCHAR* p_key, core::bc_any& p_value, bool& p_succeeded) noexcept
		{
			auto& l_key_values = *(*m_json)->m_key_values;
			const auto l_entry = l_key_values.find(p_key);

			if(l_entry == std::end(l_key_values))
			{
				p_succeeded = false;
				return *this;
			}

			p_value = l_entry->second;
			p_succeeded = true;
			return *this;
		}

		void bc_config_file::flush_changes()
		{
			const auto l_json_file_buffer = m_json->write();

			m_stream.open
			(
				m_stream.get_path().c_str(),
				core_platform::bc_file_mode::create_overwrite,
				core_platform::bc_file_access::write,
				core_platform::bc_file_sharing::read
			);
			m_stream.write
			(
				reinterpret_cast<const bcBYTE*>(l_json_file_buffer.data()),
				l_json_file_buffer.size() * sizeof(decltype(l_json_file_buffer)::value_type)
			);
			m_stream.close();
		}
	}
}