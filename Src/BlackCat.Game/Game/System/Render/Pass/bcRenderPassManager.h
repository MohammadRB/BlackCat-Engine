// [13/03/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
#include "Core/Utility/bcStopWatch.h"
#include "GraphicImp/Device/Query/bcDeviceTimeStampQuery.h"
#include "Game/System/Render/bcDeviceQuery.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/Pass/bcConcurrentRenderPass.h"
#include "Game/System/Render/Pass/bcRenderPassResourceManager.h"
#include "Game/System/Render/Pass/bcIntermediateTextureManager.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_render_system;

	struct _bc_render_pass_entry
	{
		using device_query = bc_device_query<graphic::bc_device_timestamp_query, 1>;

		const bcCHAR* m_name{ nullptr };
		bool m_is_concurrent{ false };
		bool m_is_enable{ true };
		core::bc_unique_ptr<bci_render_pass> m_pass;

		core::bc_stop_watch m_stop_watch;
		device_query m_device_query;
		core::bc_value_sampler<platform::bc_clock::small_time, 32> m_device_elapsed_sampler{ 0 };
	};

	class BC_GAME_DLL bc_render_pass_manager : public platform::bc_no_copy
	{
	public:
		bc_render_pass_manager(bc_render_system& p_render_system, bc_global_config& p_config) noexcept;

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

		template<class TPass>
		void enable_pass();

		template<class TPass>
		void disable_pass();

		void pass_initialize_resources(bc_render_system& p_render_system);

		void pass_update(const bc_render_pass_update_context& p_context);

		void pass_execute(const bc_render_pass_render_context& p_context);

		void before_reset(const bc_render_pass_reset_context& p_context);

		void after_reset(const bc_render_pass_reset_context& p_context);

		void config_changed(const bc_render_pass_config_change_context& p_context);

		void pass_destroy(bc_render_system& p_render_system);

		void pass_clear();

	private:
		void _add_pass(const bcCHAR* p_name, bool p_is_concurrent, core::bc_unique_ptr<bci_render_pass> p_pass, const bcCHAR* p_before = nullptr);

		bool _remove_pass(const bcCHAR* p_name);

		bool _enable_pass(const bcCHAR* p_name);

		bool _disable_pass(const bcCHAR* p_name);

		_bc_render_pass_entry* _get_pass(const bcCHAR* p_name);

		bc_render_system* m_render_system;
		bc_global_config* m_config;
		core::bc_vector<_bc_render_pass_entry> m_passes;
		bc_render_pass_resource_manager m_resource_manager;
		bc_intermediate_texture_manager m_texture_manager;

		_bc_render_pass_entry::device_query m_device_start_query;
		bool m_all_queries_fetched;
	};

	template<typename TPass>
	TPass* bc_render_pass_manager::get_pass()
	{
		static_assert(std::is_base_of_v<bci_render_pass, TPass>, "TPass must inherit from bc_irender_pass");

		auto* l_pass_entry = _get_pass(bc_render_pass_trait<TPass>::render_pass_name());
		return static_cast<TPass*>(l_pass_entry->m_pass.get());
	}
		
	template<typename TPass>
	void bc_render_pass_manager::add_pass(TPass p_pass)
	{
		static_assert(std::is_base_of_v<bci_render_pass, TPass>, "TPass must inherit from bc_irender_pass");

		_add_pass
		(
			bc_render_pass_trait<TPass>::render_pass_name(),
			std::is_base_of_v<bci_concurrent_render_pass, TPass>,
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
			std::is_base_of_v<bci_concurrent_render_pass, TPass>,
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

	template <class TPass>
	void bc_render_pass_manager::enable_pass()
	{
		static_assert(std::is_base_of_v<bci_render_pass, TPass>, "T must inherit from bc_irender_pass");

		_enable_pass(bc_render_pass_trait<TPass>::render_pass_name());
	}

	template <class TPass>
	void bc_render_pass_manager::disable_pass()
	{
		static_assert(std::is_base_of_v<bci_render_pass, TPass>, "T must inherit from bc_irender_pass");

		_disable_pass(bc_render_pass_trait<TPass>::render_pass_name());
	}
}
