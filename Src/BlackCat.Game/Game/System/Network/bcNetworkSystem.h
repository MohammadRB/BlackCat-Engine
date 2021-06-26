// [05/27/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Concurrency/bcTask.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Utility/bcDelegate.h"
#include "Game/System/Network/bcNetworkManager.h"
#include "Game/System/Network/Client/bcNetworkClientManagerHook.h"
#include "Game/System/Network/Server/bcNetworkServerManagerHook.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor;
		
		class BC_GAME_DLL bc_network_system : public core::bc_initializable<>
		{
		private:
			using command_create_delegate = core::bc_delegate<bc_network_message_ptr()>;
			using message_factory_container = core::bc_unordered_map_program<bc_network_message_hash, command_create_delegate>;
			
		public:
			bc_network_system();

			bc_network_system(bc_network_system&&) noexcept;

			~bc_network_system();

			bc_network_system& operator=(bc_network_system&&) noexcept;

			void start_server(bcUINT16 p_port, bci_network_server_manager_hook& p_hook);

			void start_client(const bcCHAR* p_ip, bcUINT16 p_port, bci_network_client_manager_hook& p_hook);
			
			void add_actor(bc_actor& p_actor);
			
			void remove_actor(bc_actor& p_actor);

			bc_network_message_ptr create_message_instance(bc_network_message_hash p_hash);
			
			template<class TMessage>
			void send_message(TMessage p_command);
			
			void update(const core_platform::bc_clock::update_param& p_clock);
			
			core::bc_task<void> update_async(const core_platform::bc_clock::update_param& p_clock);

			template<class ...TMessage>
			void register_messages(TMessage... p_commands);
			
		private:
			void _initialize() override;
			
			void _destroy() override;

			template<class TCommand>
			void _register_message();

			message_factory_container m_message_factories;
			core::bc_unique_ptr<bci_network_manager> m_manager;
		};

		template<class TMessage>
		void bc_network_system::send_message(TMessage p_command)
		{
			m_manager->send_message(bc_make_network_message(p_command));
		}

		template<class ...TMessage>
		void bc_network_system::register_messages(TMessage...)
		{
			auto l_expansion_list =
			{
				(
					[this]()
					{
						_register_message<TMessage>();
						return true;
					}()
				)...
			};
		}

		template<class TMessage>
		void bc_network_system::_register_message()
		{
			static_assert(std::is_base_of_v<bci_network_message, TMessage>);
			static_assert(std::is_default_constructible_v<TMessage>);
			
			m_message_factories.insert(std::make_pair
			(
				bc_network_message_traits<TMessage>::command_hash(),
				command_create_delegate([]()
				{
					return bc_make_network_message(TMessage());
				})
			));
		}
	}
}