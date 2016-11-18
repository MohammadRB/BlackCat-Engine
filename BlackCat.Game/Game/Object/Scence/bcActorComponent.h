// [04/27/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/bcConstant.h"
#include "Core/Utility/bcDataDrivenParameter.h"
#include "Game/Object/Scence/bcActor.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_component_manager;
		using bc_actor_component_hash = bcUINT32;
		using bc_actor_component_index = bcINT32;
		constexpr bc_actor_component_index g_actor_component_invalid_index = bc_actor_component_index(-1);

		class bc_iactor_component
		{
		private:
			friend class bc_actor_component_manager;

		public:
			virtual ~bc_iactor_component();

			// Initialize component per entity data members
			virtual void initialize(const core::bc_data_driven_parameter& p_parameters) = 0;

			virtual void update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param) = 0;

		protected:
			explicit bc_iactor_component(bc_actor_component_index p_index) noexcept;

			bc_iactor_component(bc_iactor_component&& p_other) noexcept;

			bc_iactor_component& operator=(bc_iactor_component&& p_other) noexcept;

			bc_actor_component_index get_index() const noexcept;

		private:
			bc_actor_component_index m_index;
		};

		inline bc_iactor_component::~bc_iactor_component() = default;

		inline bc_iactor_component::bc_iactor_component(bc_actor_component_index p_index) noexcept
			: m_index(p_index)
		{
		}

		inline bc_iactor_component::bc_iactor_component(bc_iactor_component&& p_other) noexcept
			: m_index(p_other.m_index)
		{
		}

		inline bc_iactor_component& bc_iactor_component::operator=(bc_iactor_component&& p_other) noexcept
		{
			m_index = p_other.m_index;

			return *this;
		}

		inline bc_actor_component_index bc_iactor_component::get_index() const noexcept
		{
			return m_index;
		}
	}
}