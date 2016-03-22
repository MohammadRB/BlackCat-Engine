// [03/20/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/Utility/bcServiceManager.h"

namespace black_cat
{
	namespace core
	{
		template BC_COREDLL_EXP class bc_singleton< bc_service_manager() >;

		_bc_service_container::_bc_service_container(bc_service_ptr< bc_iservice >&& p_service) 
			: m_service(std::move(p_service))
		{
		}

		_bc_service_container::_bc_service_container(_bc_service_container&& p_other) 
			: m_service(std::move(p_other.m_service))
		{
			p_other.m_service = nullptr;
		}

		_bc_service_container& _bc_service_container::operator=(_bc_service_container&& p_other)
		{
			m_service = std::move(p_other.m_service);

			return *this;
		}

		bc_service_manager::bc_service_manager()
		{
		}

		bc_service_manager::~bc_service_manager()
		{
		}

		void bc_service_manager::_initialize()
		{
		}

		void bc_service_manager::_destroy()
		{
			m_services.clear();
		}
	}
}