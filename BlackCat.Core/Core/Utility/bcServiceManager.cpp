// [03/20/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Container/bcVector.h"

namespace black_cat
{
	namespace core
	{
		template BC_CORE_DLL class bc_singleton< bc_service_manager() >;

		void bc_iservice::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
		}

		_bc_service_container::_bc_service_container(bc_service_ptr< bc_iservice >&& p_service, bcSIZE p_priority) 
			: m_service(std::move(p_service)),
			m_priority(p_priority)
		{
		}

		_bc_service_container::_bc_service_container(_bc_service_container&& p_other) 
			: m_service(std::move(p_other.m_service)),
			m_priority(p_other.m_priority)
		{
			p_other.m_service = nullptr;
		}

		_bc_service_container& _bc_service_container::operator=(_bc_service_container&& p_other)
		{
			m_service = std::move(p_other.m_service);
			m_priority = p_other.m_priority;

			return *this;
		}

		bc_service_manager::bc_service_manager()
		{
		}

		bc_service_manager::~bc_service_manager()
		{
		}

		void bc_service_manager::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			bc_vector_frame< _bc_service_container* > l_services;
			l_services.reserve(m_services.size());

			for (auto& l_service : m_services)
			{
				l_services.push_back(&l_service.second);
			}

			std::sort(std::begin(l_services), std::end(l_services), [](const _bc_service_container* p_first, const _bc_service_container* p_second)
			{
				return p_first->m_priority < p_second->m_priority;
			});

			for (auto& l_service : l_services)
			{
				l_service->m_service->update(p_clock_update_param);
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
	}
}
