// [03/22/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
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
		 * \brief Interface for log listeners. Calls to `on_log` method can be called from different threads
		 * so clients must take care of thread saftly in their implementations.
		 */
		class bc_ilog_listener
		{
		public:
			virtual ~bc_ilog_listener() = default;

			virtual void on_log(bc_log_type p_type, const bcECHAR* p_log) = 0;
		};

		class _bc_log_listener_container
		{
		public:
			_bc_log_listener_container(bc_ilog_listener* p_raw_pointer) noexcept
				: m_raw_pointer(p_raw_pointer),
				m_smart_pointer(nullptr)
			{
			}

			_bc_log_listener_container(bc_unique_ptr< bc_ilog_listener >&& p_smart_pointer) noexcept
				: m_raw_pointer(nullptr),
				m_smart_pointer(std::move(p_smart_pointer))
			{
			}

			_bc_log_listener_container(_bc_log_listener_container&& p_other) = default;

			~_bc_log_listener_container() = default;

			_bc_log_listener_container& operator=(_bc_log_listener_container&& p_other) = default;

			bc_ilog_listener* operator->() const noexcept
			{
				return m_raw_pointer ? m_raw_pointer : m_smart_pointer.get();
			}

			bc_ilog_listener* m_raw_pointer;
			bc_unique_ptr<bc_ilog_listener> m_smart_pointer;
		};

		class BC_CORE_DLL bc_logger : public bc_iservice
		{
		private:
			using key_type = std::underlying_type< bc_log_type >::type;
			using map_type = bc_unordered_map_program< key_type, bc_vector< _bc_log_listener_container > >;

		public:
			bc_logger() = default;

			bc_logger(bc_logger&&) = default;

			~bc_logger() = default;

			bc_logger& operator=(bc_logger&&) = default;

			/**
			 * \brief 
			 * Use binary OR to pass multiple type.
			 * \param p_type 
			 * \param p_log 
			 */
			void log(bc_log_type p_type, const bcECHAR* p_log);

			/**
			 * \brief 
			 * Register and take the ownership of listener.
			 * Use binary OR to pass multiple type.
			 * \param p_type 
			 * \param p_listener 
			 */
			void register_listener(bc_log_type p_type, bc_unique_ptr<bc_ilog_listener> p_listener);

			/**
			 * \brief 
			 * Register listener.
			 * Use binary OR to pass multiple type.
			 * \param p_type 
			 * \param p_listener 
			 */
			void register_listener(bc_log_type p_type, bc_ilog_listener* p_listener);

			static const bcCHAR* service_name()
			{
				return g_srv_logger;
			}

		protected:

		private:
			void _register_listener(bc_log_type p_type, _bc_log_listener_container&& p_listener);

			map_type m_listeners;
		};	
	}
}
