// [05/28/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
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
		
		class bci_network_command
		{
			friend class bc_network_system;
			
		public:
			~bci_network_command() = default;
			
			const bcCHAR* get_command_name() const noexcept;

			bc_network_command_hash get_command_hash() const noexcept;
		
		protected:
			explicit bci_network_command(const bcCHAR* p_name) noexcept;

			bci_network_command(bci_network_command&&) noexcept = default;

			bci_network_command& operator=(bci_network_command&&) noexcept = default;

			void serialize(core::bc_json_key_value& p_params) const;

			void deserialize(const core::bc_json_key_value& p_params);

			virtual void execute(const bc_network_command_context& p_context) = 0;
			
			virtual void serialize_command(core::bc_json_key_value& p_params) const = 0;

			virtual void deserialize_command(const core::bc_json_key_value& p_params) = 0;

		private:
			const bcCHAR* m_name;
			mutable bc_network_command_hash m_hash;
		};
	}
}