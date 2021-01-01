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

			bc_animation_local_transform(bc_animation_local_transform&&) noexcept = default;

			~bc_animation_local_transform() = default;

			bc_animation_local_transform& operator=(bc_animation_local_transform&&) noexcept = default;

		private:
			bc_iterator_adapter::container_type m_transforms;
		};

		inline bc_animation_local_transform::bc_animation_local_transform()
			: bc_iterator_adapter(&m_transforms)
		{
		}
	}
}