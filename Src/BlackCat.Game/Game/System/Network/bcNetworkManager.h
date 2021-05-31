// [05/27/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcSpan.h"
#include "Game/System/Network/Command/bcNetworkCommand.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor;
		
		class bci_network_manager
		{
		public:
			virtual ~bci_network_manager() = default;
			
			virtual void add_actor(bc_actor& p_actor) = 0;

			virtual void remove_actor(bc_actor& p_actor) = 0;

			virtual void send_command(bc_network_command_ptr p_command);
			
			virtual void update(const core_platform::bc_clock& p_clock, core::bc_span<bc_actor> p_actors) = 0;
		};
	}	
}