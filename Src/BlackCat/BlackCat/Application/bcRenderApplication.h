// [03/23/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/bcEvent.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcExpressionParameterManager.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Event/bcEventManager.h"
#include "Core/File/bcContentManager.h"
#include "Core/File/bcContentStreamManager.h"
#include "Core/File/bcLazyContent.h"
#include "PlatformImp/bc_ide_logger.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "Game/Application/bcRenderApplication.h"
#include "Game/System/Render/bcRenderPassManager.h"
#include "Game/System/Render/bcMaterialManager.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "Game/Object/Scene/bcActorComponentManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcHierarchyComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcNameComponent.h"
#include "Game/System/bcGameSystem.h"
#include "BlackCat/Loader/bcMeshLoader.h"
#include "BlackCat/Loader/bcTextureLoader.h"
#include "BlackCat/Loader/bcVertexShaderLoader.h"
#include "BlackCat/Loader/bcHullShaderLoader.h"
#include "BlackCat/Loader/bcDomainShaderLoader.h"
#include "BlackCat/Loader/bcGeometryShaderLoader.h"
#include "BlackCat/Loader/bcPixelShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/BlackCatPCH.h"
#include "BlackCat/Loader/bcMeshColliderLoader.h"

namespace black_cat
{
	class bc_render_application : public game::bc_render_application
	{
	public:
		using game::bc_render_application::initialize;
		using game::bc_render_application::destroy;

	public:
		bc_render_application();

		virtual ~bc_render_application();

		virtual void application_start_engine_components(game::bc_engine_component_parameter& p_engine_components, core::bc_service_manager& p_service_manager) = 0;

		virtual void application_initialize(const bcCHAR* p_commandline) = 0;

		virtual void application_load_content(core::bc_content_stream_manager* p_stream_manager) = 0;

		virtual void application_update(core_platform::bc_clock::update_param p_clock_update_param) = 0;

		virtual void application_render(core_platform::bc_clock::update_param p_clock_update_param) = 0;

		virtual bool application_event(core::bc_ievent& p_event) = 0;

		virtual void application_unload_content(core::bc_content_stream_manager* p_stream_manager) = 0;

		virtual void application_destroy() = 0;

		virtual void application_close_engine_components() = 0;

	protected:
		core::bc_service_manager* m_service_manager;
		game::bc_game_system* m_game_system;

	private:
		void app_start_engine_components(game::bc_engine_component_parameter& p_engine_components) override final;

		void app_initialize(const bcCHAR* p_commandline) override final;

		void app_load_content() override final;

		void app_update(core_platform::bc_clock::update_param p_clock_update_param) override final;

		void app_render(core_platform::bc_clock::update_param p_clock_update_param) override final;

		bool app_event(core::bc_ievent& p_event) override final;

		void app_unload_content() override final;

		void app_destroy() override final;

		void app_close_engine_components() override final;
	};
}