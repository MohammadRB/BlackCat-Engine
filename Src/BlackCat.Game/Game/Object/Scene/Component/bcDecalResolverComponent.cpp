// [02/28/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/Component/bcDecalResolverComponent.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		void bc_decal_resolver_component::load_instance(const bc_actor_component_load_context& p_context)
		{
			// Decal component is usually created on the fly and is not included in entity template components.
			// If decal component exists its load_instance function will be called otherwise create and call its function.
			auto* l_decal_component = p_context.m_actor.get_component<bc_decal_component>();
			if(l_decal_component)
			{
				return;
			}

			const auto l_decals_ite = p_context.m_parameters.find(constant::g_param_decal_parameters);
			if(l_decals_ite != std::end(p_context.m_parameters))
			{
				l_decal_component = p_context.m_actor.get_create_component<bc_decal_component>();
				l_decal_component->load_instance(p_context);
			}
		}

		void bc_decal_resolver_component::write_instance(const bc_actor_component_write_context& p_context)
		{
			// If actor has decal component, its write method will be called
		}
	}
}