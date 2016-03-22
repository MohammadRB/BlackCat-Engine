// [2/13/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
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
			static bc_string service_name()
			{
				return TService::service_name();
			}
		};

		// Act like a tag class
		class bc_iservice : public core_platform::bc_no_copy
		{
		public:
			virtual ~bc_iservice() = 0
			{
			}
		};

		template< class TService >
		using bc_service_ptr = bc_unique_ptr<TService>;

		class BC_COREDLL_EXP _bc_service_container : private core_platform::bc_no_copy
		{
		public:
			_bc_service_container(bc_service_ptr< bc_iservice >&& p_service);

			_bc_service_container(_bc_service_container&& p_other);

			_bc_service_container& operator =(_bc_service_container&& p_other);

			~_bc_service_container() = default;

			bc_iservice& get()
			{
				return *m_service;
			}

		protected:

		private:
			bc_service_ptr< bc_iservice > m_service;
		};

		class BC_COREDLL_EXP bc_service_manager : public bc_singleton< bc_service_manager() >
		{
		public:
			bc_service_manager();

			~bc_service_manager();

			template< class TService >
			void register_service(bc_service_ptr< TService >&& p_service);

			template< class TService >
			void unregister_service();

			template< class TService >
			TService* get_service();
		protected:

		private:
			void _initialize() override;

			void _destroy() override;

			bc_unordered_map_program<bc_string, _bc_service_container> m_services;
		};

		template< class TService >
		void bc_service_manager::register_service(bc_service_ptr< TService >&& p_service)
		{
			bc_string l_service_name = bc_service_traits< TService >::service_name();

			auto l_ite = m_services.lower_bound(l_service_name);
			if (l_ite == m_services.end() || m_services.key_comp()(l_service_name, l_ite->first))
			{
				l_ite = m_services.emplace_hint(l_ite, l_service_name, _bc_service_container(std::move(p_service)));
			}
			else
			{
				l_ite->second = _bc_service_container(std::move(p_service));
			}
		}

		template< class TService >
		void bc_service_manager::unregister_service()
		{
			TService* l_result = nullptr;
			bc_string l_service_name = bc_service_traits< TService >::service_name();

			auto l_ite = m_services.find(l_service_name);

			if (l_ite != m_services.end())
				m_services.erase(l_ite);
		}

		template< class TService >
		TService* bc_service_manager::get_service()
		{
			TService* l_result = nullptr;
			bc_string l_service_name = bc_service_traits< TService >::service_name();

			auto l_ite = m_services.find(l_service_name);

			if (l_ite != m_services.end())
				l_result = static_cast< TService* >(&l_ite->second.get());

			return l_result;
		}
	}
}