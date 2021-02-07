// [03/14/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcExport.h"

namespace black_cat
{
	namespace core
	{
		using bc_message_hash = bcSIZE;

		template< class TMessage >
		struct bc_message_traits
		{
			static constexpr const bcCHAR* message_name()
			{
				return TMessage::message_name();
			}

			static constexpr bcUINT32 message_hash()
			{
				return TMessage::message_hash();
			}
		};
		
		class BC_CORE_DLL bci_message
		{
		public:
			virtual ~bci_message();

			const bcCHAR* get_message_name() const noexcept;

			bc_message_hash get_message_hash() const noexcept;

			static bc_message_hash get_hash(const bcCHAR* p_name) noexcept;

			template< class TMessage >
			static bool is(const bci_message& p_message);

			template< class TMessage >
			static TMessage* as(bci_message& p_message);

			template< class TMessage >
			static const TMessage* as(const bci_message& p_message);

		protected:
			bci_message(const bcCHAR* p_name);

			bci_message(const bci_message&);

			bci_message& operator=(const bci_message&);
			
		private:
			const bcCHAR* m_name;
			mutable bc_message_hash m_hash;
		};

		template< class TMessage >
		bool bci_message::is(const bci_message& p_message)
		{
			constexpr auto l_hash = bc_message_traits<TMessage>::message_hash();
			return l_hash == p_message.get_message_hash();
		}

		template< class TMessage >
		TMessage* bci_message::as(bci_message& p_message)
		{
			if (is<TMessage>(p_message))
			{
				return static_cast<TMessage*>(&p_message);
			}

			return nullptr;
		}

		template< class TMessage >
		const TMessage* bci_message::as(const bci_message& p_message)
		{
			return as<TMessage>(const_cast<bci_message&>(p_message));
		}
	}
}