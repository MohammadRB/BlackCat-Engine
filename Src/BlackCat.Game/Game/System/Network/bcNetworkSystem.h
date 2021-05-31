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
		using bc_actor_network_id = bcSIZE;
		
		enum class bc_network_manager_type : bcUBYTE
		{
			client, server
		};
		
		class BC_GAME_DLL bc_network_system : public core::bc_initializable<bc_network_manager_type>
		{
		private:
			using command_create_delegate = core::bc_delegate<bc_network_command_ptr()>;
			using commands_container = core::bc_unordered_map_program<bc_network_command_hash, command_create_delegate>;
			
		public:
			bc_network_system();

			bc_network_system(bc_network_system&&) noexcept;

			~bc_network_system();

			bc_network_system& operator=(bc_network_system&&) noexcept;

			void add_actor(bc_actor& p_actor);
			
			void remove_actor(bc_actor& p_actor);

			template<class TCommand>
			void send_command(TCommand p_command);
			
			void update(const core_platform::bc_clock::update_param& p_clock);

			template<class ...TCommand>
			void register_commands(TCommand... p_commands);
			
		private:
			void _initialize(bc_network_manager_type p_manager) override;
			
			void _destroy() override;

			template<class TCommand>
			void _register_command();

			core::bc_unique_ptr<bci_network_manager> m_manager;
			commands_container m_command_factories;
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