// [03/11/2022 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_sound_loader : public core::bc_base_content_loader
	{
	public:
		bc_sound_loader() noexcept = default;

		bc_sound_loader(bc_sound_loader&&) noexcept = default;

		~bc_sound_loader() override = default;

		bc_sound_loader& operator=(bc_sound_loader&&) noexcept = default;

		bool support_offline_processing() const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;
	};
}