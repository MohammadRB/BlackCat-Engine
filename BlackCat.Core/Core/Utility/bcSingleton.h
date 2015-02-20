#pragma once

#include "Core/CorePCH.h"
#include "Core/bcCoreUtility.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_singleton;

		template < typename C, typename ...A >
		class bc_singleton< C(A...) > : public bc_initializable< A... >
		{
		private:
			using this_type = bc_singleton< C(A...) >;
			using base_type = bc_initializable< A... >;

		public:
			bc_singleton()
			{
			}

			bc_singleton(const this_type&) = delete;

			~bc_singleton()
			{
			}

			this_type& operator=(const this_type&) = delete;

			static void start_up(A... pArgs)
			{
				bcAssert(m_instance == nullptr);

				m_instance = new C(); // TODO
				m_instance->initialize(pArgs...);
			}

			static void close()
			{
				bcAssert(m_instance != nullptr);

				m_instance->destroy();
				delete m_instance;

				m_instance = nullptr;
			}

			static C& get()
			{
				bcAssert(m_instance != nullptr);
				return *m_instance;
			}

		protected:
			static C* m_instance;
		};

		template < typename C, typename ...A >
		C* bc_singleton< C(A...) >::m_instance = nullptr;
	}
}