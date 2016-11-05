// [2/13/2015 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Utility/bcSingleton.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"

namespace black_cat
{
	namespace core
	{
		template< class TService >
		class bc_service_traits
		{
		public:
			static const bcCHAR* service_name()
			{
				return TService::service_name();
			}
		};

		class bc_service_manager;

		// Act like a tag class
		class BC_CORE_DLL bc_iservice : public core_platform::bc_no_copy
		{
		public:
			friend class bc_service_manager;

		public:
			bc_iservice(bc_iservice&&) = default;

			virtual ~bc_iservice() = 0 {}

			bc_iservice& operator=(bc_iservice&&) = default;

		protected:
			bc_iservice() = default;

			virtual void update(core_platform::bc_clock::update_param p_clock_update_param);
		};

		template< class TService >
		using bc_service_ptr = bc_unique_ptr<TService>;

		template< class TService, typename ...TArgs >
		bc_service_ptr< TService > bc_make_service(TArgs&&... p_args)
		{
			return core::bc_make_unique< TService >(core::bc_alloc_type::program, std::forward<TArgs>(p_args)...);
		}

		template< class TService >
		TService* bc_get_service()
		{
			return bc_service_manager::get().get_service<TService>();
		}

		class BC_CORE_DLL _bc_service_container : private core_platform::bc_no_copy
		{
		public:
			_bc_service_container(bc_service_ptr< bc_iservice >&& p_service, bcSIZE p_priority);

			_bc_service_container(_bc_service_container&& p_other) noexcept;

			_bc_service_container& operator =(_bc_service_container&& p_other) noexcept;

			~_bc_service_container() = default;

			bc_service_ptr< bc_iservice > m_service;
			bcSIZE m_priority;
		protected:

		private:
			
		};

		class BC_CORE_DLL bc_service_manager : public bc_singleton< bc_service_manager() >
		{
		private:
			using map_t = bc_unordered_map_program<bc_string, _bc_service_container>;

		public:
			bc_service_manager();

			~bc_service_manager();

			template< class TService >
			TService* register_service(bc_service_ptr< TService >&& p_service);

			/*template< class TService >
			void unregister_service();*/

			template< class TService >
			TService* get_service();

			// Call to update all registered services
			void update(core_platform::bc_clock::update_param p_clock_update_param);

		protected:

		private:
			void _initialize() override;

			void _destroy() override;

			map_t m_services;
		};

		template< class TService >
		TService* bc_service_manager::register_service(bc_service_ptr< TService >&& p_service)
		{
			static_assert(std::is_base_of<bc_iservice, TService>::value, "services must inherite from bc_iservice");

			bc_string l_service_name = bc_service_traits< TService >::service_name();
			bcSIZE l_service_priority = m_services.size();

			auto l_ite = m_services.lower_bound(l_service_name);
			if (l_ite == m_services.end() || m_services.key_comp()(l_service_name, l_ite->first))
			{
				l_ite = m_services.emplace_hint(l_ite, l_service_name, _bc_service_container(std::move(p_service), l_service_priority));
			}
			else
			{
				l_ite->second = _bc_service_container(std::move(p_service), l_service_priority);
			}

			return static_cast< TService* >(l_ite->second.m_service.get());
		}

		/*template< class TService >
		void bc_service_manager::unregister_service()
		{
			TService* l_result = nullptr;
			bc_string l_service_name = bc_service_traits< TService >::service_name();

			auto l_ite = m_services.find(l_service_name);

			if (l_ite != m_services.end())
				m_services.erase(l_ite);
		}*/

		template< class TService >
		TService* bc_service_manager::get_service()
		{
			TService* l_result = nullptr;
			bc_string l_service_name = bc_service_traits< TService >::service_name();

			auto l_ite = m_services.find(l_service_name);

			if (l_ite != m_services.end())
				l_result = static_cast< TService* >(l_ite->second.m_service.get());

			return l_result;
		}
	}
}