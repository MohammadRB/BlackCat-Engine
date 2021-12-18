// [12/17/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Container/bcVector.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/Texture/bcTextureConfig.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_pass_reset_context;

		struct _bc_intermediate_texture
		{
			graphic::bc_texture2d_ref m_texture;
			graphic::bc_texture_config m_config;
			bool m_is_in_use;
		};

		class BC_GAME_DLL bc_intermediate_texture_manager
		{
		public:
			explicit bc_intermediate_texture_manager(graphic::bc_device& p_device) noexcept;

			bc_intermediate_texture_manager(bc_intermediate_texture_manager&&) noexcept;

			~bc_intermediate_texture_manager() = default;

			bc_intermediate_texture_manager& operator=(bc_intermediate_texture_manager&&) noexcept;

			/**
			 * \brief Try to find an intermediate texture with given config or create one and mark it as in use texture
			 * \n <b>ThreadSafe</b>
			 * \param p_config 
			 * \return 
			 */
			graphic::bc_texture2d get_texture(const graphic::bc_texture_config& p_config);

			/**
			 * \brief Give back the texture to be marked as not in use
			 * \param p_texture 
			 */
			void free_texture(graphic::bc_texture2d p_texture);

			void before_reset(const bc_render_pass_reset_context& p_context);

			void after_reset(const bc_render_pass_reset_context& p_context);
			
		private:
			graphic::bc_device* m_device;
			core_platform::bc_shared_mutex m_mutex;
			core::bc_vector<_bc_intermediate_texture> m_textures;
		};

		/**
		 * \brief RAII class for intermediate texture
		 */
		class bc_intermediate_texture_guard
		{
		public:
			bc_intermediate_texture_guard(bc_intermediate_texture_manager& p_manager, const graphic::bc_texture_config& p_config) noexcept;

			bc_intermediate_texture_guard(bc_intermediate_texture_guard&&) = default;

			~bc_intermediate_texture_guard();

			bc_intermediate_texture_guard& operator=(bc_intermediate_texture_guard&&) = default;

			graphic::bc_texture2d get_texture2d() const noexcept;

		private:
			bc_intermediate_texture_manager* m_manager;
			graphic::bc_texture2d m_texture;
		};

		inline graphic::bc_texture2d bc_intermediate_texture_guard::get_texture2d() const noexcept
		{
			return m_texture;
		}
	}
}