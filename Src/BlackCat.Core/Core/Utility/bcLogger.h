// [03/22/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/CorePlatformPCH.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Concurrency/bcThreadLocal.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcArray.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcLoggerOutputStream.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"

namespace black_cat
{
	namespace core
	{
		/**
		 * \brief Interface for log listeners. Calls to 'on_log' method can be called from different threads so
		 * clients must take care of threads in their implementations.
		 */
		class bci_log_listener
		{
		public:
			virtual ~bci_log_listener() = default;

			virtual bool is_file()
			{
				return false;
			}
			
			virtual void on_log(bc_log_type p_type, const bcECHAR* p_log) = 0;
		};

		class _bc_log_listener_container
		{
		public:
			explicit _bc_log_listener_container(bci_log_listener* p_raw_pointer) noexcept
				: m_raw_pointer(p_raw_pointer),
				m_smart_pointer(nullptr)
			{
			}

			explicit _bc_log_listener_container(bc_unique_ptr<bci_log_listener>&& p_smart_pointer) noexcept
				: m_raw_pointer(nullptr),
				m_smart_pointer(std::move(p_smart_pointer))
			{
			}

			_bc_log_listener_container(const _bc_log_listener_container& p_other) = default;

			_bc_log_listener_container(_bc_log_listener_container&& p_other) = default;

			~_bc_log_listener_container() = default;

			_bc_log_listener_container& operator=(const _bc_log_listener_container& p_other) = default;

			_bc_log_listener_container& operator=(_bc_log_listener_container&& p_other) = default;

			bci_log_listener* operator*() const noexcept
			{
				return m_raw_pointer ? m_raw_pointer : m_smart_pointer.get();
			}
			
			bci_log_listener* operator->() const noexcept
			{
				return m_raw_pointer ? m_raw_pointer : m_smart_pointer.get();
			}

			bci_log_listener* m_raw_pointer;
			bc_shared_ptr<bci_log_listener> m_smart_pointer;
		};

		class BC_CORE_DLL bc_logger : public bci_service
		{
			BC_SERVICE(logger)

		private:
			using key_type = std::underlying_type<bc_log_type>::type;
			using listener_map_type = bc_array<bc_vector<_bc_log_listener_container>, 4>;

		public:
			bc_logger();

			bc_logger(bc_logger&&) noexcept = delete;

			~bc_logger() override = default;

			bc_logger& operator=(bc_logger&&) noexcept = delete;

			/**
			 * \brief Use binary OR to pass multiple type.
			 * \param p_types
			 * \param p_log 
			 */
			void log(bc_log_type p_types, const bcECHAR* p_log);

			void log_info(const bcECHAR* p_log)
			{
				log(bc_log_type::info, p_log);
			}

			void log_debug(const bcECHAR* p_log)
			{
				log(bc_log_type::debug, p_log);
			}

			void log_warning(const bcECHAR* p_log)
			{
				log(bc_log_type::warning, p_log);
			}
			
			void log_error(const bcECHAR* p_log)
			{
				log(bc_log_type::error, p_log);
			}

			/**
			 * \brief Get logger output stream for formatted logs
			 * \return 
			 */
			bc_logger_output_stream& log(bc_log_type p_types);

			bc_logger_output_stream& log_info()
			{
				return log(bc_log_type::info);
			}

			bc_logger_output_stream& log_debug()
			{
				return log(bc_log_type::debug);
			}

			bc_logger_output_stream& log_warning()
			{
				return log(bc_log_type::warning);
			}

			bc_logger_output_stream& log_error()
			{
				return log(bc_log_type::error);
			}
			
			/**
			 * \brief Register and take the ownership of listener. Use binary OR to pass multiple types.
			 * \param p_types
			 * \param p_listener 
			 */
			void register_listener(bc_log_type p_types, bc_unique_ptr<bci_log_listener> p_listener);

			/**
			 * \brief Register listener. Use binary OR to pass multiple types.
			 * \param p_types
			 * \param p_listener 
			 */
			void register_listener(bc_log_type p_types, bci_log_listener* p_listener);

			void unregister_listener(bci_log_listener* p_listener);

		private:
			void _log(bc_log_type p_type, const bcECHAR* p_log, bool p_file_modifier);

			void _register_listener(bc_log_type p_types, _bc_log_listener_container p_listener);

			void _replicate_last_logs(bc_log_type p_types, bci_log_listener* p_listener);

			listener_map_type m_listeners;
			bc_thread_local<bc_logger_output_stream> m_log_stream;

			core_platform::bc_mutex m_logs_mutex;
			bcSIZE m_logs_ptr;
			bc_vector<std::pair<bc_log_type, bc_estring>> m_logs;
		};

		inline void bc_log(bc_log_type p_types, const bcECHAR* p_log)
		{
			bc_get_service<bc_logger>()->log(p_types, p_log);
		}

		inline bc_logger_output_stream& bc_log(bc_log_type p_types)
		{
			return bc_get_service<bc_logger>()->log(p_types);
		}
	}
}