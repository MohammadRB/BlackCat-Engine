// [06/27/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Math/bcVector3f.h"
#include "GraphicImp/Resource/View/bcResourceView.h"

namespace black_cat
{
	class bc_cascaded_shadow_map_buffer_container
	{
		using entry = std::tuple<core::bc_vector3f, graphic::bc_resource_view>;

	public:
		bcSIZE size() const noexcept;

		graphic::bc_resource_view get(const core::bc_vector3f& p_light_direction) const noexcept;

		void add(const core::bc_vector3f& p_light_direction, const graphic::bc_resource_view& p_buffer);

		void clear();

	private:
		core::bc_vector<entry> m_depth_buffers;
	};

	inline bcSIZE bc_cascaded_shadow_map_buffer_container::size() const noexcept
	{
		return m_depth_buffers.size();
	}

	inline graphic::bc_resource_view bc_cascaded_shadow_map_buffer_container::get(const core::bc_vector3f& p_light_direction) const noexcept
	{
		const auto l_iterator = std::find_if(std::cbegin(m_depth_buffers), std::cend(m_depth_buffers), [&](const entry& p_entry)
		{
			return std::get<core::bc_vector3f>(p_entry) == p_light_direction;
		});

		if(l_iterator == std::cend(m_depth_buffers))
		{
			return graphic::bc_resource_view();
		}

		return std::get<graphic::bc_resource_view>(*l_iterator);
	}

	inline void bc_cascaded_shadow_map_buffer_container::add(const core::bc_vector3f& p_light_direction, const graphic::bc_resource_view& p_buffer)
	{
		m_depth_buffers.push_back(std::make_tuple(p_light_direction, p_buffer));
	}

	inline void bc_cascaded_shadow_map_buffer_container::clear()
	{
		m_depth_buffers.clear();
	}
}