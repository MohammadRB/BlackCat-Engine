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
			error = 1,
			info = 2,
			debug = 4,
		};

		class bc_ilog_listener
		{
		public:
			virtual ~bc_ilog_listener()
			{
			}

			virtual void on_log(bc_log_type p_type, const bcECHAR* p_log) = 0;

			// Default implementation call other overload
			virtual void on_log(bc_log_type p_type, bc_estring_frame p_log)
			{
				on_log(p_type, p_log.c_str());
			}
		};

		class BC_CORE_DLL bc_logger : public bc_iservice
		{
		private:
			using key_type = std::underlying_type<bc_log_type>::type;
			using map_type = bc_unordered_map_program<key_type, bc_vector<bc_unique_ptr<bc_ilog_listener>>>;

		public:
			bc_logger() = default;

			bc_logger(bc_logger&&) = default;

			~bc_logger() = default;

			bc_logger& operator=(bc_logger&&) = default;

			// Use binary OR to pass multiple type
			void log(bc_log_type p_type, const bcECHAR* p_log);

			void log(bc_log_type p_type, bc_estring_frame p_log);

			// Use binary OR to pass multiple type
			void register_listener(bc_log_type p_type, bc_unique_ptr<bc_ilog_listener> p_listener);

			static const bcCHAR* service_name()
			{
				return g_srv_logger;
			}

		protected:

		private:
			map_type m_listeners;
		};	
	}
}
