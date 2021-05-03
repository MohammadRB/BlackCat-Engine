// [03/23/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Container/bcArray.h"

namespace black_cat
{
	namespace core
	{
		void bc_logger::log(bc_log_type p_types, const bcECHAR* p_log)
		{
			bc_array<bc_log_type, 4> l_types{ { bc_log_type::info, bc_log_type::debug, bc_log_type::warning, bc_log_type::error } };

			for (auto l_type : l_types)
			{
				if (!bc_enum::has(p_types, l_type))
				{
					continue;
				}

				_log(l_type, p_log);
			}
		}

		void bc_logger::register_listener(bc_log_type p_types, bc_unique_ptr<bci_log_listener> p_listener)
		{
			_register_listener(p_types, _bc_log_listener_container(std::move(p_listener)));
		}

		void bc_logger::register_listener(bc_log_type p_types, bci_log_listener* p_listener)
		{
			_register_listener(p_types, _bc_log_listener_container(p_listener));
		}

		void bc_logger::unregister_listener(bci_log_listener* p_listener)
		{
			for (auto& l_entry : m_listeners)
			{
				auto l_position = std::find_if(std::cbegin(l_entry), std::cend(l_entry), [p_listener](const _bc_log_listener_container& p_list)
				{
					if(p_list.m_raw_pointer && p_list.m_raw_pointer == p_listener)
					{
						return true;
					}

					return false;
				});

				if(l_position != std::cend(l_entry))
				{
					l_entry.erase(l_position);
				}
			}
		}

		void bc_logger::_log(bc_log_type p_types, const bcECHAR* p_log)
		{
			const auto l_entry_index = std::log2(static_cast<map_type::size_type>(p_types));
			auto& l_entry = m_listeners.at(l_entry_index);

			for (auto& l_listener : l_entry)
			{
				l_listener->on_log(p_types, p_log);
			}
		}

		void bc_logger::_register_listener(bc_log_type p_types, _bc_log_listener_container&& p_listener)
		{
			bc_array<bc_log_type, 4> l_types{ { bc_log_type::info, bc_log_type::debug, bc_log_type::warning, bc_log_type::error } };

			for (auto l_type : l_types)
			{
				if (!bc_enum::has(p_types, l_type))
				{
					continue;
				}

				const auto l_entry_index = std::log2(static_cast<map_type::size_type>(l_type));
				m_listeners.at(l_entry_index).push_back(p_listener);
			}
		}
	}
}