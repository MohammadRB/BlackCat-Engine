#include "CorePCH.h"
#include "CoreExceptions.h"
#include "bcBitVector.h"

namespace BlackCat
{
	namespace Core
	{
		bcBitVector::bcBitVector( bcINT32 pSize )
		{
			mArray = 0;
			mSize = 0;
			resize( pSize );
		}
		bcBitVector::~bcBitVector()
		{
			if( mArray != 0 )
				free( mArray );
			mArray = 0;
		}
		void bcBitVector::resize( bcINT32 pSize )
		{
			bcUINT32* lNewVector = 0;
			if( pSize % 32 == 0 )
				pSize = pSize / 32;
			else
				pSize = (pSize / 32) + 1;
			lNewVector = (bcUINT32*)malloc( pSize * sizeof(bcUINT32));
			if( lNewVector == 0 )
				throw Exception(L"Error in BitVector.CPP: Can'nt resize vector");
			bcINT32 min;
			if( pSize < mSize )
				min = pSize;
			else
				min = mSize;
			/*bcINT32 lIndex;
			for( lIndex = 0; lIndex < min; lIndex++ )
				lNewVector[lIndex] = mArray[lIndex];*/
			memcpy(lNewVector, mArray, min * sizeof(bcUINT32));
			mSize = pSize;
			if( mArray != 0 )
				free( mArray );
			mArray = lNewVector;
		}
		bcBOOL bcBitVector::operator[] ( bcINT32 pIndex )
		{
			bcINT32 lCell = pIndex / 32;
			bcINT32 lBit = pIndex % 32;
			return (mArray[lCell] & (1 << lBit)) >> lBit;
		}
		void bcBitVector::set( bcINT32 pIndex, bcBOOL pValue )
		{
			bcINT32 lCell = pIndex / 32;
			bcINT32 lBit = pIndex % 32;
			if( pValue == true )
				mArray[lCell] = (mArray[lCell] | (1 << lBit));
			else
				mArray[lCell] = (mArray[lCell] & (~(1 << lBit)));
		}
		void bcBitVector::falseAll()
		{
			/*bcINT32 lIndex;
			for( lIndex = 0; lIndex < mSize; lIndex++ )
				mArray[lIndex] = 0;*/
			memset(mArray, 0, mSize * sizeof(bcINT32));
		}
		void bcBitVector::trueAll()
		{
			/*bcINT32 lIndex;
			for( lIndex = 0; lIndex < mSize; lIndex++ )
				mArray[lIndex] = 0xFFFFFFFF;*/
			memset(mArray, 1, mSize * sizeof(bcINT32));
		}
	}
}