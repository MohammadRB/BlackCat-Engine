// [03/23/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Container/bcArray.h"

namespace black_cat
{
	namespace core
	{
		inline void get_included_types(bc_log_type p_type, bc_array<bc_log_type, 3>& p_result)
		{
			p_result.fill(static_cast<bc_log_type>(0));

			bc_enum::has(p_type, bc_log_type::error) ? p_result[0] = bc_log_type::error : void(0);
			bc_enum::has(p_type, bc_log_type::info) ? p_result[1] = (bc_log_type::info) : void(0);
			bc_enum::has(p_type, bc_log_type::debug) ? p_result[2] = (bc_log_type::debug) : void(0);
		}

		void bc_logger::log(bc_log_type p_type, const bcECHAR* p_log)
		{
			bc_array<bc_log_type, 3> l_types;

			get_included_types(p_type, l_types);

			auto l_listeners_end = std::end(m_listeners);
			for (auto l_type : l_types)
			{
				if (static_cast< std::underlying_type<bc_log_type>::type >(l_type) == 0)
					continue;

				auto l_ite = m_listeners.find(static_cast<key_type>(l_type));

				if (l_ite != l_listeners_end)
				{
					for (auto& l_listener : l_ite->second)
					{
						l_listener->on_log(p_type, p_log);
					}
				}
			}
		}

		void bc_logger::log(bc_log_type p_type, bc_estring_frame p_log)
		{
			bc_array<bc_log_type, 3> l_types;

			get_included_types(p_type, l_types);

			auto l_listeners_end = std::end(m_listeners);
			for (auto l_type : l_types)
			{
				if(static_cast< std::underlying_type<bc_log_type>::type >(l_type) == 0)
					continue;

				auto l_ite = m_listeners.find(static_cast<key_type>(l_type));

				if (l_ite != l_listeners_end)
				{
					for(auto& l_listener : l_ite->second)
					{
						l_listener->on_log(p_type, std::move(p_log));
					}
				}
			}
		}

		void bc_logger::register_listener(bc_log_type p_type, bc_unique_ptr<bc_ilog_listener> p_listener)
		{
			bc_array<bc_log_type, 3> l_types;

			get_included_types(p_type, l_types);
			
			auto l_listeners_end = std::end(m_listeners);
			for(auto l_type : l_types)
			{
				if (static_cast< std::underlying_type<bc_log_type>::type >(l_type) == 0)
					continue;

				auto l_ite = m_listeners.find(static_cast<key_type>(l_type));

				if(l_ite != l_listeners_end)
				{
					l_ite->second.push_back(std::move(p_listener));
				}
				else
				{
					bc_vector< bc_unique_ptr<bc_ilog_listener> > l_listeners;
					l_listeners.push_back(std::move(p_listener));

					m_listeners.insert(map_type::value_type
						(
							static_cast< key_type >(l_type), 
							std::move(l_listeners)
						));
				}
			}
		}
	}
}
