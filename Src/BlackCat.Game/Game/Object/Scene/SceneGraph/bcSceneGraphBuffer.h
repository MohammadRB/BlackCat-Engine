// [01/15/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
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
		class bc_scene_graph_buffer
		{
		private:
			using container_type = core::bc_vector_frame<bc_actor>;

		public:
			using value_type = container_type::value_type;
			using pointer = container_type::pointer;
			using const_pointer = container_type::const_pointer;
			using reference = container_type::reference;
			using const_reference = container_type::const_reference;
			using difference_type = container_type::difference_type;
			using size_type = container_type::size_type;
			using iterator = container_type::iterator;
			using const_iterator = container_type::const_iterator;
			using reverse_iterator = container_type::reverse_iterator;
			using const_reverse_iterator = container_type::const_reverse_iterator;

		public:
			bc_scene_graph_buffer();

			bc_scene_graph_buffer(bcSIZE p_initial_size);

			bc_scene_graph_buffer(std::initializer_list<bc_actor> p_initializer);

			bc_scene_graph_buffer(bc_scene_graph_buffer&&) noexcept;

			~bc_scene_graph_buffer();

			bc_scene_graph_buffer& operator=(bc_scene_graph_buffer&&) noexcept;

			iterator begin() noexcept;

			const_iterator begin() const noexcept;

			const_iterator cbegin() const noexcept;

			iterator end() noexcept;

			const_iterator end() const noexcept;

			const_iterator cend() const noexcept;

			reverse_iterator rbegin() noexcept;

			const_reverse_iterator rbegin() const noexcept;

			const_reverse_iterator crbegin() const noexcept;

			reverse_iterator rend() noexcept;

			const_reverse_iterator rend() const noexcept;

			const_reverse_iterator crend() const noexcept;

			size_type size() const noexcept;

			void add_actor(const bc_actor& p_actor);

			iterator find(const bc_actor& p_actor) noexcept;
			
			const_iterator find(const bc_actor& p_actor) const noexcept;
			
			void render_actors(bc_render_state_buffer& p_buffer) const;

			template<typename TRenderComponent>
			void render_actors(bc_render_state_buffer& p_buffer) const;

			template<typename TRenderComponent, typename ...TArgs>
			void render_actors(bc_render_state_buffer& p_buffer, TArgs&&... p_args) const;

			void draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const;

		private:
			container_type m_actors;
		};

		inline bc_scene_graph_buffer::bc_scene_graph_buffer() = default;

		inline bc_scene_graph_buffer::bc_scene_graph_buffer(bcSIZE p_initial_size)
		{
			m_actors.reserve(p_initial_size);
		}

		inline bc_scene_graph_buffer::bc_scene_graph_buffer(std::initializer_list<bc_actor> p_initializer)
			: m_actors(p_initializer)
		{
		}

		inline bc_scene_graph_buffer::bc_scene_graph_buffer(bc_scene_graph_buffer&& p_other) noexcept = default;

		inline bc_scene_graph_buffer::~bc_scene_graph_buffer() = default;

		inline bc_scene_graph_buffer& bc_scene_graph_buffer::operator=(bc_scene_graph_buffer&& p_other) noexcept = default;

		inline bc_scene_graph_buffer::iterator bc_scene_graph_buffer::begin() noexcept
		{
			return m_actors.begin();
		}

		inline bc_scene_graph_buffer::const_iterator bc_scene_graph_buffer::begin() const noexcept
		{
			return m_actors.begin();
		}

		inline bc_scene_graph_buffer::const_iterator bc_scene_graph_buffer::cbegin() const noexcept
		{
			return m_actors.cbegin();
		}

		inline bc_scene_graph_buffer::iterator bc_scene_graph_buffer::end() noexcept
		{
			return m_actors.end();
		}

		inline bc_scene_graph_buffer::const_iterator bc_scene_graph_buffer::end() const noexcept
		{
			return m_actors.end();
		}

		inline bc_scene_graph_buffer::const_iterator bc_scene_graph_buffer::cend() const noexcept
		{
			return m_actors.cend();
		}

		inline bc_scene_graph_buffer::reverse_iterator bc_scene_graph_buffer::rbegin() noexcept
		{
			return m_actors.rbegin();
		}

		inline bc_scene_graph_buffer::const_reverse_iterator bc_scene_graph_buffer::rbegin() const noexcept
		{
			return m_actors.rbegin();
		}

		inline bc_scene_graph_buffer::const_reverse_iterator bc_scene_graph_buffer::crbegin() const noexcept
		{
			return m_actors.crbegin();
		}

		inline bc_scene_graph_buffer::reverse_iterator bc_scene_graph_buffer::rend() noexcept
		{
			return m_actors.rend();
		}

		inline bc_scene_graph_buffer::const_reverse_iterator bc_scene_graph_buffer::rend() const noexcept
		{
			return m_actors.rend();
		}

		inline bc_scene_graph_buffer::const_reverse_iterator bc_scene_graph_buffer::crend() const noexcept
		{
			return m_actors.crend();
		}

		inline bc_scene_graph_buffer::size_type bc_scene_graph_buffer::size() const noexcept
		{
			return m_actors.size();
		}

		inline void bc_scene_graph_buffer::add_actor(const bc_actor& p_actor)
		{
			m_actors.push_back(p_actor);
		}

		inline bc_scene_graph_buffer::iterator bc_scene_graph_buffer::find(const bc_actor& p_actor) noexcept
		{
			for (auto l_ite = begin(), l_end = end(); l_ite != l_end; ++l_ite)
			{
				if(*l_ite == p_actor)
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

		inline void bc_scene_graph_buffer::render_actors(bc_render_state_buffer& p_buffer) const
		{
			for(bc_actor& l_actor : m_actors)
			{
				auto* l_component = l_actor.get_component<bc_render_component>();
				if(l_component)
				{
					l_component->render(p_buffer);
				}
			}
		}

		template< typename TRenderComponent >
		void bc_scene_graph_buffer::render_actors(bc_render_state_buffer& p_buffer) const
		{
			static_assert(std::is_base_of_v<bc_render_component, TRenderComponent>, "TComponent must inherit from bc_render_component");

			for (bc_actor& l_actor : m_actors)
			{
				auto* l_component = static_cast<bc_render_component*>(l_actor.get_component<TRenderComponent>());
				if (l_component)
				{
					l_component->render(p_buffer);
				}
			}
		}

		template< typename TRenderComponent, typename ... TArgs >
		void bc_scene_graph_buffer::render_actors(bc_render_state_buffer& p_buffer, TArgs&&... p_args) const
		{
			static_assert(std::is_base_of_v<bc_render_component, TRenderComponent>, "TComponent must inherit from bc_render_component");

			for (bc_actor& l_actor : m_actors)
			{
				auto* l_component = l_actor.get_component<TRenderComponent>();
				if (l_component)
				{
					l_component->render(p_buffer, std::forward<TArgs>(p_args)...);
				}
			}
		}

		inline void bc_scene_graph_buffer::draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const
		{
			for (const bc_actor& l_actor : *this)
			{
				l_actor.draw_debug(p_shape_drawer);
			}
		}
	}
}