// [05/27/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"

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
			
			virtual void send_message(bc_network_message_ptr p_message) = 0;

			virtual void update(const core_platform::bc_clock::update_param& p_clock) = 0;

			template<class TMessage, typename = std::enable_if_t<!std::is_same_v<TMessage, bc_network_message_ptr>>>
			void send_message(TMessage p_message);
		};

		template<class TMessage, typename>
		void bci_network_manager::send_message(TMessage p_message)
		{
			send_message(bc_make_network_message(std::move(p_message)));
		}
	}
}