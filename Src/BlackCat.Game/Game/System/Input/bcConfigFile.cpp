// [12/20/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/File/bcPath.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcJsonParse.h"
#include "Game/System/Input/bcConfigFile.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		bc_config_file::bc_config_file()
			: m_key_values(nullptr)
		{
		}

		core::bc_estring_view bc_config_file::get_path() const noexcept
		{
			return m_path;
		}

		bc_config_file& bc_config_file::add_if_not_exist_config_key(core::bc_string_view p_key, core::bc_any p_value) noexcept
		{
			auto& l_key_values = *m_key_values;
			const auto l_entry = l_key_values.find(p_key);

			if (l_entry == std::end(l_key_values))
			{
				l_key_values.add(core::bc_string(p_key), std::move(p_value));
			}

			return *this;
		}

		bc_config_file& bc_config_file::add_or_update_config_key(core::bc_string_view p_key, core::bc_any p_value) noexcept
		{
			auto& l_key_values = *m_key_values;
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

		bc_config_file& bc_config_file::add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector2f& p_value) noexcept
		{
			json_parse::bc_write(*m_key_values, core::bc_string(p_key), p_value);
			return *this;
		}

		bc_config_file& bc_config_file::add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector2i& p_value) noexcept
		{
			json_parse::bc_write(*m_key_values, core::bc_string(p_key), p_value);
			return *this;
		}

		bc_config_file& bc_config_file::add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector3f& p_value) noexcept
		{
			json_parse::bc_write(*m_key_values, core::bc_string(p_key), p_value);
			return *this;
		}

		bc_config_file& bc_config_file::add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector3i& p_value) noexcept
		{
			json_parse::bc_write(*m_key_values, core::bc_string(p_key), p_value);
			return *this;
		}

		bc_config_file& bc_config_file::add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector4f& p_value) noexcept
		{
			json_parse::bc_write(*m_key_values, core::bc_string(p_key), p_value);
			return *this;
		}

		bc_config_file& bc_config_file::add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector4i& p_value) noexcept
		{
			json_parse::bc_write(*m_key_values, core::bc_string(p_key), p_value);
			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_any& p_value) const noexcept
		{
			bool l_succeeded;
			return read_config_key(p_key, p_value, l_succeeded);
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_any& p_value, bool& p_succeeded) const noexcept
		{
			auto& l_key_values = *m_key_values;
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

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector2f& p_value) const
		{
			const bool l_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			if(!l_succeeded)
			{
				throw bc_invalid_operation_exception("Cannot read config key of type vector2");
			}

			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector2f& p_value, bool& p_succeeded) const noexcept
		{
			p_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector2i& p_value) const
		{
			const bool l_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			if (!l_succeeded)
			{
				throw bc_invalid_operation_exception("Cannot read config key of type vector2");
			}

			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector2i& p_value, bool& p_succeeded) const noexcept
		{
			p_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector3f& p_value) const
		{
			const bool l_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			if (!l_succeeded)
			{
				throw bc_invalid_operation_exception("Cannot read config key of type vector3");
			}

			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector3f& p_value, bool& p_succeeded) const noexcept
		{
			p_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector3i& p_value) const
		{
			const bool l_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			if (!l_succeeded)
			{
				throw bc_invalid_operation_exception("Cannot read config key of type vector3");
			}

			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector3i& p_value, bool& p_succeeded) const noexcept
		{
			p_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector4f& p_value) const
		{
			const bool l_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			if (!l_succeeded)
			{
				throw bc_invalid_operation_exception("Cannot read config key of type vector4");
			}

			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector4f& p_value, bool& p_succeeded) const noexcept
		{
			p_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector4i& p_value) const
		{
			const bool l_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			if (!l_succeeded)
			{
				throw bc_invalid_operation_exception("Cannot read config key of type vector4");
			}

			return *this;
		}

		const bc_config_file& bc_config_file::read_config_key(core::bc_string_view p_key, core::bc_vector4i& p_value, bool& p_succeeded) const noexcept
		{
			p_succeeded = json_parse::bc_load(*m_key_values, p_key, p_value);
			return *this;
		}

		void bc_config_file::reload()
		{
			load(core::bc_path(m_path));
		}

		void bc_config_file::flush_changes()
		{
			std::sort
			(
				std::begin(*m_key_values), 
				std::end(*m_key_values), 
				[](const core::bc_json_key_value::value_type& p_entry1, const core::bc_json_key_value::value_type& p_entry2)
				{
					return p_entry1.first < p_entry2.first;
				}
			);

			const auto l_json_file_buffer = write_json();

			m_stream.open
			(
				m_stream.get_path().c_str(),
				platform::bc_file_mode::create_overwrite,
				platform::bc_file_access::write,
				platform::bc_file_sharing::read
			);
			m_stream.write
			(
				reinterpret_cast<const bcBYTE*>(l_json_file_buffer.data()),
				l_json_file_buffer.size() * sizeof(decltype(l_json_file_buffer)::value_type)
			);
			m_stream.close();
		}

		void bc_config_file::load(const core::bc_path& p_path)
		{
			m_path = p_path.get_string();

			if (!m_stream.open
			(
				m_path.c_str(),
				platform::bc_file_mode::open_create,
				platform::bc_file_access::read_write,
				platform::bc_file_sharing::read
			))
			{
				const auto l_error_message = "Cannot open config file: " + core::bc_to_string_frame(p_path.get_string().c_str());
				throw bc_io_exception(l_error_message.c_str());
			}

			core::bc_string_frame l_file_content;
			if (p_path.exist() && m_stream.length() > 0)
			{
				core::bc_read_all_lines(m_stream, l_file_content);
			}
			else
			{
				l_file_content = "{}";
			}

			m_stream.close();
			m_key_values = load_json(l_file_content.c_str());
		}
	}
}