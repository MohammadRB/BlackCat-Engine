// [05/28/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Messaging/bcMessage.h"
#include "Core/File/bcJsonDocument.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		using bc_network_command_hash = bcUINT32;

		template<class TCommand>
		struct bc_network_command_traits
		{
			static constexpr const bcCHAR* command_name()
			{
				return TCommand::command_name();
			}

			static constexpr bc_network_command_hash command_hash()
			{
				return TCommand::command_hash();
			}
		};

		class bc_network_system;
		class bci_network_command;
		using bc_network_command_ptr = core::bc_unique_ptr<bci_network_command>;

		template<class TCommand>
		bc_network_command_ptr bc_make_network_command(TCommand p_command)
		{
			return core::bc_make_unique<TCommand>(std::move(p_command));
		}
		
		struct bc_network_command_context
		{
		};
		
		class bci_network_command : public core::bci_message
		{
		public:
			~bci_network_command() override = default;

			/**
			 * \brief Serialize command into json key/value pair which is provided as parameter
			 * \param p_params 
			 */
			virtual void serialize(core::bc_json_key_value& p_params) const = 0;

			/**
			 * \brief deserialize command from json key/value pair which is provided as parameter. 
			 * \param p_params
			 */
			virtual void deserialize(const core::bc_json_key_value& p_params) = 0;

			/**
			 * \brief Execute command logic on the remote part of connection
			 * \param p_context 
			 */
			virtual void execute(const bc_network_command_context& p_context) = 0;
		
		protected:
			explicit bci_network_command(const bcCHAR* p_name) noexcept
				: bci_message(p_name)
			{
				
			}

			bci_network_command(bci_network_command&&) noexcept = default;

			bci_network_command& operator=(bci_network_command&&) noexcept = default;
		};
	}
}