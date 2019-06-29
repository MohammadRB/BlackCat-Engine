// [09/14/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcLazyContent.h"
#include "Core/Content/bcContentManager.h"
#include "Game/Object/Scene/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderInstance.h"

namespace black_cat
{
	namespace game
	{
		bc_height_map_component::bc_height_map_component(bc_actor_component_index p_index)
			: bc_render_component(p_index)
		{
		}

		bc_height_map_component::bc_height_map_component(bc_height_map_component&& p_other) noexcept
			: bc_render_component(std::move(p_other)),
			m_height_map(std::move(p_other.m_height_map))
		{
		}

		bc_height_map_component::~bc_height_map_component()
		{
		}

		bc_height_map_component& bc_height_map_component::operator=(bc_height_map_component&& p_other) noexcept
		{
			m_height_map = std::move(p_other.m_height_map);
			bc_render_component::operator=(std::move(p_other));

			return *this;
		}

		bc_actor bc_height_map_component::get_actor() const noexcept
		{
			return get_manager()->component_get_actor(*this);
		}

		void bc_height_map_component::set_world_transform(const core::bc_matrix4f& p_transform)
		{
			m_transform = p_transform;
		}

		void bc_height_map_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			auto& l_height_map_name = p_parameters.get_value_throw< core::bc_string >(constant::g_param_heightmap);
			m_height_map = core::bc_lazy_content::get_content<bc_height_map>(l_height_map_name.c_str());
			auto* l_mediate = p_actor.get_component<bc_mediate_component>();

			l_mediate->set_bound_box
			(
				physics::bc_bound_box
				(
					m_transform.get_translation(),
					core::bc_vector3f
					(
						(m_height_map->get_width() * m_height_map->get_xz_multiplier()) / 2,
						1024.0 / 2, // TODO Get actual terrain height
						(m_height_map->get_height() * m_height_map->get_xz_multiplier()) / 2
					)
				)
			);
		}

		void bc_height_map_component::update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}

		void bc_height_map_component::render(bc_render_system& p_render_system) const
		{
			bc_render_instance l_instance(m_transform);
			p_render_system.add_render_instance(*m_height_map->get_render_state(), l_instance);
		}

		void bc_height_map_component::write_instance(bc_actor& p_actor, core::bc_json_key_value& p_parameters)
		{
			auto* l_content_manager = core::bc_get_service<core::bc_content_manager>();
			l_content_manager->save(*m_height_map);
		}
	}
}