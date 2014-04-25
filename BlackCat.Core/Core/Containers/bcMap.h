#pragma once

#ifndef bcMap_H
#define bcMap_H

#include "CorePCH.h"
#include "bcContainer.h"
#include "bcTree.h"

namespace BlackCat
{
	namespace Core
	{
		template < class KT, typename T, class CMP = bcLess< KT >, class Allocator = bcContainerAllocation<> >
		class bcMap : public bcTree < KT, T, 2, CMP, Allocator >
		{
			typedef bcMap < KT, T, CMP, Allocator > ThisType;
		private:

		protected:
			
		public:
			bcMap() {};
			~bcMap() {};
		};
	}
}

#endif