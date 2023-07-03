// [20/03/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/bcException.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Container/bcVector.h"

namespace black_cat
{
	namespace core
	{
		template BC_CORE_DLL class bc_singleton<bc_service_manager()>;

		bci_service::~bci_service() = default;

		void bci_service::update(const platform::bc_clock::update_param& p_clock)
		{
		}

		_bc_service_container::_bc_service_container(bcUINT32 p_hash, const bcCHAR* p_name, bcSIZE p_priority, bc_service_ptr< bci_service > p_service)
			: m_hash(p_hash),
			m_name(p_name),
			m_priority(p_priority),
			m_service(std::move(p_service))
		{
		}

		bc_service_manager::bc_service_manager() = default;

		bc_service_manager::~bc_service_manager() = default;

		void bc_service_manager::update(const platform::bc_clock::update_param& p_clock_update_param)
		{
			for (const auto* l_service : m_sorted_services)
			{
				l_service->m_service->update(p_clock_update_param);
			}
		}

		void bc_service_manager::_initialize()
		{
		}

		void bc_service_manager::_destroy()
		{
			/*for(auto l_ite = std::rbegin(m_sorted_services), l_end = std::rend(m_sorted_services); l_ite != l_end; ++l_ite)
			{
				(*l_ite)->m_service->destroy();
			}*/

			for (auto l_ite = std::rbegin(m_sorted_services), l_end = std::rend(m_sorted_services); l_ite != l_end; ++l_ite)
			{
				m_services.erase(m_services.find((*l_ite)->m_hash));
			}

			m_sorted_services.clear();
		}

		bci_service* bc_service_manager::_get_service(bcUINT32 p_service_hash)
		{
			bci_service* l_result = nullptr;
			const auto l_ite = m_services.find(p_service_hash);

			if (l_ite != std::end(m_services))
			{
				l_result = l_ite->second.m_service.get();
			}

			return l_result;
		}

		bci_service* bc_service_manager::_register_service(bcUINT32 p_hash, const bcCHAR* p_name, bc_service_ptr<bci_service> p_service)
		{
			const bcSIZE l_priority = m_services.size();

			auto l_ite = m_services.find(p_hash);
			if (l_ite == std::end(m_services))
			{
				l_ite = m_services.insert({ p_hash, _bc_service_container(p_hash, p_name, l_priority, std::move(p_service)) }).first;
				m_sorted_services.push_back(&l_ite->second);
			}
			else
			{
				throw bc_invalid_operation_exception("A service with same name has already been registered");
			}

			std::sort
			(
				std::begin(m_sorted_services),
				std::end(m_sorted_services),
				[](const sorted_map_t::value_type p_first, const sorted_map_t::value_type p_second)
				{
					return p_first->m_priority < p_second->m_priority;
				}
			);

			return l_ite->second.m_service.get();
		}
	}
}