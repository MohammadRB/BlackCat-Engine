// [12/31/2020 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Container/bcIteratorAdapter.h"

#include "3rdParty/Ozz/Include/ozz/base/maths/soa_transform.h"

namespace black_cat
{
	namespace game
	{
		class bc_animation_local_transform : public core::bc_iterator_adapter< core::bc_vector_movable< ozz::math::SoaTransform > >
		{
		public:
			bc_animation_local_transform();

			explicit bc_animation_local_transform(container_type::size_type p_size);

			bc_animation_local_transform(bc_animation_local_transform&& p_other) noexcept;

			~bc_animation_local_transform();

			bc_animation_local_transform& operator=(bc_animation_local_transform&& p_other) noexcept;

			void resize(container_type::size_type p_size);
		
		private:
			container_type m_transforms;
		};

		inline bc_animation_local_transform::bc_animation_local_transform()
			: bc_iterator_adapter(m_transforms)
		{
		}

		inline bc_animation_local_transform::bc_animation_local_transform(container_type::size_type p_size)
			: bc_iterator_adapter(m_transforms)
		{
			resize(p_size);
		}

		inline bc_animation_local_transform::bc_animation_local_transform(bc_animation_local_transform&& p_other) noexcept
			: bc_iterator_adapter(m_transforms),
			m_transforms(std::move(p_other.m_transforms))
		{
		}

		inline bc_animation_local_transform::~bc_animation_local_transform() = default;

		inline bc_animation_local_transform& bc_animation_local_transform::operator=(bc_animation_local_transform&& p_other) noexcept
		{
			m_transforms = std::move(p_other.m_transforms);
			return *this;
		}

		inline void bc_animation_local_transform::resize(container_type::size_type p_size)
		{
			m_transforms.resize(p_size);
		}
	}
}