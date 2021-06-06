// [05/27/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Utility/bcDelegate.h"
#include "Game/System/Network/bcNetworkManager.h"
#include "Game/System/Network/Command/bcNetworkCommand.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bci_network_client_manager_hook;
		
		using bc_actor_network_id = bcSIZE;
		
		enum class bc_network_manager_type : bcUBYTE
		{
			client, server
		};
		
		class BC_GAME_DLL bc_network_system : public core::bc_initializable<>
		{
		private:
			using command_create_delegate = core::bc_delegate<bc_network_command_ptr()>;
			using commands_container = core::bc_unordered_map_program<bc_network_command_hash, command_create_delegate>;
			
		public:
			bc_network_system();

			bc_network_system(bc_network_system&&) noexcept;

			~bc_network_system();

			bc_network_system& operator=(bc_network_system&&) noexcept;

			void start_server(bcUINT16 p_port);

			void start_client(const bcCHAR* p_ip, bcUINT16 p_port, bci_network_client_manager_hook& p_hook);
			
			void add_actor(bc_actor& p_actor);
			
			void remove_actor(bc_actor& p_actor);

			bc_network_command_ptr create_command_instance(bc_network_command_hash p_hash);
			
			template<class TCommand>
			void send_command(TCommand p_command);
			
			void update(const core_platform::bc_clock::update_param& p_clock);

			template<class ...TCommand>
			void register_commands(TCommand... p_commands);
			
		private:
			void _initialize() override;
			
			void _destroy() override;

			template<class TCommand>
			void _register_command();

			commands_container m_command_factories;
			bc_network_manager_type m_manager_type;
			core::bc_unique_ptr<bci_network_manager> m_manager;
		};

		template<class TCommand>
		void bc_network_system::send_command(TCommand p_command)
		{
			m_manager->send_command(bc_make_network_command(p_command));
		}

		template<class ...TCommand>
		void bc_network_system::register_commands(TCommand...)
		{
			auto l_expansion_list =
			{
				(
					[this]()
					{
						_register_command<TCommand>();
						return true;
					}()
				)...
			};
		}

		template<class TCommand>
		void bc_network_system::_register_command()
		{
			static_assert(std::is_base_of_v<bci_network_command, TCommand>);
			static_assert(std::is_default_constructible_v<TCommand>);
			
			m_command_factories.insert(std::make_pair
			(
				bc_network_command_traits<TCommand>::command_hash(),
				command_create_delegate([]()
				{
					return bc_make_network_command(TCommand());
				})
			));
		}
	}
}