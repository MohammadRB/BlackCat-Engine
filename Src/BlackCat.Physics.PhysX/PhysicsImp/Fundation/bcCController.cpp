// [22/03/2021 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"

#include "Core/Utility/bcEnumOperand.h"
#include "PhysicsImp/Fundation/bcCController.h"
#include "PhysicsImp/bcImplementation.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/bcExport.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ccontroller<g_api_physx>::bc_platform_ccontroller() noexcept
			: bc_platform_physics_reference(),
			m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ccontroller<g_api_physx>::bc_platform_ccontroller(platform_pack& p_pack) noexcept
			: bc_platform_physics_reference(),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ccontroller<g_api_physx>::bc_platform_ccontroller(const bc_platform_ccontroller& p_other) noexcept
			: bc_platform_physics_reference(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ccontroller<g_api_physx>::~bc_platform_ccontroller() = default;

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ccontroller<g_api_physx>& bc_platform_ccontroller<g_api_physx>::operator=(const bc_platform_ccontroller& p_other) noexcept
		{
			bc_platform_physics_reference::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_dynamic bc_platform_ccontroller<g_api_physx>::get_actor() const noexcept
		{
			bc_rigid_dynamic::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_controller->getActor();

			return bc_rigid_dynamic(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_ccontroller<g_api_physx>::get_position() const noexcept
		{
			return bc_to_game_hand_ex(m_pack.m_controller->getPosition());
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_ccontroller<g_api_physx>::set_position(const core::bc_vector3f& p_position) noexcept
		{
			m_pack.m_controller->setPosition(bc_to_right_hand_ex(p_position));
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_ccontroller<g_api_physx>::get_foot_position() const noexcept
		{
			return bc_to_game_hand_ex(m_pack.m_controller->getFootPosition());
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_ccontroller<g_api_physx>::set_foot_position(const core::bc_vector3f& p_position) noexcept
		{
			m_pack.m_controller->setFootPosition(bc_to_right_hand_ex(p_position));
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_ccontroller<g_api_physx>::get_up() const noexcept
		{
			return bc_to_game_hand(m_pack.m_controller->getUpDirection());
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_ccontroller<g_api_physx>::set_up(const core::bc_vector3f& p_position) noexcept
		{
			m_pack.m_controller->setUpDirection(bc_to_right_hand(p_position));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_ccontroller<g_api_physx>::get_step_offset() const noexcept
		{
			return m_pack.m_controller->getStepOffset();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_ccontroller<g_api_physx>::set_step_offset(bcFLOAT p_offset) noexcept
		{
			m_pack.m_controller->setStepOffset(p_offset);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_ccontroller<g_api_physx>::get_contact_offset() const noexcept
		{
			return m_pack.m_controller->getContactOffset();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_ccontroller<g_api_physx>::set_contact_offset(bcFLOAT p_offset) noexcept
		{
			m_pack.m_controller->setContactOffset(p_offset);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_ccontroller<g_api_physx>::get_slope_limit() const noexcept
		{
			return m_pack.m_controller->getSlopeLimit();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_ccontroller<g_api_physx>::set_slope_limit(bcFLOAT p_offset) noexcept
		{
			m_pack.m_controller->setSlopeLimit(p_offset);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_ccontroller<g_api_physx>::resize(bcFLOAT p_height, bool p_align_foot) noexcept
		{
			if(p_align_foot)
			{
				m_pack.m_controller->resize(p_height);
			}
			else
			{
				static_cast<physx::PxCapsuleController*>(m_pack.m_controller)->setHeight(p_height);
			}
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_ccontroller_collision_flag bc_platform_ccontroller<g_api_physx>::move(const core::bc_vector3f& p_displacement,
			const platform::bc_clock::update_param& p_clock,
			bc_scene_query_pre_filter_callback* p_pre_filter,
			bc_scene_query_post_filter_callback* p_post_filter,
			bc_ccontroller_collision_filter_callback* p_cc_vs_cc_filter) noexcept
		{
			physx::PxFilterData l_filter_data;
			bc_px_ccontroller_query_filter l_filter_callback(p_pre_filter, p_post_filter, false);
			bc_px_ccontroller_collision_filter l_controller_filter_callback(p_cc_vs_cc_filter);

			physx::PxQueryFilterCallback* l_filter_callback_ptr = &l_filter_callback;
			physx::PxControllerFilterCallback* l_controller_filter_callback_ptr = nullptr;
			
			if(p_cc_vs_cc_filter)
			{
				l_controller_filter_callback_ptr = &l_controller_filter_callback;
			}

			physx::PxControllerFilters l_filter(&l_filter_data, l_filter_callback_ptr, l_controller_filter_callback_ptr);

			if(p_post_filter)
			{
				l_filter.mFilterFlags |= physx::PxQueryFlag::Enum::ePOSTFILTER;
			}
			
			const auto l_px_flag = m_pack.m_controller->move
			(
				bc_to_right_hand(p_displacement),
				0.01f,
				p_clock.m_elapsed_second,
				l_filter
			);

			return static_cast<bc_ccontroller_collision_flag>(static_cast<bcUINT32>(l_px_flag));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_ccontroller<g_api_physx>::is_valid() const noexcept
		{
			return m_pack.m_controller != nullptr;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_ccontroller<g_api_physx>::set_data(void* p_user_data) noexcept
		{
			m_pack.m_controller->setUserData(p_user_data);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void* bc_platform_ccontroller<g_api_physx>::get_data() const noexcept
		{
			return m_pack.m_controller->getUserData();
		}
	}
}