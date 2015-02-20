// [2/13/2015 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Utility/bcSingleton.h"
#include "Core/Utility/bcDelegate.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcMap.h"
#include "Core/Event/bcEventManager.h"

namespace black_cat
{
	namespace core
	{
		template< class TService >
		class bc_service_traits
		{
		public:
			bc_string service_name() const
			{
				return TService::service_name();
			}
		};

		class bc_iservice
		{
		public:
			virtual ~bc_iservice()
			{
			}

			virtual bc_string service_name() const = 0;
		};

		class bc_service_manager : public bc_singleton< bc_service_manager() >
		{
		public:
			template< class TService >
			using bc_service_ptr = bc_unique_ptr<TService>;
			using delegate_type = bc_delegate< bc_service_ptr< bc_iservice >() >;

		public:
			bc_service_manager()
			{
			}

			~bc_service_manager()
			{
			}

			void initialize() override
			{
			}

			void destroy() override
			{
			}

			template< class TService >
			void register_service(bc_service_ptr< TService >&& p_service)
			{
				bc_string l_service_name = bc_service_traits< TService >::service_name();
				
				auto l_ite = m_services.lower_bound(l_service_name);
				if (l_ite == m_services.end() || m_services.key_comp()(l_service_name, l_ite->first))
				{
					l_ite = m_services.emplace_hint(l_ite, l_service_name, service_container(std::move(p_service)));
				}
				else
				{
					l_ite->second = service_container(std::move(p_service));
				}
			}

			template< class TService >
			TService* get_service()
			{
				TService* l_result = nullptr;
				bc_string l_service_name = bc_service_traits< TService >::service_name();
				
				auto l_ite = m_services.find(l_service_name);

				if (l_ite != m_services.end())
					l_result = static_cast<TService*>(&l_ite->get());

				return l_result;
			}

			core_platform::bc_clock& global_clock() noexcept(true)
			{
				return m_clock;
			}

			bc_event_manager& global_event_manager() noexcept(true)
			{
				return m_event_manager;
			}

		protected:

		private:
			class service_container : private core_platform::bc_no_copy
			{
			public:
				service_container(bc_service_ptr< bc_iservice >&& p_service)
					: m_service(std::move(p_service))
				{
				}

				service_container(service_container&& p_other)
					: m_service(std::move(p_other.m_service))
				{
					p_other.m_service = nullptr;
				}

				service_container& operator =(service_container&& p_other)
				{
					m_service = std::move(p_other.m_service);
					p_other.m_service = nullptr;

					return *this;
				}

				~service_container() = default;

				bc_iservice& get()
				{
					return *m_service;
				}

			protected:

			private:
				bc_service_ptr< bc_iservice > m_service;
			};

			bc_map<bc_string, service_container> m_services;

			core_platform::bc_clock m_clock;
			bc_event_manager m_event_manager;
		};
	}
}