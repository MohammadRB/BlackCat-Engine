// [11/11/2020 MRB]

#pragma once

#include "Core/Math/bcCurve.h"
#include "Core/Container/bcArray.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcSpan.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Game/System/Render/Particle/bcParticleBuilder.h"
#include "Game/System/Render/Particle/bcParticleManagerContainer.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
	}
	
	namespace game
	{
		/**
		 * \brief
		 * \b ThreadSafe
		 */
		class BC_GAME_DLL bc_particle_manager
		{
		private:
			using string_hash_t = std::hash<const bcCHAR*>;
			using particle_definition_container = core::bc_unordered_map_program<string_hash_t::result_type, core::bc_vector_program<bc_particle_emitter_trait>>;
			
		public:
			static constexpr bcSIZE s_curve_sample_count = 10;
			static constexpr bcSIZE s_curve_count = bc_particle_builder::s_curves.size();
			using curve_sample_container = core::bc_array<bcFLOAT, s_curve_sample_count>;
			using curves_container = core::bc_array<curve_sample_container, s_curve_count>;
			
		public:
			bc_particle_manager();

			bc_particle_manager(bc_particle_manager&&) noexcept = default;

			~bc_particle_manager() = default;

			bc_particle_manager& operator=(bc_particle_manager&&) noexcept = default;

			bc_particle_manager_container create_container() const noexcept;

			const curves_container& get_curves() const noexcept;

			bcFLOAT sample_curve(bcSIZE p_curve_index, bcFLOAT p_normalized_time) const noexcept;

			core::bc_const_span<bc_particle_emitter_trait> get_emitter_definition(const bcCHAR* p_name) const noexcept;

			void register_emitter_definition(const bcCHAR* p_name, const bc_particle_builder& p_builder);

		private:
			curves_container m_curves;
			particle_definition_container m_emitter_definitions;
		};
	}
}