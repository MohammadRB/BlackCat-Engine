// [03/13/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "Game/bcExport.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/bcRenderThread.h"
#include "Game/System/Render/Pass/bcRenderPassResourceShare.h"

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
			bcUINT32 m_position;
			const bcCHAR* m_name;
			core::bc_unique_ptr< bc_irender_pass > m_pass;
		};

		class BC_GAME_DLL bc_render_pass_manager : public core_platform::bc_no_copy
		{
		public:
			bc_render_pass_manager();

			bc_render_pass_manager(bc_render_pass_manager&&) noexcept = default;
			
			~bc_render_pass_manager();

			bc_render_pass_manager& operator=(bc_render_pass_manager&&) noexcept = default;

			template< typename T >
			void add_pass(bcUINT32 p_location, T&& p_pass);

			template< typename T >
			T* get_pass();

			bc_irender_pass* get_pass(bcUINT32 p_location);

			template< typename T >
			bool remove_pass();

			bool remove_pass(bcUINT32 p_location);

			void pass_initialize_resources(bc_render_system& p_render_system);

			void pass_update(const bc_render_pass_update_context& p_param);

			void pass_execute(const bc_render_pass_render_context& p_param);

			void before_reset(const bc_render_pass_reset_context& p_param);

			void after_reset(const bc_render_pass_reset_context& p_param);

			void pass_destroy(bc_render_system& p_render_system);

		private:
			void _add_pass(_bc_render_pass_entry&& p_entry);

			bool _remove_pass(const bcCHAR* p_name);

			bc_irender_pass* _get_pass(const bcCHAR* p_name);

			core::bc_vector< _bc_render_pass_entry > m_passes;
			bc_render_pass_resource_share m_state_share;
		};

		template< typename T >
		void bc_render_pass_manager::add_pass(bcUINT32 p_location, T&& p_pass)
		{
			static_assert(std::is_base_of_v<bc_irender_pass, T>, "T must inherit from bc_irender_pass");

			_bc_render_pass_entry l_entry;
			l_entry.m_position = p_location;
			l_entry.m_name = bc_render_pass_trait<T>::render_pass_name();
			l_entry.m_pass = core::bc_make_unique<T>(core::bc_alloc_type::program, std::move(p_pass));

			_add_pass(std::move(l_entry));
		}

		template< typename T >
		bool bc_render_pass_manager::remove_pass()
		{
			static_assert(std::is_base_of_v<bc_irender_pass, T>, "T must inherite from bc_irender_pass");

			return _remove_pass(bc_render_pass_trait<T>::render_pass_name());
		}

		template< typename T >
		T* bc_render_pass_manager::get_pass()
		{
			static_assert(std::is_base_of_v<bc_irender_pass, T>, "T must inherite from bc_irender_pass");

			return static_cast< T* >(_get_pass(bc_render_pass_trait<T>::render_pass_name()));
		}
	}
}