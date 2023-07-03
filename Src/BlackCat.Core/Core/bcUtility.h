// [14/09/2016 MRB]

#pragma once

#include "Core/Math/bcCoordinate.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Utility/bcTemplateMetaType.h"
#include "Core/Utility/bcRandom.h"

namespace black_cat
{
	template< typename T >
	using _dereference_t = std::remove_reference_t<decltype(*std::declval<T>())>;

	template< typename TObject >
	auto bc_null_default(const TObject& p_object, _dereference_t<TObject> p_default) -> _dereference_t<TObject>
	{
		return p_object == nullptr ? p_default : *p_object;
	}

	inline constexpr bool bc_is_power_of_two(bcUINT p_number)
	{
		return p_number && !(p_number & (p_number - 1));
	}

	inline bcFLOAT bc_noise(bcFLOAT p_frequency, bcFLOAT p_intensity)
	{
		return std::cos(p_frequency * core::g_pi) *
				std::cos(p_frequency * core::g_pi) *
				std::cos(p_frequency * 3 * core::g_pi) *
				std::cos(p_frequency * 5 * core::g_pi) *
				0.5f +
				std::sin(p_frequency * 25 * core::g_pi) *
				p_intensity;
	}

	template<typename TIte>
	void bc_randomize_direction(core::bc_random& p_random, const core::bc_vector3f& p_direction, bcSIZE p_deviation, TIte p_destination, TIte p_destination_end)
	{
		const auto l_max_random = p_random.max();
		const auto l_rnd_coefficient = p_deviation / 180.f;
		
		while(p_destination != p_destination_end)
		{
			bcFLOAT l_randoms[3];
			for(auto l_rnd_ite = 0U; l_rnd_ite < 3; ++l_rnd_ite)
			{
				l_randoms[l_rnd_ite] = (p_random.generate() * 1.0f / l_max_random - 0.5f) * 2;
				l_randoms[l_rnd_ite] = l_randoms[l_rnd_ite] * l_rnd_coefficient;
			}

			auto l_new_direction = core::bc_vector3f
			(
				p_direction.x + l_randoms[0],
				p_direction.y + l_randoms[1],
				p_direction.z + l_randoms[2]
			);
			l_new_direction.normalize();

			*p_destination = l_new_direction;
			++p_destination;
		}
	}
}