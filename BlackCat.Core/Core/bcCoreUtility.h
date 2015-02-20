#pragma once

#include "Core/CorePCH.h"
#include "CorePlatform/bcCorePlatformUtility.h"

namespace black_cat
{
	namespace core
	{	
		template< typename ...A >
		class bc_initializable
		{
		public:
			bc_initializable() 
				: m_initialized(false)
			{
			}

			virtual ~bc_initializable()
			{
				bcAssert(!m_initialized);
			}

			virtual void initialize(A... pArgs) = 0;

			virtual void destroy() = 0;

		protected:
			bool m_initialized;

		private:
		};
	}
}