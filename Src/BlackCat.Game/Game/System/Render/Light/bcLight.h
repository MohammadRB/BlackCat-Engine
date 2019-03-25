// [02/23/2019 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Utility/bcRefCountPtr.h"
#include "Game/System/Render/Light/bcDirectLight.h"
#include "Game/System/Render/Light/bcPointLight.h"
#include "Game/System/Render/Light/bcSpotLight.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_light;
		class bc_light_manager;

		class BC_GAME_DLL _bc_light_ptr_deleter
		{
		public:
			_bc_light_ptr_deleter();

			_bc_light_ptr_deleter(bc_light_manager* p_manager);

			void operator()(bc_light* p_light);

		private:
			bc_light_manager* m_manager;
		};

		enum class bc_light_type
		{
			direct,
			point,
			spot
		};

		class BC_GAME_DLL bc_light : core::bc_ref_count, core_platform::bc_no_copy
		{
			friend class bc_light_manager;

		public:
			bc_light(bc_light&& p_other) noexcept;

			~bc_light() = default;

			bc_light& operator=(bc_light&& p_other) noexcept;

			const core::bc_matrix4f& get_transformation() const
			{
				return m_transformation;
			}

			void set_transformation(const core::bc_matrix4f& p_transformation)
			{
				m_transformation = p_transformation;
			}

			bc_light_type get_type() const
			{
				return m_type;
			}

			bc_direct_light* as_direct_light();

			const bc_direct_light* as_direct_light() const;

			bc_point_light* as_point_light();

			const bc_point_light* as_point_light() const;

			bc_spot_light* as_spot_light();

			const bc_spot_light* as_spot_light() const;

		private:
			explicit bc_light(const bc_direct_light& p_light);

			explicit bc_light(const bc_point_light& p_light);

			explicit bc_light(const bc_spot_light& p_light);

			union
			{
				bc_direct_light m_direct_light;
				bc_point_light m_point_light;
				bc_spot_light m_spot_light;
			};
			bc_light_type m_type;
			core::bc_matrix4f m_transformation;
		};

		using bc_light_ptr = core::bc_ref_count_ptr<bc_light, _bc_light_ptr_deleter>;
	}
}