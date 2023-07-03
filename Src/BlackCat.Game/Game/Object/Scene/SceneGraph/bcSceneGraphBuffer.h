// [15/01/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Container/bcIteratorAdapter.h"
#include "Game/System/Input/bcCameraInstance.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/System/Render/bcShapeDrawer.h"

namespace black_cat
{
	namespace game
	{
		/**
		 * \brief This class uses frame memory
		 */
		class bc_scene_graph_buffer : public core::bc_iterator_adapter<core::bc_vector_frame<bc_actor>>
		{
		public:
			bc_scene_graph_buffer() noexcept;

			explicit bc_scene_graph_buffer(bcSIZE p_initial_size);

			bc_scene_graph_buffer(std::initializer_list<bc_actor> p_initializer);

			template<typename TInputIterator>
			bc_scene_graph_buffer(TInputIterator p_first, TInputIterator p_last);

			bc_scene_graph_buffer(bc_scene_graph_buffer&&) noexcept;

			~bc_scene_graph_buffer();

			bc_scene_graph_buffer& operator=(bc_scene_graph_buffer&&) noexcept;

			iterator find(const bc_actor& p_actor) noexcept;

			const_iterator find(const bc_actor& p_actor) const noexcept;
			
			void add(const bc_actor& p_actor);

			template<typename TComponent>
			bc_scene_graph_buffer get_actors() const;

			void render_actors(const bc_actor_render_camera& p_camera, bc_render_state_buffer& p_buffer) const;

			template<typename TRenderComponent>
			void render_actors(const bc_actor_render_camera& p_camera, bc_render_state_buffer& p_buffer) const;

			template<typename TRenderComponent, typename ...TArgs>
			void render_actors(const bc_actor_render_camera& p_camera, bc_render_state_buffer& p_buffer, TArgs&&... p_args) const;

			void draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const;

		private:
			container_type m_actors;
		};

		inline bc_scene_graph_buffer::bc_scene_graph_buffer() noexcept
			: bc_iterator_adapter(m_actors)
		{	
		}

		inline bc_scene_graph_buffer::bc_scene_graph_buffer(bcSIZE p_initial_size)
			: bc_iterator_adapter(m_actors)
		{
			m_actors.reserve(p_initial_size);
		}

		inline bc_scene_graph_buffer::bc_scene_graph_buffer(std::initializer_list<bc_actor> p_initializer)
			: bc_iterator_adapter(m_actors),
			m_actors(p_initializer)
		{
		}

		template<typename TInputIterator>
		bc_scene_graph_buffer::bc_scene_graph_buffer(TInputIterator p_first, TInputIterator p_last)
			: bc_iterator_adapter(m_actors),
			m_actors(p_first, p_last)
		{
		}

		inline bc_scene_graph_buffer::bc_scene_graph_buffer(bc_scene_graph_buffer&& p_other) noexcept
			: bc_iterator_adapter(m_actors),
			m_actors(std::move(p_other.m_actors))
		{
		}

		inline bc_scene_graph_buffer::~bc_scene_graph_buffer() = default;

		inline bc_scene_graph_buffer& bc_scene_graph_buffer::operator=(bc_scene_graph_buffer&& p_other) noexcept
		{
			m_actors = std::move(p_other.m_actors);
			return *this;
		}

		inline bc_scene_graph_buffer::iterator bc_scene_graph_buffer::find(const bc_actor& p_actor) noexcept
		{
			for (auto l_ite = begin(), l_end = end(); l_ite != l_end; ++l_ite)
			{
				const bc_actor& l_actor = *l_ite;
				if(l_actor == p_actor)
				{
					return l_ite;
				}
			}

			return end();
		}

		inline bc_scene_graph_buffer::const_iterator bc_scene_graph_buffer::find(const bc_actor& p_actor) const noexcept
		{
			return const_cast<bc_scene_graph_buffer&>(*this).find(p_actor);
		}

		inline void bc_scene_graph_buffer::add(const bc_actor& p_actor)
		{
			m_actors.push_back(p_actor);
		}
		
		template<typename TComponent>
		bc_scene_graph_buffer bc_scene_graph_buffer::get_actors() const
		{
			static_assert(std::is_base_of_v<bci_actor_component, TComponent>, "TComponent must inherit from bc_render_component");

			bc_scene_graph_buffer l_buffer;

			for (bc_actor& l_actor : m_actors)
			{
				if (auto* l_component = l_actor.get_component<TComponent>())
				{
					l_buffer.add(l_actor);
				}
			}

			return l_buffer;
		}

		inline void bc_scene_graph_buffer::render_actors(const bc_actor_render_camera& p_camera, bc_render_state_buffer& p_buffer) const
		{
			const bc_actor_component_render_context l_render_context(p_camera, p_buffer);
			
			for(bc_actor& l_actor : m_actors)
			{
				if(const auto* l_component = l_actor.get_component<bc_render_component>())
				{
					l_component->render(l_render_context);
				}
			}
		}

		template<typename TRenderComponent>
		void bc_scene_graph_buffer::render_actors(const bc_actor_render_camera& p_camera, bc_render_state_buffer& p_buffer) const
		{
			static_assert(std::is_base_of_v<bc_render_component, TRenderComponent>, "TComponent must inherit from bc_render_component");

			const bc_actor_component_render_context l_render_context(p_camera, p_buffer);
			
			for (bc_actor& l_actor : m_actors)
			{
				if (const auto* l_component = static_cast<bc_render_component*>(l_actor.get_component<TRenderComponent>()))
				{
					l_component->render(l_render_context);
				}
			}
		}

		template<typename TRenderComponent, typename ...TArgs>
		void bc_scene_graph_buffer::render_actors(const bc_actor_render_camera& p_camera, bc_render_state_buffer& p_buffer, TArgs&&... p_args) const
		{
			static_assert(std::is_base_of_v<bc_render_component, TRenderComponent>, "TComponent must inherit from bc_render_component");

			const bc_actor_component_render_context l_render_context(p_camera, p_buffer);
			
			for (bc_actor& l_actor : m_actors)
			{
				if (auto* l_component = l_actor.get_component<TRenderComponent>())
				{
					l_component->render(l_render_context, std::forward<TArgs>(p_args)...);
				}
			}
		}
		
		inline void bc_scene_graph_buffer::draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const
		{
			for (const bc_actor& l_actor : m_actors)
			{
				l_actor.draw_debug(p_shape_drawer);
			}
		}
	}
}