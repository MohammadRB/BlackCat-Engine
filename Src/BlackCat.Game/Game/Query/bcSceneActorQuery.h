// [09/01/2024 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Core/Utility/bcDelegate.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Query/bcSceneGraphQuery.h"
#include "Game/Query/bcQueryContext.h"

namespace black_cat::game
{
	class bc_camera_frustum;

	template<class T>
	class bc_scene_actor_query : public core::bc_query<bc_scene_query_context>
	{
		BC_QUERY(saq)

	public:
		bc_scene_actor_query();

		bc_scene_actor_query(bc_scene_actor_query&&) noexcept = default;

		~bc_scene_actor_query() override = default;

		bc_scene_actor_query& operator=(bc_scene_actor_query&&) noexcept = default;

		core::bc_vector<T> get_result() const noexcept;

		bc_scene_actor_query& with(const bc_camera_frustum& p_frustum);

		template<class TComponent>
		bc_scene_actor_query& only() noexcept;

		bc_scene_actor_query& transform(core::bc_delegate<T(const bc_actor&)> p_transform);

	protected:
		void execute(const bc_scene_query_context& p_context) noexcept override;

	private:
		bc_scene_graph_query m_graph_query;
		core::bc_delegate<T(const bc_actor&)> m_transform_func;
		core::bc_vector<T> m_result;
	};

	template <class T>
	bc_scene_actor_query<T>::bc_scene_actor_query()
		: bc_query(message_name())
	{
	}

	template <class T>
	core::bc_vector<T> bc_scene_actor_query<T>::get_result() const noexcept
	{
		return m_result;
	}

	template <class T>
	bc_scene_actor_query<T>& bc_scene_actor_query<T>::with(const bc_camera_frustum& p_frustum)
	{
		m_graph_query.with(p_frustum);
		return *this;
	}

	template <class T>
	template <class TComponent>
	bc_scene_actor_query<T>& bc_scene_actor_query<T>::only() noexcept
	{
		m_graph_query.only<TComponent>();
		return *this;
	}

	template <class T>
	bc_scene_actor_query<T>& bc_scene_actor_query<T>::transform(core::bc_delegate<T(const bc_actor&)> p_transform)
	{
		m_transform_func = p_transform;
		return *this;
	}

	template <class T>
	void bc_scene_actor_query<T>::execute(const bc_scene_query_context& p_context) noexcept
	{
		if (!m_transform_func.is_valid())
		{
			return;
		}

		m_graph_query.execute(p_context);
		auto l_scene_buffer = m_graph_query.get_scene_buffer();

		m_result.reserve(l_scene_buffer.size());

		for (auto& l_actor : l_scene_buffer)
		{
			auto l_entry = m_transform_func(l_actor);
			m_result.push_back(std::move(l_entry));
		}
	}
}
