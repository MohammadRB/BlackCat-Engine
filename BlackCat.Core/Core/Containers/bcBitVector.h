#pragma once

#ifndef bcBitVector_H
#define bcBitVector_h

#include "CorePCH.h"
#include "bcContainer.h"

namespace BlackCat
{
	namespace Core
	{
#pragma region BitVector
		class BC_COREDLL_EXP bcBitVector : bcContainer_Base
		{
		private:

		protected:
			bcUINT32* mArray;
			bcUINT32 mSize;

		public:
			bcBitVector( bcINT32 p_size );
			~bcBitVector();

			void resize( bcINT32 pSize );
			bcBOOL operator[] ( bcINT32 pIndex );
			void set( bcINT32 pIndex, bcBOOL pValue );
			void falseAll();
			void trueAll();

			bcUINT32 size() { return mSize; }
		};
#pragma endregion
	}
}

#endif