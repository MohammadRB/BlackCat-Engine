// [06/27/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Math/bcVector3f.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "Game/System/Render/Light/bcLightInstance.h"
#include "App/bcException.h"

namespace black_cat
{
	class bc_cascaded_shadow_map_buffer_container
	{
	public:
		struct entry;
		using value_type = std::pair<game::bc_light_instance::id_t, entry>;

	public:
		value_type& get_by_index(bcSIZE p_index);
		
		const value_type& get_by_index(bcSIZE p_index) const;

		value_type& get_by_id(game::bc_light_instance::id_t p_light_id);
		
		const value_type& get_by_id(game::bc_light_instance::id_t p_light_id) const;

		bcINT32 find_index(game::bc_light_instance::id_t p_light_id) const;

		void add(value_type&& p_item);

		bcSIZE size() const noexcept;

		void clear();

	private:
		core::bc_vector<value_type> m_depth_buffers;
	};

	struct bc_cascaded_shadow_map_buffer_container::entry
	{
		bcSIZE m_shadow_map_size;
		bcSIZE m_shadow_map_count;
		core::bc_vector_frame<bcSIZE> m_cascade_sizes;
		core::bc_vector_frame<core::bc_matrix4f> m_view_projections;
		graphic::bc_resource_view m_resource_view;
	};
	
	inline bc_cascaded_shadow_map_buffer_container::value_type& bc_cascaded_shadow_map_buffer_container::get_by_index(bcSIZE p_index)
	{
		return m_depth_buffers.at(p_index);
	}

	inline const bc_cascaded_shadow_map_buffer_container::value_type& bc_cascaded_shadow_map_buffer_container::get_by_index(bcSIZE p_index) const
	{
		return const_cast<bc_cascaded_shadow_map_buffer_container&>(*this).get_by_index(p_index);
	}

	inline bc_cascaded_shadow_map_buffer_container::value_type& bc_cascaded_shadow_map_buffer_container::get_by_id(game::bc_light_instance::id_t p_light_id)
	{
		const auto l_iterator = std::find_if
		(
			std::cbegin(m_depth_buffers), 
			std::cend(m_depth_buffers), 
			[&](const value_type& p_entry)
			{
				return p_entry.first == p_light_id;
			}
		);

		if (l_iterator == std::cend(m_depth_buffers))
		{
			throw bc_key_not_found_exception("No entry were found with given light direction");
		}

		return *l_iterator;
	}

	inline const bc_cascaded_shadow_map_buffer_container::value_type& bc_cascaded_shadow_map_buffer_container::get_by_id(game::bc_light_instance::id_t p_light_id) const
	{
		return const_cast<bc_cascaded_shadow_map_buffer_container&>(*this).get_by_id(p_light_id);
	}

	inline bcINT32 bc_cascaded_shadow_map_buffer_container::find_index(game::bc_light_instance::id_t p_light_id) const
	{
		const auto l_iterator = std::find_if
		(
			std::cbegin(m_depth_buffers),
			std::cend(m_depth_buffers),
			[&](const value_type& p_entry)
			{
				return p_entry.first == p_light_id;
			}
		);

		if (l_iterator == std::cend(m_depth_buffers))
		{
			return -1;
		}

		return std::distance(std::cbegin(m_depth_buffers), l_iterator);
	}

	inline void bc_cascaded_shadow_map_buffer_container::add(value_type&& p_item)
	{
		m_depth_buffers.push_back(std::move(p_item));
	}

	inline bcSIZE bc_cascaded_shadow_map_buffer_container::size() const noexcept
	{
		return m_depth_buffers.size();
	}

	inline void bc_cascaded_shadow_map_buffer_container::clear()
	{
		m_depth_buffers.clear();
	}
}