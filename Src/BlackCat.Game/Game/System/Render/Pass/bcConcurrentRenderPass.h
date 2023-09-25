// [08/04/2022 MRB]

#pragma once

#include "Game/System/Render/bcRenderThread.h"
#include "Game/System/Render/Pass/bcRenderPass.h"

namespace black_cat::game
{
	struct bc_concurrent_render_pass_render_context : public bc_render_pass_render_context
	{
	public:
		bc_concurrent_render_pass_render_context(const bc_render_pass_render_context& p_context, bc_render_thread& p_child_render_thread)
			: bc_render_pass_render_context(p_context),
			  m_child_render_thread(p_child_render_thread)
		{
		}
			
		bc_render_thread& m_child_render_thread;
	};

	class bci_concurrent_render_pass : public bci_render_pass
	{
	public:
		bci_concurrent_render_pass() noexcept = default;

		bci_concurrent_render_pass(bci_concurrent_render_pass&&) noexcept = default;

		~bci_concurrent_render_pass() noexcept override = default;

		bci_concurrent_render_pass& operator=(bci_concurrent_render_pass&&) noexcept = default;

		void initialize_frame(const bc_render_pass_render_context& p_context) override final;

		void execute(const bc_render_pass_render_context& p_context) override final;

		virtual void execute(const bc_concurrent_render_pass_render_context& p_context) = 0;

		virtual void initialize_frame(const bc_concurrent_render_pass_render_context& p_context) = 0;
	};

	inline void bci_concurrent_render_pass::initialize_frame(const bc_render_pass_render_context& p_context)
	{
	}

	inline void bci_concurrent_render_pass::execute(const bc_render_pass_render_context& p_context)
	{
	}
}
