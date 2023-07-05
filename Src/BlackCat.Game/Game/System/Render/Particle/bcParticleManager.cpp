// [15/11/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/bcUtility.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Input/bcFileSystem.h"

namespace black_cat::game
{
	bc_particle_manager::bc_particle_manager()
		: m_curves{}
	{
		for (bcSIZE l_curve_i = 0; l_curve_i < bc_particle_builder::s_curves.size(); ++l_curve_i)
		{
			const auto* l_particle_curve = bc_particle_builder::s_curves[l_curve_i];
			core::bc_curve2f l_curve(std::cbegin(*l_particle_curve), std::cend(*l_particle_curve));
			// Sample at higher rate than actually needed to avoid repeated values in output array
			auto l_curve_samples = l_curve.sample_many(s_curve_sample_count * 2);
			curve_sample_container l_curve_samples_container;

			for (bcSIZE l_curve_sample_i = 0; l_curve_sample_i < s_curve_sample_count; ++l_curve_sample_i)
			{
				const auto l_time = static_cast<bcFLOAT>(l_curve_sample_i) / (s_curve_sample_count - 1);
				const auto l_read_sample = std::lower_bound
				(
					std::begin(l_curve_samples),
					std::end(l_curve_samples),
					core::bc_vector2f(l_time),
					[&](const core::bc_vector2f& p_sample1, const core::bc_vector2f& p_sample2)
					{
						return p_sample1.x < p_sample2.x;
					}
				);

				l_curve_samples_container[l_curve_sample_i] = l_read_sample->y;
			}

			m_curves[l_curve_i] = l_curve_samples_container;
		}
	}

	bc_particle_manager_container bc_particle_manager::create_container() const noexcept
	{
		return bc_particle_manager_container(*this);
	}

	const bc_particle_manager::curves_container& bc_particle_manager::get_curves() const noexcept
	{
		return m_curves;
	}

	bcFLOAT bc_particle_manager::sample_curve(bcSIZE p_curve_index, bcFLOAT p_normalized_time) const noexcept
	{
		p_normalized_time = std::min(1.f, p_normalized_time);
		const curve_sample_container& l_curve_samples = m_curves[p_curve_index];
		const bcFLOAT l_time = p_normalized_time * (s_curve_sample_count - 1);
		const bcSIZE l_lower_band = std::floor(l_time);
		const bcSIZE l_upper_band = std::ceil(l_time);

		const bcFLOAT l_lower_band_time = static_cast<bcFLOAT>(l_lower_band) / (s_curve_sample_count - 1);
		const bcFLOAT l_lower_band_sample = l_curve_samples[l_lower_band];
		const bcFLOAT l_upper_band_time = static_cast<bcFLOAT>(l_upper_band) / (s_curve_sample_count - 1);
		const bcFLOAT l_upper_band_sample = l_curve_samples[l_upper_band];

		const bcFLOAT l_normalized_time_between_samples = (p_normalized_time - l_lower_band_time) / (l_upper_band_time - l_lower_band_time + .001f);

		return l_normalized_time_between_samples * (l_upper_band_sample - l_lower_band_sample) + l_lower_band_sample;
	}

	core::bc_const_span<bc_particle_emitter_trait> bc_particle_manager::get_emitter_definition(const bcCHAR* p_name) const noexcept
	{
		const auto l_hash = string_hash_t()(p_name);
		const auto l_entry = m_emitter_definitions.find(l_hash);

		if(l_entry == std::end(m_emitter_definitions))
		{
			return {};
		}

		return core::bc_make_cspan(l_entry->second);
	}

	void bc_particle_manager::register_emitter_definition(const bcCHAR* p_name, const bc_particle_builder& p_builder)
	{
		core::bc_vector_program<bc_particle_emitter_trait> l_emitters(std::begin(p_builder.m_emitters), std::end(p_builder.m_emitters));

		auto l_hash = string_hash_t()(p_name);
		auto l_value = std::make_pair(l_hash, std::move(l_emitters));
		auto l_entry = m_emitter_definitions.find(l_hash);

		if (l_entry == std::end(m_emitter_definitions))
		{
			l_entry = m_emitter_definitions.insert(std::move(l_value)).first;
		}
		else
		{
			l_entry->second = std::move(l_value.second);
		}
	}
}
