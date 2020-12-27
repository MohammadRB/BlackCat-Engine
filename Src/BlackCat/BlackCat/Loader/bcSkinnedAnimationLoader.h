// [12/26/2020 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_skinned_animation_loader : public core::bc_base_content_loader
	{
	public:
		bc_skinned_animation_loader() = default;

		bc_skinned_animation_loader(bc_skinned_animation_loader&&) noexcept = default;

		~bc_skinned_animation_loader() = default;

		bc_skinned_animation_loader& operator=(bc_skinned_animation_loader&&) noexcept = default;
		
		bool support_offline_processing() const override;

		void content_offline_processing(core::bc_content_loading_context& p_context) const override;
		
		void content_processing(core::bc_content_loading_context& p_context) const override;
	};
}