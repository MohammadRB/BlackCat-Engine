// [05/28/2021 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Messaging/bcMessage.h"
#include "Core/File/bcJsonDocument.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		using bc_network_message_id = bcUINT32;
		using bc_network_message_hash = core::bc_string_cmp_hash;

		template<class TCommand>
		struct bc_network_message_traits
		{
			static constexpr const bcCHAR* command_name()
			{
				return TCommand::command_name();
			}

			static constexpr bc_network_message_hash command_hash()
			{
				return TCommand::command_hash();
			}
		};

		class bc_network_system;
		class bci_network_message;
		using bc_network_message_ptr = core::bc_shared_ptr<bci_network_message>;

		template<class TCommand>
		bc_network_message_ptr bc_make_network_message(TCommand p_command)
		{
			return core::bc_make_shared<TCommand>(std::move(p_command));
		}
		
		struct bc_network_message_client_context
		{
		};

		struct bc_network_message_server_context
		{
		};
		
		class bci_network_message : public core::bci_message
		{
		public:
			~bci_network_message() override = default;

			bc_network_message_id get_id() const noexcept;
			
			/**
			 * \brief Serialize command into json key/value pair which is provided as parameter
			 * \param p_params 
			 */
			void serialize(core::bc_json_key_value& p_params) const;

			/**
			 * \brief Deserialize command from json key/value pair which is provided as parameter. 
			 * \param p_params
			 */
			void deserialize(const core::bc_json_key_value& p_params);

			/**
			 * \brief Indicate whether message must be acknowledged by the receiver or not
			 * \return 
			 */
			virtual bool need_acknowledgment() const noexcept;
			
			/**
			 * \brief Execute message logic on the remote part of connection
			 * \param p_context
			 */
			virtual void execute(const bc_network_message_client_context& p_context);

			/**
			 * \brief Execute message logic on the remote part of connection
			 * \param p_context
			 */
			virtual void execute(const bc_network_message_server_context& p_context);

			/**
			 * \brief Execute message logic when message delivery is acknowledged
			 * \param p_context 
			 */
			virtual void acknowledge(const bc_network_message_client_context& p_context);

			/**
			 * \brief Execute message logic when message delivery is acknowledged
			 * \param p_context
			 */
			virtual void acknowledge(const bc_network_message_server_context& p_context);
		
		protected:
			explicit bci_network_message(const bcCHAR* p_name) noexcept
				: bci_message(p_name),
				m_id(0)
			{
			}

			bci_network_message(bci_network_message&&) noexcept = default;

			bci_network_message& operator=(bci_network_message&&) noexcept = default;

		private:
			virtual void serialize_message(core::bc_json_key_value& p_params) const = 0;

			virtual void deserialize_message(const core::bc_json_key_value& p_params) = 0;

			static core_platform::bc_atomic<bc_network_message_id> s_id_counter;
			
			mutable bc_network_message_id m_id;
		};

		inline bc_network_message_id bci_network_message::get_id() const noexcept
		{
			return m_id;
		}

		inline bool bci_network_message::need_acknowledgment() const noexcept
		{
			return false;
		}
	}
}