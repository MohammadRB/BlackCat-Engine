// [03/13/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcStopWatch.h"
#include "GraphicImp/Device/bcDevice.h"
#include "Game/bcExport.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/Pass/bcRenderPassResourceManager.h"
#include "Game/System/Render/Pass/bcIntermediateTextureManager.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_system;
		class bc_scene;
		struct bc_render_system_update_context;

		struct _bc_render_pass_entry
		{
		public:
			const bcCHAR* m_name;
			core::bc_unique_ptr<bci_render_pass> m_pass;
			core::bc_unique_ptr<core::bc_stop_watch> m_stop_watch;
		};

		class BC_GAME_DLL bc_render_pass_manager : public core_platform::bc_no_copy
		{
		public:
			explicit bc_render_pass_manager(graphic::bc_device& p_device) noexcept;

			bc_render_pass_manager(bc_render_pass_manager&&) noexcept;
			
			~bc_render_pass_manager();

			bc_render_pass_manager& operator=(bc_render_pass_manager&&) noexcept;

			template<typename TPass>
			TPass* get_pass();

			template<typename TPass>
			void add_pass(TPass p_pass);

			template<typename TPass, class TPassBefore>
			void add_pass_before(TPass p_pass);

			template<typename TPass>
			bool remove_pass();

			void pass_initialize_resources(bc_render_system& p_render_system);

			void pass_update(const bc_render_pass_update_context& p_context);

			void pass_execute(const bc_render_pass_render_context& p_context);

			void before_reset(const bc_render_pass_reset_context& p_context);

			void after_reset(const bc_render_pass_reset_context& p_context);

			void config_changed(const bc_render_pass_config_change_context& p_context);

			void pass_destroy(bc_render_system& p_render_system);

			void pass_clear();

		private:
			void _add_pass(const bcCHAR* p_name, core::bc_unique_ptr<bci_render_pass> p_pass, const bcCHAR* p_before = nullptr);

			bool _remove_pass(const bcCHAR* p_name);

			bci_render_pass* _get_pass(const bcCHAR* p_name);

			core::bc_vector<_bc_render_pass_entry> m_passes;
			bc_render_pass_resource_manager m_resource_manager;
			bc_intermediate_texture_manager m_texture_manager;
		};

		template<typename TPass>
		TPass* bc_render_pass_manager::get_pass()
		{
			static_assert(std::is_base_of_v<bci_render_pass, TPass>, "TPass must inherit from bc_irender_pass");

			return static_cast<TPass*>(_get_pass(bc_render_pass_trait<TPass>::render_pass_name()));
		}
		
		template<typename TPass>
		void bc_render_pass_manager::add_pass(TPass p_pass)
		{
			static_assert(std::is_base_of_v<bci_render_pass, TPass>, "TPass must inherit from bc_irender_pass");

			_add_pass
			(
				bc_render_pass_trait<TPass>::render_pass_name(), 
				core::bc_make_unique<TPass>(core::bc_alloc_type::program, std::move(p_pass))
			);
		}

		template<typename TPass, class TPassBefore>
		void bc_render_pass_manager::add_pass_before(TPass p_pass)
		{
			static_assert(std::is_base_of_v<bci_render_pass, TPass>, "TPass must inherit from bc_irender_pass");

			_add_pass
			(
				bc_render_pass_trait<TPass>::render_pass_name(),
				core::bc_make_unique<TPass>(core::bc_alloc_type::program, std::move(p_pass)),
				bc_render_pass_trait<TPassBefore>::render_pass_name()
			);
		}

		template<typename TPass>
		bool bc_render_pass_manager::remove_pass()
		{
			static_assert(std::is_base_of_v<bci_render_pass, TPass>, "T must inherit from bc_irender_pass");

			return _remove_pass(bc_render_pass_trait<TPass>::render_pass_name());
		}
	}
}