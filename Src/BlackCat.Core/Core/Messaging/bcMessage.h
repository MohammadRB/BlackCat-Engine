// [03/14/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcExport.h"

namespace black_cat
{
	namespace core
	{
		using bc_message_hash = bcUINT32;

		template< class TEvent >
		struct bc_message_traits
		{
			static constexpr const bcCHAR* message_name()
			{
				return TEvent::message_name();
			}

			static constexpr bcUINT32 message_hash()
			{
				return TEvent::message_hash();
			}
		};
		
		class BC_CORE_DLL bc_imessage
		{
		public:
			virtual ~bc_imessage();

			virtual const bcCHAR* get_name() const noexcept;

			virtual bc_message_hash get_hash() const noexcept;

			static bc_message_hash get_hash(const bcCHAR* p_name) noexcept;

			template< class TMessage >
			static bool is(const bc_imessage& p_message);

			template< class TMessage >
			static TMessage* as(bc_imessage& p_message);

			template< class TMessage >
			static const TMessage* as(const bc_imessage& p_message);

		protected:
			bc_imessage(const bcCHAR* p_name);

			bc_imessage(const bc_imessage&);

			bc_imessage& operator=(const bc_imessage&);
			
		private:
			const bcCHAR* m_name;
			mutable bc_message_hash m_hash;
		};

		template< class TMessage >
		bool bc_imessage::is(const bc_imessage& p_message)
		{
			constexpr auto l_hash = bc_message_traits<TMessage>::message_hash();
			return l_hash == p_message.get_hash();
		}

		template< class TMessage >
		TMessage* bc_imessage::as(bc_imessage& p_message)
		{
			if (is<TMessage>(p_message))
			{
				return static_cast<TMessage*>(p_message);
			}

			return nullptr;
		}

		template< class TMessage >
		const TMessage* bc_imessage::as(const bc_imessage& p_message)
		{
			return as<TMessage>(const_cast<bc_imessage&>(p_message));
		}
	}
}