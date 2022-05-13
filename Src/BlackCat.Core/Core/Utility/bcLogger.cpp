// [03/23/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcEnumOperand.h"

namespace black_cat
{
	namespace core
	{
		bc_logger::bc_logger()
			: m_enabled_logs(bc_log_type::all),
			m_logs_ptr(0)
		{
			m_logs.resize(25, std::make_pair(bc_log_type::info, bc_estring()));
		}

		void bc_logger::set_enabled_log_types(bc_log_type p_types) noexcept
		{
			m_enabled_logs = core::bc_enum::none<bc_log_type>();

			for (const auto l_type : { bc_log_type::info, bc_log_type::debug, bc_log_type::warning, bc_log_type::error })
			{
				if (!bc_enum::has(p_types, l_type))
				{
					continue;
				}

				m_enabled_logs = bc_enum::set(m_enabled_logs, l_type, true);
			}
		}

		void bc_logger::log(bc_log_type p_types, const bcECHAR* p_log)
		{
			const bool l_file_modifier = bc_enum::has(p_types, bc_log_type::only_file);

			for (const auto l_type : { bc_log_type::info, bc_log_type::debug, bc_log_type::warning, bc_log_type::error })
			{
				if (!bc_enum::has(m_enabled_logs, l_type) || !bc_enum::has(p_types, l_type))
				{
					continue;
				}

				_log(l_type, p_log, l_file_modifier);
			}
		}

		bc_logger_output_stream& bc_logger::log(bc_log_type p_types)
		{
			auto& l_stream = m_log_stream.get();
			return l_stream.start_log(*this, p_types);
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

		void bc_logger::_log(bc_log_type p_type, const bcECHAR* p_log, bool p_file_modifier)
		{
			if(!p_file_modifier)
			{
				auto l_logs_ptr = m_logs_ptr.load(platform::bc_memory_order::relaxed);
				auto l_new_logs_ptr = l_logs_ptr;
				do
				{
					l_new_logs_ptr = (l_logs_ptr + 1) % m_logs.size();
				} while (m_logs_ptr.compare_exchange_weak(&l_logs_ptr, l_new_logs_ptr, platform::bc_memory_order::relaxed));

				m_logs[l_logs_ptr].first = p_type;
				m_logs[l_logs_ptr].second = p_log;
			}
			
			const auto l_entry_index = static_cast<key_type>(std::log2(static_cast<listener_map_type::size_type>(p_type)));
			auto& l_entry = m_listeners.at(l_entry_index);

			{
				platform::bc_shared_mutex_shared_guard l_listeners_lock(m_listener_mutex);

				for (const auto& l_listener : l_entry)
				{
					if (p_file_modifier && !l_listener->is_file())
					{
						continue;
					}

					l_listener->on_log(p_type, p_log);
				}
			}
		}

		void bc_logger::_register_listener(bc_log_type p_types, _bc_log_listener_container p_listener)
		{
			auto* l_listener = *p_listener;

			{
				platform::bc_shared_mutex_guard l_listeners_lock(m_listener_mutex);

				for (auto l_type : { bc_log_type::info, bc_log_type::debug, bc_log_type::warning, bc_log_type::error })
				{
					if (!bc_enum::has(p_types, l_type))
					{
						continue;
					}

					const auto l_entry_index = static_cast<key_type>(std::log2(static_cast<listener_map_type::size_type>(l_type)));
					m_listeners.at(l_entry_index).push_back(p_listener);
				}
			}

			_replicate_last_logs(p_types, l_listener);
		}

		void bc_logger::_replicate_last_logs(bc_log_type p_types, bci_log_listener* p_listener)
		{
			{
				platform::bc_shared_mutex_shared_guard l_listeners_lock(m_listener_mutex);

				auto l_logs_ptr = m_logs_ptr.load(platform::bc_memory_order::relaxed) + 1; // Start from last logs
				for (auto l_ite = 0U; l_ite != m_logs.size(); ++l_ite, l_logs_ptr = (l_logs_ptr + 1) % m_logs.size())
				{
					auto& [l_type, l_log] = m_logs[l_logs_ptr];					
					if (l_log.empty())
					{
						continue;
					}

					if(bc_enum::has(p_types, l_type))
					{
						p_listener->on_log(l_type, l_log.c_str());
					}
				}
			}
		}
	}
}