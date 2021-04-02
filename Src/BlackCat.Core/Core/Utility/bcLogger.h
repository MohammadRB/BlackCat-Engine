// [03/22/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/CorePlatformPCH.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcArray.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Memory/bcPtr.h"

namespace black_cat
{
	namespace core
	{
		enum class bc_log_type : bcUBYTE
		{
			info = 1,
			debug = 2,
			error = 4,
		};

		/**
		 * \brief Interface for log listeners. Calls to `on_log` method can be called from different threads so
		 * clients must take care of threads in their implementations.
		 */
		class bci_log_listener
		{
		public:
			virtual ~bci_log_listener() = default;

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

			explicit _bc_log_listener_container(bc_unique_ptr< bci_log_listener >&& p_smart_pointer) noexcept
				: m_raw_pointer(nullptr),
				m_smart_pointer(std::move(p_smart_pointer))
			{
			}

			_bc_log_listener_container(const _bc_log_listener_container& p_other) = default;

			_bc_log_listener_container(_bc_log_listener_container&& p_other) = default;

			~_bc_log_listener_container() = default;

			_bc_log_listener_container& operator=(const _bc_log_listener_container& p_other) = default;

			_bc_log_listener_container& operator=(_bc_log_listener_container&& p_other) = default;

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
			using key_type = std::underlying_type< bc_log_type >::type;
			using map_type = bc_array< bc_vector< _bc_log_listener_container >, 3 >;

		public:
			bc_logger() = default;

			bc_logger(bc_logger&&) noexcept = delete;

			~bc_logger() = default;

			bc_logger& operator=(bc_logger&&) noexcept = delete;

			/**
			 * \brief 
			 * Use binary OR to pass multiple type.
			 * \param p_types
			 * \param p_log 
			 */
			void log(bc_log_type p_types, const bcECHAR* p_log);

			void log_info(const bcECHAR* p_log)
			{
				_log(bc_log_type::info, p_log);
			}

			void log_debug(const bcECHAR* p_log)
			{
#ifdef BC_DEBUG
				_log(bc_log_type::debug, p_log);
#endif
			}

			void log_error(const bcECHAR* p_log)
			{
				_log(bc_log_type::error, p_log);
			}

			/**
			 * \brief 
			 * Register and take the ownership of listener.
			 * Use binary OR to pass multiple type.
			 * \param p_types
			 * \param p_listener 
			 */
			void register_listener(bc_log_type p_types, bc_unique_ptr<bci_log_listener> p_listener);

			/**
			 * \brief 
			 * Register listener.
			 * Use binary OR to pass multiple type.
			 * \param p_types
			 * \param p_listener 
			 */
			void register_listener(bc_log_type p_types, bci_log_listener* p_listener);

			void unregister_listener(bci_log_listener* p_listener);

		private:
			void _log(bc_log_type p_type, const bcECHAR* p_log);

			void _register_listener(bc_log_type p_types, _bc_log_listener_container&& p_listener);

			map_type m_listeners;
		};

		inline void bc_log(bc_log_type p_types, const bcECHAR* p_log)
		{
			bc_get_service<bc_logger>()->log(p_types, p_log);
		}
	}
}