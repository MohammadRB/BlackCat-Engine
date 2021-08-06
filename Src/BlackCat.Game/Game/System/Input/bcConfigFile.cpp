// [12/20/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/File/bcPath.h"
#include "Core/File/bcJsonDocument.h"
#include "Game/System/Input/bcConfigFile.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		bc_config_file::bc_config_file()
			: m_json_key_value(nullptr)
		{
		}

		bc_config_file& bc_config_file::add_or_update_config_key(const bcCHAR* p_key, core::bc_any p_value) noexcept
		{
			auto& l_key_values = *m_json_key_value;
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

		bc_config_file& bc_config_file::add_or_update_config_key(const bcCHAR* p_key, const core::bc_vector2f& p_value) noexcept
		{
			core::bc_vector<core::bc_any> l_array(2);
			l_array[0].set_value(p_value.x);
			l_array[1].set_value(p_value.y);

			return add_or_update_config_key(p_key, core::bc_any(std::move(l_array)));
		}

		bc_config_file& bc_config_file::add_or_update_config_key(const bcCHAR* p_key, const core::bc_vector3f& p_value) noexcept
		{
			core::bc_vector<core::bc_any> l_array(3);
			l_array[0].set_value(p_value.x);
			l_array[1].set_value(p_value.y);
			l_array[2].set_value(p_value.z);

			return add_or_update_config_key(p_key, core::bc_any(std::move(l_array)));
		}

		bc_config_file& bc_config_file::add_or_update_config_key(const bcCHAR* p_key, const core::bc_vector4f& p_value) noexcept
		{
			core::bc_vector<core::bc_any> l_array(4);
			l_array[0].set_value(p_value.x);
			l_array[1].set_value(p_value.y);
			l_array[2].set_value(p_value.z);
			l_array[3].set_value(p_value.w);

			return add_or_update_config_key(p_key, core::bc_any(std::move(l_array)));
		}

		bc_config_file& bc_config_file::read_config_key(const bcCHAR* p_key, core::bc_any& p_value) noexcept
		{
			bool l_succeeded;
			return read_config_key(p_key, p_value, l_succeeded);
		}

		bc_config_file& bc_config_file::read_config_key(const bcCHAR* p_key, core::bc_any& p_value, bool& p_succeeded) noexcept
		{
			auto& l_key_values = *m_json_key_value;
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

		bc_config_file& bc_config_file::read_config_key(const bcCHAR* p_key, core::bc_vector2f& p_value)
		{
			core::bc_any l_value;
			read_config_key(p_key, l_value);

			auto& l_array = l_value.as_throw<core::bc_vector<core::bc_any>>();

			p_value.x = l_array[0].as_throw<bcFLOAT>();
			p_value.y = l_array[1].as_throw<bcFLOAT>();
			
			return *this;
		}

		bc_config_file& bc_config_file::read_config_key(const bcCHAR* p_key, core::bc_vector2f& p_value, bool& p_succeeded) noexcept
		{
			core::bc_any l_value;
			read_config_key(p_key, l_value);

			auto* l_array = l_value.as<core::bc_vector<core::bc_any>>();
			if(!l_array || l_array->size() < 2)
			{
				p_succeeded = false;
				return *this;
			}

			auto* l_x = (*l_array)[0].as<bcFLOAT>();
			auto* l_y = (*l_array)[1].as<bcFLOAT>();
			if(!l_x || !l_y)
			{
				p_succeeded = false;
				return *this;
			}
			
			p_value.x = *l_x;
			p_value.y = *l_y;

			p_succeeded = true;
			return *this;
		}

		bc_config_file& bc_config_file::read_config_key(const bcCHAR* p_key, core::bc_vector3f& p_value)
		{
			core::bc_any l_value;
			read_config_key(p_key, l_value);

			auto& l_array = l_value.as_throw<core::bc_vector<core::bc_any>>();

			p_value.x = l_array[0].as_throw<bcFLOAT>();
			p_value.y = l_array[1].as_throw<bcFLOAT>();
			p_value.z = l_array[3].as_throw<bcFLOAT>();

			return *this;
		}

		bc_config_file& bc_config_file::read_config_key(const bcCHAR* p_key, core::bc_vector3f& p_value, bool& p_succeeded) noexcept
		{
			core::bc_any l_value;
			read_config_key(p_key, l_value);

			auto* l_array = l_value.as<core::bc_vector<core::bc_any>>();
			if (!l_array || l_array->size() < 3)
			{
				p_succeeded = false;
				return *this;
			}

			auto* l_x = (*l_array)[0].as<bcFLOAT>();
			auto* l_y = (*l_array)[1].as<bcFLOAT>();
			auto* l_z = (*l_array)[2].as<bcFLOAT>();
			if (!l_x || !l_y || !l_z)
			{
				p_succeeded = false;
				return *this;
			}

			p_value.x = *l_x;
			p_value.y = *l_y;
			p_value.z = *l_z;

			p_succeeded = true;
			return *this;
		}

		bc_config_file& bc_config_file::read_config_key(const bcCHAR* p_key, core::bc_vector4f& p_value)
		{
			core::bc_any l_value;
			read_config_key(p_key, l_value);

			auto& l_array = l_value.as_throw<core::bc_vector<core::bc_any>>();

			p_value.x = l_array[0].as_throw<bcFLOAT>();
			p_value.y = l_array[1].as_throw<bcFLOAT>();
			p_value.z = l_array[3].as_throw<bcFLOAT>();
			p_value.w = l_array[4].as_throw<bcFLOAT>();

			return *this;
		}

		bc_config_file& bc_config_file::read_config_key(const bcCHAR* p_key, core::bc_vector4f& p_value, bool& p_succeeded) noexcept
		{
			core::bc_any l_value;
			read_config_key(p_key, l_value);

			auto* l_array = l_value.as<core::bc_vector<core::bc_any>>();
			if (!l_array || l_array->size() < 4)
			{
				p_succeeded = false;
				return *this;
			}

			auto* l_x = (*l_array)[0].as<bcFLOAT>();
			auto* l_y = (*l_array)[1].as<bcFLOAT>();
			auto* l_z = (*l_array)[2].as<bcFLOAT>();
			auto* l_w = (*l_array)[3].as<bcFLOAT>();
			if (!l_x || !l_y || !l_z || !l_w)
			{
				p_succeeded = false;
				return *this;
			}

			p_value.x = *l_x;
			p_value.y = *l_y;
			p_value.z = *l_z;
			p_value.w = *l_w;

			p_succeeded = true;
			return *this;
		}

		void bc_config_file::flush_changes()
		{
			const auto l_json_file_buffer = write_json();

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

		void bc_config_file::load(const bcECHAR* p_content_path, const bcECHAR* p_config_name)
		{
			const auto l_config_path = core::bc_path(p_content_path).combine(core::bc_path(p_config_name));
			const bool l_config_exist = l_config_path.exist();

			if (!m_stream.open
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
			if (l_config_exist && m_stream.length() > 0)
			{
				core::bc_read_all_lines(m_stream, l_file_content);
			}
			else
			{
				l_file_content = "{}";
			}

			m_stream.close();
			m_json_key_value = load_json(l_file_content.c_str());
		}
	}
}