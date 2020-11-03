// [10/30/2020 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Core/Utility/bcNullable.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/System/Render/Light/bcLight.h"
#include "Game/System/Render/Light/bcLightInstance.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene_light_query : public core::bc_query<bc_light_instances_query_context>
		{
			BC_QUERY(sc_lgt)

		public:
			bc_scene_light_query(bc_light_type p_types);

			bc_scene_light_query(bc_scene_light_query&&) noexcept;

			~bc_scene_light_query();

			bc_scene_light_query& operator=(bc_scene_light_query&&) noexcept;

			bc_scene_light_query& with(const bc_camera_frustum& p_frustum);

			core::bc_vector<bc_light_instance> get_lights() noexcept;
			
			void execute(const bc_light_instances_query_context& p_context) noexcept override;
			
		private:
			bc_light_type m_types;
			core::bc_nullable<bc_camera_frustum> m_frustum;
			core::bc_vector<bc_light_instance> m_lights;
		};

		inline bc_scene_light_query::bc_scene_light_query(bc_light_type p_types)
			: bc_query(message_name()),
			m_types(p_types)
		{
		}

		inline bc_scene_light_query::bc_scene_light_query(bc_scene_light_query&& p_other) noexcept
			: bc_query(p_other),
			m_types(p_other.m_types),
			m_frustum(std::move(p_other.m_frustum)),
			m_lights(std::move(p_other.m_lights))
		{
		}

		inline bc_scene_light_query::~bc_scene_light_query() = default;

		inline bc_scene_light_query& bc_scene_light_query::operator=(bc_scene_light_query&& p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_types = p_other.m_types;
			m_frustum = std::move(p_other.m_frustum);
			m_lights = std::move(p_other.m_lights);
			
			return *this;
		}

		inline bc_scene_light_query& bc_scene_light_query::with(const bc_camera_frustum& p_frustum)
		{
			m_frustum = p_frustum;
			return *this;
		}
		
		inline core::bc_vector<bc_light_instance> bc_scene_light_query::get_lights() noexcept
		{
			return std::move(m_lights);
		}

		inline void bc_scene_light_query::execute(const bc_light_instances_query_context& p_context) noexcept
		{
			const bool l_need_direct = core::bc_enum::has(m_types, bc_light_type::direct),
				l_need_spot = core::bc_enum::has(m_types, bc_light_type::spot),
				l_need_point = core::bc_enum::has(m_types, bc_light_type::point);

			{
				// TODO const_cast
				auto& l_iterator = const_cast<bc_light_manager::iterator_buffer&>(p_context.m_iterator);
				core_platform::bc_lock_guard<bc_light_manager::iterator_buffer> l_lock(l_iterator);

				for(auto& l_light : p_context.m_iterator)
				{
					const bool l_is_direct = l_need_direct && l_light.get_type() == bc_light_type::direct;
					if (l_is_direct)
					{
						m_lights.push_back(bc_light_instance(l_light));
						continue;
					}

					const bool l_is_spot = l_need_spot && l_light.get_type() == bc_light_type::spot;
					if (l_is_spot)
					{
						if (m_frustum.is_set() && !m_frustum->intersects(l_light.get_bound_box()))
						{
							continue;
						}

						m_lights.push_back(bc_light_instance(l_light));
					}

					const bool l_is_point = l_need_point && l_light.get_type() == bc_light_type::point;
					if (l_is_point)
					{
						if (m_frustum.is_set() && !m_frustum->intersects(l_light.get_bound_box()))
						{
							continue;
						}

						m_lights.push_back(bc_light_instance(l_light));
					}
				}
			}
		}
	}
}