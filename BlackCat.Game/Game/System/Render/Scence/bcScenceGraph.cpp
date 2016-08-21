// [07/14/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/Scence/bcScenceGraph.h"
#include "Game/Object/Scence/Component/bcRenderComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_scence_graph::bc_scence_graph()
		{
		}

		bc_scence_graph::~bc_scence_graph()
		{
		}

		void bc_scence_graph::add_object(bc_actor p_actor)
		{
			m_objects.push_back(p_actor);
		}

		bc_actor bc_scence_graph::remove_object(bc_actor p_actor)
		{
			auto l_object = std::find(std::cbegin(m_objects), std::cend(m_objects), p_actor);

			m_objects.erase(l_object);

			return *l_object;
		}

		void bc_scence_graph::update(const bc_input_system& p_input_system, core_platform::bc_clock::update_param p_clock_update_param)
		{
		}

		void bc_scence_graph::render(bc_render_system& p_render_system, bool p_preserve_render_instances)
		{
			for(auto& l_actor : m_objects)
			{
				auto* l_render_component = l_actor.get_component< bc_render_component >();

				if(l_render_component != nullptr)
				{
					l_render_component->render(p_render_system);
				}
			}

			p_render_system.render_all_instances();

			if(!p_preserve_render_instances)
			{
				p_render_system.clear_render_instances();
			}
		}
	}
}