// [06/27/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Math/bcVector3f.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "BlackCat/bcException.h"

namespace black_cat
{
	class bc_cascaded_shadow_map_buffer_container
	{
	public:
		struct entry;
		using value_type = std::pair<core::bc_vector3f, entry>;

	public:
		const value_type& get(bcSIZE p_index) const;

		const value_type& get(const core::bc_vector3f& p_light_direction) const;

		void add(value_type&& p_item);

		void clear();

		bcSIZE size() const noexcept;

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

	inline const bc_cascaded_shadow_map_buffer_container::value_type& bc_cascaded_shadow_map_buffer_container::get(bcSIZE p_index) const
	{
		return m_depth_buffers.at(p_index);
	}

	inline const bc_cascaded_shadow_map_buffer_container::value_type& bc_cascaded_shadow_map_buffer_container::get(const core::bc_vector3f& p_light_direction) const
	{
		const auto l_iterator = std::find_if(std::cbegin(m_depth_buffers), std::cend(m_depth_buffers), [&](const value_type& p_entry)
		{
			return p_entry.first == p_light_direction;
		});

		if(l_iterator == std::cend(m_depth_buffers))
		{
			throw bc_key_not_found_exception("No entry were found with given light direction");
		}

		return *l_iterator;
	}

	inline void bc_cascaded_shadow_map_buffer_container::add(value_type&& p_item)
	{
		m_depth_buffers.push_back(std::move(p_item));
	}

	inline void bc_cascaded_shadow_map_buffer_container::clear()
	{
		m_depth_buffers.clear();
	}

	inline bcSIZE bc_cascaded_shadow_map_buffer_container::size() const noexcept
	{
		return m_depth_buffers.size();
	}
}