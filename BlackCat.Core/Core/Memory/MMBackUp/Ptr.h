//  [8/9/2013 MRB]

#pragma once

#ifndef Ptr_H
#define Ptr_H

#include "CorePCH.h"
#include "CoreOS.h"
#include "MemBlock.h"
//#include "bcMemory.h"

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

		template<typename T> class Ptr;
		template<> class Ptr<void>;

		// == Ptr ===================================================================================== MRB ==
#pragma region Ptr
		template < typename T > class Ptr
		{
		private:

		protected:
			T* mPointer;

			bcInline bcBOOL _isOnStack() const
			{
				return bcCoreOS::isPointerOnStack(mPointer);
			}
			bcInline MemBlock* _getMemBlock() const
			{
				return MemBlock::retrieveMemBlock(mPointer);
			}
			bcInline void _construct(void* pPointer)
			{
				mPointer = reinterpret_cast<T*>(pPointer);

				MemBlock* lMemBlock = _getMemBlock();
				if(lMemBlock != bcNULL)
				{
					lMemBlock->refrenceCount(lMemBlock->refrenceCount() + 1);
					if(lMemBlock->dynamicPointer())
					{
						//bcMemMng::get()->registerPointerInDynamicAllocators(*this);
					}
				}
			}
			bcInline void _deconstruct()
			{
				MemBlock* lMemBlock = _getMemBlock();
				if(lMemBlock != bcNULL)
				{
					lMemBlock->refrenceCount(lMemBlock->refrenceCount() - 1);
					if(lMemBlock->refrenceCount() == 0)
					{
						if(lMemBlock->alignment() > 16)
						{
							//bcAlignedFree(*this);
						}
						else
						{
							//bcFree(*this);
						}
					}
					else if(lMemBlock->dynamicPointer())
					{
						//bcMemMng::get()->unRegisterPointerInDynamicAllocators(*this);
					}
				}
				mPointer = bcNULL;
			}
			template<typename T1>
			bcInline Ptr<T>& _assign(const Ptr<T1>& pOther)
			{
				if (this != &pOther) // Avoid self assignment
				{
					_deconstruct();
				}

				_construct(pOther.get());

				return *this;
			}
			bcInline bcBYTE _compare(const void* pPointer) const
			{
				return (mPointer == pPointer) ? 0 : ((mPointer > pPointer) ? 1 : ((mPointer < pPointer) ? -1 : 0));
			}

		public:

			Ptr()
			{
				mPointer = bcNULL;
			}

			Ptr(T* pPointer)
			{
				_construct(pPointer);
			}

			Ptr(const Ptr<T>& pOther)
			{
				_construct(pOther.get());
			}

			template< typename T1 >
			Ptr(const Ptr<T1>& pOther)
			{
				_construct(pOther.get());
			}

			Ptr(const Ptr<void>& pOther)
			{
				_construct(pOther.get());
			}

			~Ptr()
			{
				_deconstruct();
			}

			bcInline T& operator* () const
			{
#ifdef BC_MEMORY_DEBUG
				MemBlock* lMemBlock = _getMemBlock();
				bcAssert((lMemBlock != bcNULL && lMemBlock->free() != bcTRUE));
#endif
				return *mPointer;
			}

			bcInline T* operator-> () const
			{
#ifdef BC_MEMORY_DEBUG
				MemBlock* lMemBlock = _getMemBlock();
				bcAssert((lMemBlock != bcNULL && lMemBlock->free() != bcTRUE));
#endif
				return mPointer;
			}

			bcInline T* get() const
			{
				return mPointer;
			}

			Ptr<T>& operator =(std::nullptr_t hp)
			{
				_deconstruct();
				return *this;
			}
    
			Ptr<T>& operator =(const Ptr<T>& pOther)
			{
				return _assign(pOther);
			}

			template< typename T1 >
			Ptr<T>& operator =(const Ptr<T1>& pOther)
			{
				return _assign(pOther);
			}

			Ptr<T>& operator =(const Ptr<void>& pOther)
			{
				return _assign(pOther);
			}

			bcInline bcBOOL operator ==( const Ptr<T>& pOther ) const
			{
				return (_compare(pOther.get()) == 0);
			}

			bcInline bcBOOL operator ==( const T* pOther ) const
			{
				return (_compare(pOther) == 0);
			}

			bcInline bcBOOL operator !=( const Ptr<T>& pOther ) const
			{
				return !(_compare(pOther.get()) == 0);
			}

			bcInline bcBOOL operator !=( const T* pOther ) const
			{
				return !(_compare(pOther) == 0);
			}

			bcInline bcBOOL operator <( const Ptr<T>& pOther ) const
			{
				return (_compare(pOther.get()) == -1);
			}

			bcInline bcBOOL operator <( const T* pOther ) const
			{
				return (_compare(pOther) == -1);
			}

			bcInline bcBOOL operator <=( const Ptr<T>& pOther ) const
			{
				bcBYTE flag = _compare(pOther.get());
				return (flag == -1 || flag == 0);
			}

			bcInline bcBOOL operator <=( const T* pOther ) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == -1 || flag == 0);
			}

			bcInline bcBOOL operator >( const Ptr<T>& pOther ) const
			{
				return (_compare(pOther.get()) == 1);
			}

			bcInline bcBOOL operator >( const T* pOther ) const
			{
				return (_compare(pOther) == 1);
			}

			bcInline bcBOOL operator >=( const Ptr<T>& pOther ) const
			{
				bcBYTE flag = _compare(pOther.get());
				return (flag == 1 || flag == 0);
			}

			bcInline bcBOOL operator >=( const T* pOther ) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == 1 || flag == 0);
			}

			bcInline void operator ++()
			{
				mPointer++;
			}

			bcInline void operator ++(bcINT32)
			{
				++mPointer;
			}

			bcInline void operator --()
			{
				mPointer--;
			}

			bcInline void operator --(bcINT32)
			{
				--mPointer;
			}

			bcInline Ptr<T> operator +(bcINT32 pValue) const
			{
				return mPointer + sizeof(T) * pValue;
			}

			bcInline Ptr<T>& operator +=(bcINT32 pValue)
			{
				mPointer += sizeof(T) * pValue;
				return *this;
			}

			bcInline Ptr<T> operator -(bcINT32 pValue) const
			{
				return mPointer - sizeof(T) * pValue;
			}

			bcInline Ptr<T>& operator -=(bcINT32 pValue)
			{
				mPointer -= sizeof(T) * pValue;
				return *this;
			}

			bcInline operator T*() const
			{
				return get();
			}

			bcInline bcBOOL operator !() const
			{
				return mPointer == bcNULL;
			}
		};

		// == Ptr<void> =============================================================================== MRB ==
		template <> class Ptr<void>
		{
			typedef Ptr<void> ThisType;
		private:
			
		protected:

		public:
			void* mPointer;

			bcInline bcBOOL _isOnStack() const
			{
				return bcCoreOS::isPointerOnStack(mPointer);
			}
			bcInline MemBlock* _getMemBlock() const
			{
				return MemBlock::retrieveMemBlock(mPointer);
			}
			bcInline void _construct(void* pPointer)
			{
				mPointer = reinterpret_cast<void*>(pPointer);

				MemBlock* lMemBlock = _getMemBlock();
				if(lMemBlock != bcNULL)
				{
					lMemBlock->refrenceCount(lMemBlock->refrenceCount() + 1);
					if(lMemBlock->dynamicPointer())
					{
						//bcMemMng::get()->registerPointerInDynamicAllocators(*this);
					}
				}
			}
			bcInline void _deconstruct()
			{
				MemBlock* lMemBlock = _getMemBlock();
				if(lMemBlock != bcNULL)
				{
					lMemBlock->refrenceCount(lMemBlock->refrenceCount() - 1);
					if(lMemBlock->refrenceCount() == 0)
					{
						if(lMemBlock->alignment() > 16)
						{
							//bcAlignedFree(*this);
						}
						else
						{
							//bcFree(*this);
						}
					}
					else if(lMemBlock->dynamicPointer())
					{
						//bcMemMng::get()->unRegisterPointerInDynamicAllocators(*this);
					}
				}
				mPointer = bcNULL;
			}
			template<typename T1>
			bcInline Ptr<void>& _assign(const Ptr<T1>& pOther)
			{
				if (this != &pOther) // Avoid self assignment
				{
					_deconstruct();
				}

				_construct(pOther.get());

				return *this;
			}
			bcInline bcBYTE _compare(const void* pPointer) const
			{
				return (mPointer == pPointer) ? 0 : ((mPointer > pPointer) ? 1 : ((mPointer < pPointer) ? -1 : 0));
			}

			Ptr()
			{
				mPointer = bcNULL;
			}

			Ptr(void* pPointer)
			{
				_construct(pPointer);
			}

			Ptr(const Ptr<void>& pOther)
			{
				_construct(pOther.get());
			}

			template< typename T1 >
			Ptr(const Ptr<T1>& pOther)
			{
				_construct(pOther.get());
			}

			~Ptr()
			{
				_deconstruct();
			}

			bcInline void* get() const
			{
				return mPointer;
			}

			Ptr<void>& operator =(std::nullptr_t hp)
			{
				_deconstruct();
				return *this;
			}

			Ptr<void>& operator =(const Ptr<void>& pOther)
			{
				return _assign(pOther);
			}

			template< typename T1 >
			Ptr<void>& operator =(const Ptr<T1>& pOther)
			{
				return _assign(pOther);
			}

			bcInline bcBOOL operator ==( const Ptr<void>& pOther ) const
			{
				return (_compare(pOther.get()) == 0);
			}

			bcInline bcBOOL operator ==( const void* pOther ) const
			{
				return (_compare(pOther) == 0);
			}

			bcInline bcBOOL operator !=( const Ptr<void>& pOther ) const
			{
				return !(_compare(pOther.get()) == 0);
			}

			bcInline bcBOOL operator !=( const void* pOther ) const
			{
				return !(_compare(pOther) == 0);
			}

			bcInline bcBOOL operator <( const Ptr<void>& pOther ) const
			{
				return (_compare(pOther.get()) == -1);
			}

			bcInline bcBOOL operator <( const void* pOther ) const
			{
				return (_compare(pOther) == -1);
			}

			bcInline bcBOOL operator <=( const Ptr<void>& pOther ) const
			{
				bcBYTE flag = _compare(pOther.get());
				return (flag == -1 || flag == 0);
			}

			bcInline bcBOOL operator <=( const void* pOther ) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == -1 || flag == 0);
			}

			bcInline bcBOOL operator >( const Ptr<void>& pOther ) const
			{
				return (_compare(pOther.get()) == 1);
			}

			bcInline bcBOOL operator >( const void* pOther ) const
			{
				return (_compare(pOther) == 1);
			}

			bcInline bcBOOL operator >=( const Ptr<void>& pOther ) const
			{
				bcBYTE flag = _compare(pOther.get());
				return (flag == 1 || flag == 0);
			}

			bcInline bcBOOL operator >=( const void* pOther ) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == 1 || flag == 0);
			}

			bcInline operator void*()
			{
				return get();
			}

			bcInline bcBOOL operator !() const
			{
				return mPointer == bcNULL;
			}
		};

#pragma endregion

#endif
	}
}

#endif