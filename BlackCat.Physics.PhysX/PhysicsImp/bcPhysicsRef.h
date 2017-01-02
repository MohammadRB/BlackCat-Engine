// [12/09/2016 MRB]

#pragma once

#include "PhysicsImp/bcPhysicsReference.h"

#include "CorePlatform/bcType.h"
#include "Physics/bcPhysicsRef.h"

namespace black_cat
{
	namespace physics
	{
		template< class TObject >
		struct bc_platform_physics_ref_pack<g_api_physx, TObject>
		{
			TObject m_object;
		};

		template< bc_physics_api TApi, class TObject >
		bc_platform_physics_ref<TApi, TObject>::bc_platform_physics_ref()
		{
		}

		template< bc_physics_api TApi, class TObject >
		template< typename >
		bc_platform_physics_ref<TApi, TObject>::bc_platform_physics_ref(const type& p_object)
		{
			reset(p_object);
		}

		template< bc_physics_api TApi, class TObject >
		bc_platform_physics_ref<TApi, TObject>::bc_platform_physics_ref(bc_platform_physics_ref&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template< bc_physics_api TApi, class TObject >
		bc_platform_physics_ref<TApi, TObject>::~bc_platform_physics_ref()
		{
			reset();
		}

		template< bc_physics_api TApi, class TObject >
		bc_platform_physics_ref<TApi, TObject>& bc_platform_physics_ref<TApi, TObject>::operator=(bc_platform_physics_ref&& p_other) noexcept
		{
			reset(p_other.m_pack.m_object);
			p_other.m_pack.m_object = type();

			return *this;
		}

		template< bc_physics_api TApi, class TObject >
		typename bc_platform_physics_ref<TApi, TObject>::type* bc_platform_physics_ref<TApi, TObject>::operator->() noexcept
		{
			return &get();
		}

		template< bc_physics_api TApi, class TObject >
		typename bc_platform_physics_ref<TApi, TObject>::type& bc_platform_physics_ref<TApi, TObject>::operator*()
		{
			return get();
		}

		template< bc_physics_api TApi, class TObject >
		typename bc_platform_physics_ref<TApi, TObject>::type& bc_platform_physics_ref<TApi, TObject>::get()
		{
			return m_pack.m_object;
		}

		template< bc_physics_api TApi, class TObject >
		const typename bc_platform_physics_ref<TApi, TObject>::type& bc_platform_physics_ref<TApi, TObject>::get() const
		{
			return const_cast<bc_platform_physics_ref&>(*this).get();
		}

		template< bc_physics_api TApi, class TObject >
		void bc_platform_physics_ref<TApi, TObject>::reset()
		{
			auto& l_reference = static_cast<bc_platform_physics_reference<TApi>&>(m_pack.m_object);

			if(l_reference.is_valid())
			{
				l_reference.get_platform_pack().m_px_object->release();
				m_pack.m_object = type();
			}
		}

		template< bc_physics_api TApi, class TObject >
		void bc_platform_physics_ref<TApi, TObject>::reset(const type& p_object)
		{
			auto& l_reference = static_cast<bc_platform_physics_reference<TApi>&>(m_pack.m_object);

			if (l_reference.is_valid())
			{
				l_reference.get_platform_pack().m_px_object->release();
			}

			m_pack.m_object = p_object;
		}
	}
}