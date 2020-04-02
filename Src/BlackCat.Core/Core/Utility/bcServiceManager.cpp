// [03/20/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/bcException.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Container/bcVector.h"

namespace black_cat
{
	namespace core
	{
		template BC_CORE_DLL class bc_singleton< bc_service_manager() >;

		bc_iservice::~bc_iservice() = default;

		void bc_iservice::update(const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}

		_bc_service_container::_bc_service_container(bc_service_ptr< bc_iservice > p_service, bcSIZE p_priority) 
			: m_service(std::move(p_service)),
			m_priority(p_priority)
		{
		}

		bc_service_manager::bc_service_manager() = default;

		bc_service_manager::~bc_service_manager() = default;

		void bc_service_manager::update(const core_platform::bc_clock::update_param& p_clock_update_param)
		{
			bc_vector_frame< map_t::const_iterator > l_services;
			l_services.reserve(m_services.size());

			for (auto l_begin = std::cbegin(m_services), l_end = std::cend(m_services); l_begin != l_end; ++l_begin)
			{
				l_services.push_back(l_begin);
			}

			std::sort(std::begin(l_services), std::end(l_services), [](const map_t::const_iterator& p_first, const map_t::const_iterator& p_second)
			{
				return p_first->second.m_priority < p_second->second.m_priority;
			});

			for (auto& l_service : l_services)
			{
				l_service->second.m_service->update(p_clock_update_param);
			}
		}

		void bc_service_manager::_initialize()
		{
		}

		void bc_service_manager::_destroy()
		{
			bc_vector_frame< map_t::const_iterator > l_services;
			l_services.reserve(m_services.size());

			for (auto l_begin = std::cbegin(m_services), l_end = std::cend(m_services); l_begin != l_end; ++l_begin)
			{
				l_services.push_back(l_begin);
			}

			std::sort(std::begin(l_services), std::end(l_services), [](const map_t::const_iterator& p_first, const map_t::const_iterator& p_second)
			{
				return p_first->second.m_priority > p_second->second.m_priority; // Sort in reverse order
			});

			for (auto& l_service : l_services)
			{
				m_services.erase(l_service);
			}
		}

		bc_iservice* bc_service_manager::_get_service(bcUINT32 p_service_hash)
		{
			bc_iservice* l_result = nullptr;
			const auto l_ite = m_services.find(p_service_hash);

			if (l_ite != std::end(m_services))
			{
				l_result = l_ite->second.m_service.get();
			}

			return l_result;
		}

		bc_iservice* bc_service_manager::_register_service(const bcCHAR* p_name, bcUINT32 p_hash, bc_service_ptr<bc_iservice> p_service)
		{
			const bcSIZE l_service_priority = m_services.size();

			auto l_ite = m_services.find(p_hash);
			if (l_ite == std::end(m_services))
			{
				l_ite = m_services.insert({ p_hash, _bc_service_container(std::move(p_service), l_service_priority) }).first;
			}
			else
			{
				throw bc_invalid_operation_exception("A service with the same name has already registered");
				//l_ite->second = _bc_service_container(std::move(p_service), l_service_priority);
			}

			return l_ite->second.m_service.get();
		}
	}
}