// [2/13/2015 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/bcExport.h"
#include "Core/bcConstant.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Utility/bcSingleton.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"

namespace black_cat
{
	namespace core
	{
		template<class TService>
		class bc_service_traits
		{
		public:
			static constexpr const bcCHAR* service_name()
			{
				return TService::service_name();
			}
			
			static constexpr bc_string_cmp_hash service_hash()
			{
				return TService::service_hash();
			}
		};

		class bc_service_manager;

		class BC_CORE_DLL bci_service : public core_platform::bc_no_copy_move
		{
			friend class bc_service_manager;

		public:
			virtual ~bci_service();

		protected:
			bci_service() = default;

		private:
			virtual void update(const core_platform::bc_clock::update_param& p_clock);
		};

		template<class TService>
		using bc_service_ptr = bc_unique_ptr<TService>;

		class BC_CORE_DLL _bc_service_container : private core_platform::bc_no_copy
		{
		public:
			_bc_service_container(bcUINT32 p_hash, const bcCHAR* p_name, bcSIZE p_priority, bc_service_ptr< bci_service > p_service);

			_bc_service_container(_bc_service_container&& p_other) noexcept = default;

			~_bc_service_container() = default;

			_bc_service_container& operator =(_bc_service_container&& p_other) noexcept = default;

			bcUINT32 m_hash;
			const bcCHAR* m_name;
			bcSIZE m_priority;
			bc_service_ptr< bci_service > m_service;
		};

		class BC_CORE_DLL bc_service_manager : public bc_singleton<bc_service_manager()>
		{
		private:
			using map_t = bc_unordered_map_program<bcUINT32, _bc_service_container>;
			using sorted_map_t = bc_vector_movable<_bc_service_container*>;

		public:
			bc_service_manager();

			~bc_service_manager() override;

			template<class TService>
			TService* get_service();

			template<class TService>
			TService* register_service(bc_service_ptr<TService> p_service);

			void update(const core_platform::bc_clock::update_param& p_clock_update_param);

		private:
			void _initialize() override;

			void _destroy() override;

			bci_service* _get_service(bcUINT32 p_service_hash);

			bci_service* _register_service(bcUINT32 p_hash, const bcCHAR* p_name, bc_service_ptr<bci_service> p_service);

			map_t m_services;
			sorted_map_t m_sorted_services;
		};

		template<class TService>
		TService* bc_service_manager::get_service()
		{
			constexpr auto l_service_hash = bc_service_traits<TService>::service_hash();
			return static_cast<TService*>(_get_service(l_service_hash));
		}

		template<class TService>
		TService* bc_service_manager::register_service(bc_service_ptr<TService> p_service)
		{
			static_assert(std::is_base_of_v<bci_service, TService>, "services must inherit from bc_iservice");

			constexpr auto l_hash = bc_service_traits<TService>::service_hash();
			constexpr auto* l_name = bc_service_traits<TService>::service_name();
			return static_cast<TService*>(_register_service(l_hash, l_name, std::move(p_service)));
		}

		template<class TService, typename ...TArgs>
		bc_service_ptr< TService > bc_make_service(TArgs&&... p_args)
		{
			return bc_make_unique<TService>(bc_alloc_type::program, std::forward<TArgs>(p_args)...);
		}

		template<class TService>
		TService* bc_register_service(bc_service_ptr<TService> p_service)
		{
			static bc_service_manager* s_instance = bc_service_manager::get();
			return s_instance->register_service(std::move(p_service));
		}

		template<class TService>
		TService* bc_get_service()
		{
			static bc_service_manager* s_instance = bc_service_manager::get();
			return s_instance->get_service<TService>();
		}
	}
}