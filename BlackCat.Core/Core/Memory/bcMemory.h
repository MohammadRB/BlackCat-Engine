//  [9/4/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "MemoryManagment.h"
#include "Ptr.h"

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

//#define P(T) bcPtr<T>
//#define P(T) T*
//#define RP(T) T*

#define bcAlloc(pSize, pAllocType, pCategory)									bcMemMng::get()->alloc(pSize, pAllocType, pCategory, __FILE__, __LINE__)
#define bcFree(pPointer)														/*if(pPointer)*/ { bcMemMng::get()->free(pPointer); pPointer = bcNULL; }
#define bcReAlloc(pPointer, pNewSize, pAllocType, pCategory)					bcMemMng::get()->reAlloc(pPointer, pNewSize, pAllocType, pCategory, __FILE__, __LINE__)
#define bcAlignedAlloc(pSize, pAlignment, pAllocType, pCategory)				bcMemMng::get()->alignedAlloc(pSize, pAlignment, pAllocType, pCategory, __FILE__, __LINE__)
#define bcAlignedFree(pPointer)													/*if(pPointer)*/ { bcMemMng::get()->alignedFree(pPointer); pPointer = bcNULL; }
#define bcAlignedReAlloc(pPointer, pNewSize, pAlignment, pAllocType, pCategory)	bcMemMng::get()->alignedReAlloc(pPointer, pNewSize, pAlignment, pAllocType, pCategory, __FILE__, __LINE__)

//#define bcNew(pT, pAllocType, pCategory)										bcMemMng::get()->new_(pT, pAllocType, pCategory, __FILE__, __LINE__)
#define bcNew(pAllocType, pCategory)											new (pAllocType, pCategory, __FILE__, __LINE__)
//#define bcNewArray(pT, pArrayLenth, pAllocType, pCategory)						bcMemMng::get()->newArray(pT, pArrayLenth, pAllocType, pCategory, __FILE__, __LINE__)
#define bcNewArray(pArrayLenth, pAllocType, pCategory)							new [pArrayLenth] (pAllocType, pCategory, __FILE__, __LINE__)
#define bcDelete(pT)															/*if(pT)*/ { bcMemMng::get()->delete_(pT); pT = bcNULL; }
#define bcDeleteArray(pT)														/*if(pT)*/ { bcMemMng::get()->deleteArray(pT); pT = bcNULL; }
//#define bcAlignedNew(pT, pAlignment, pAllocType, pCategory)						bcMemMng::get()->alignedNew(pT, pAlignment, pAllocType, pCategory, __FILE__, __LINE__)
#define bcAlignedNew(pAlignment, pAllocType, pCategory)							new (pAlignment, pAllocType, pCategory, __FILE__, __LINE__)
//#define bcAlignedNewArray(pT, pAlignment, pArrayLenth, pAllocType, pCategory)	bcMemMng::get()->alignedNewArray(pT, pAlignment, pArrayLenth, pAllocType, pCategory, __FILE__, __LINE__)
#define bcAlignedNewArray(pAlignment, pArrayLenth, pAllocType, pCategory)		new [pArrayLenth] (pAlignment, pAllocType, pCategory, __FILE__, __LINE__)
#define bcAlignedDelete(pT)														/*if(pT)*/ { bcMemMng::get()->alignedDelete(pT); pT = bcNULL; }
#define bcAlignedDeleteArray(pT)												/*if(pT)*/ { bcMemMng::get()->alignedDeleteArray(pT); pT = bcNULL; }

		template< typename T > class bcPtr;
		template<> class bcPtr<void>;
		/*template< typename T > class bcMovePtr;
		template<> class bcMovePtr<void>;*/
		//template< typename T > class bcScopePtr;
		//template<> class bcScopePtr<void>;
		template < typename T, typename Deleter > class bcUniquePtr;
		template< typename T, typename Deleter > class bcSharedPtr;
		//template<> class bcSharedPtr<void>;

		template<typename T>
		class bcSmartPtrDeleter
		{
		private:

		protected:

		public:
			void delete_(T* pPointer)
			{
				bcDelete(pPointer);
			}
		};

#pragma region Ptr
		// == Ptr ===================================================================================== MRB ==
		template < typename T >
		class bcPtr : bcPtrBase<T>
		{
		private:

		protected:
			T* mPointer;
#ifdef BC_MEMORY_DEFRAG
			bcINT32 mOffset;
#endif

			bcInline T* _getOrginalPtr() const
			{
#ifdef BC_MEMORY_DEFRAG
				return mPointer + mOffset;
#else
				return mPointer;
#endif
			}

			bcInline void _construct(void* pPointer, bcINT32 pOffset = 0)
			{
				mPointer = reinterpret_cast<T*>(pPointer);
#ifdef BC_MEMORY_DEFRAG
				mOffset = pOffset;
#endif

				_baseConstruct(_getOrginalPtr());
			}
			bcInline void _deconstruct()
			{
				_baseDeconstruct(_getOrginalPtr());

				mPointer = bcNULL;
#ifdef BC_MEMORY_DEFRAG
				mOffset = 0;
#endif
			}
			template<typename T1>
			bcInline bcPtr<T>& _assign(const bcPtr<T1>& pOther)
			{
				if (this != &pOther) // Avoid self assignment
				{
					_deconstruct();
#ifdef BC_MEMORY_DEFRAG
					_construct(pOther, pOther.mOffset);
#else
					_construct(pOther);
#endif
				}

				return *this;
			}
			bcInline bcBYTE _compare(const void* pPointer) const
			{
				return (get() == pPointer) ? 0 : ((get() > pPointer) ? 1 : ((get() < pPointer) ? -1 : 0));
			}

		public:

			bcPtr() : bcPtrBase()
			{
				mPointer = bcNULL;
#ifdef BC_MEMORY_DEFRAG
				mOffset = 0;
#endif
			}

			bcPtr(T* pPointer) : bcPtrBase()
			{
				_construct(pPointer);
			}

			bcPtr(const bcPtr<T>& pOther)
			{
#ifdef BC_MEMORY_DEFRAG
				_construct(pOther.get(), pOther.mOffset);
#else
				_construct(pOther.get());
#endif
			}

			template< typename T1 >
			bcPtr(const bcPtr<T1>& pOther)
			{
#ifdef BC_MEMORY_DEFRAG
				_construct(pOther.get(), pOther.mOffset);
#else
				_construct(pOther.get());
#endif
			}

			bcPtr(const bcPtr<void>& pOther)
			{
				_construct(pOther.get());
			}

			~bcPtr()
			{
				_deconstruct();
			}

			bcInline T& operator* () const
			{
#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEBUG)
				//MemBlock* lMemBlock = _getMemBlock(_getOrginalPtr());
				//bcAssert((lMemBlock != bcNULL && lMemBlock->free() != bcTRUE));
#endif
				return *mPointer;
			}

			bcInline T* operator-> () const
			{
#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEBUG)
				//MemBlock* lMemBlock = _getMemBlock(_getOrginalPtr());
				//bcAssert((lMemBlock != bcNULL && lMemBlock->free() != bcTRUE));
#endif
				return mPointer;
			}

			bcInline T* get() const
			{
				return mPointer;
			}

			bcPtr<T>& operator =(std::nullptr_t hp)
			{
				_deconstruct();
				return *this;
			}

			bcPtr<T>& operator =(const bcPtr<T>& pOther)
			{
				return _assign(pOther);
			}

			template< typename T1 >
			bcPtr<T>& operator =(const bcPtr<T1>& pOther)
			{
				return _assign(pOther);
			}

			bcPtr<T>& operator =(const bcPtr<void>& pOther)
			{
				return _assign(pOther);
			}

			bcInline bcBOOL operator ==(const bcPtr<T>& pOther) const
			{
				return (_compare(pOther.get()) == 0);
			}

			bcInline bcBOOL operator ==(const T* pOther) const
			{
				return (_compare(pOther) == 0);
			}

			bcInline bcBOOL operator !=(const bcPtr<T>& pOther) const
			{
				return !(_compare(pOther.get()) == 0);
			}

			bcInline bcBOOL operator !=(const T* pOther) const
			{
				return !(_compare(pOther) == 0);
			}

			bcInline bcBOOL operator <(const bcPtr<T>& pOther) const
			{
				return (_compare(pOther.get()) == -1);
			}

			bcInline bcBOOL operator <(const T* pOther) const
			{
				return (_compare(pOther) == -1);
			}

			bcInline bcBOOL operator <=(const bcPtr<T>& pOther) const
			{
				bcBYTE flag = _compare(pOther.get());
				return (flag == -1 || flag == 0);
			}

			bcInline bcBOOL operator <=(const T* pOther) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == -1 || flag == 0);
			}

			bcInline bcBOOL operator >(const bcPtr<T>& pOther) const
			{
				return (_compare(pOther.get()) == 1);
			}

			bcInline bcBOOL operator >(const T* pOther) const
			{
				return (_compare(pOther) == 1);
			}

			bcInline bcBOOL operator >=(const bcPtr<T>& pOther) const
			{
				bcBYTE flag = _compare(pOther.get());
				return (flag == 1 || flag == 0);
			}

			bcInline bcBOOL operator >=(const T* pOther) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == 1 || flag == 0);
			}

			bcInline void operator ++()
			{
				mPointer++;
#ifdef BC_MEMORY_DEFRAG
				mOffset--;
#endif
			}

			bcInline void operator ++(bcINT32)
			{
				++mPointer;
#ifdef BC_MEMORY_DEFRAG
				--mOffset;
#endif
			}

			bcInline void operator --()
			{
				mPointer--;
#ifdef BC_MEMORY_DEFRAG
				mOffset++;
#endif
			}

			bcInline void operator --(bcINT32)
			{
				--mPointer;
#ifdef BC_MEMORY_DEFRAG
				++mOffset;
#endif
			}

			bcInline bcPtr<T> operator +(bcINT32 pValue) const
			{
				bcPtr<T> lTemp;
				lTemp.mPointer = mPointer + pValue;
#ifdef BC_MEMORY_DEFRAG
				lTemp.mOffset = mOffset - pValue;
#endif
				return lTemp;
			}

			bcInline bcPtr<T>& operator +=(bcINT32 pValue)
			{
				mPointer += pValue;
#ifdef BC_MEMORY_DEFRAG
				mOffset -= pValue;
#endif
				return *this;
			}

			bcInline bcPtr<T> operator -(bcINT32 pValue) const
			{
				bcPtr<T> lTemp;
				lTemp.mPointer = mPointer - pValue;
#ifdef BC_MEMORY_DEFRAG
				lTemp.mOffset = mOffset + pValue;
#endif
				return lTemp;
			}

			bcInline bcPtr<T>& operator -=(bcINT32 pValue)
			{
				mPointer -= pValue;
#ifdef BC_MEMORY_DEFRAG
				mOffset += pValue;
#endif
				return *this;
			}

			bcInline operator T*() const
			{
				return get();
			}

			bcInline bcBOOL operator !() const
			{
				return get() == bcNULL;
			}
		};

		// == Ptr<void> =============================================================================== MRB ==
		template <>
		class bcPtr<void> : bcPtrBase<void>
		{
			typedef bcPtr<void> ThisType;
		private:

		protected:

		public:
			void* mPointer;

			bcInline void _construct(void* pPointer)
			{
				mPointer = reinterpret_cast<void*>(pPointer);

				_baseConstruct(mPointer);
			}
			bcInline void _deconstruct()
			{
				_baseDeconstruct(get());

				mPointer = bcNULL;
			}
			template<typename T1>
			bcInline bcPtr<void>& _assign(const bcPtr<T1>& pOther)
			{
				if (this != &pOther) // Avoid self assignment
				{
					_deconstruct();
					_construct(pOther.get());
				}

				return *this;
			}
			bcInline bcBYTE _compare(const void* pPointer) const
			{
				return (get() == pPointer) ? 0 : ((get() > pPointer) ? 1 : ((get() < pPointer) ? -1 : 0));
			}

			bcPtr()
			{
				mPointer = bcNULL;
			}

			bcPtr(void* pPointer)
			{
				_construct(pPointer);
			}

			bcPtr(const bcPtr<void>& pOther)
			{
				_construct(pOther.get());
			}

			template< typename T1 >
			bcPtr(const bcPtr<T1>& pOther)
			{
				_construct(pOther.get());
			}

			~bcPtr()
			{
				_deconstruct();
			}

			bcInline void* get() const
			{
				return mPointer;
			}

			bcPtr<void>& operator =(std::nullptr_t hp)
			{
				_deconstruct();
				return *this;
			}

			bcPtr<void>& operator =(const bcPtr<void>& pOther)
			{
				return _assign(pOther);
			}

			template< typename T1 >
			bcPtr<void>& operator =(const bcPtr<T1>& pOther)
			{
				return _assign(pOther);
			}

			bcInline bcBOOL operator ==(const bcPtr<void>& pOther) const
			{
				return (_compare(pOther.get()) == 0);
			}

			bcInline bcBOOL operator ==(const void* pOther) const
			{
				return (_compare(pOther) == 0);
			}

			bcInline bcBOOL operator !=(const bcPtr<void>& pOther) const
			{
				return !(_compare(pOther.get()) == 0);
			}

			bcInline bcBOOL operator !=(const void* pOther) const
			{
				return !(_compare(pOther) == 0);
			}

			bcInline bcBOOL operator <(const bcPtr<void>& pOther) const
			{
				return (_compare(pOther.get()) == -1);
			}

			bcInline bcBOOL operator <(const void* pOther) const
			{
				return (_compare(pOther) == -1);
			}

			bcInline bcBOOL operator <=(const bcPtr<void>& pOther) const
			{
				bcBYTE flag = _compare(pOther.get());
				return (flag == -1 || flag == 0);
			}

			bcInline bcBOOL operator <=(const void* pOther) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == -1 || flag == 0);
			}

			bcInline bcBOOL operator >(const bcPtr<void>& pOther) const
			{
				return (_compare(pOther.get()) == 1);
			}

			bcInline bcBOOL operator >(const void* pOther) const
			{
				return (_compare(pOther) == 1);
			}

			bcInline bcBOOL operator >=(const bcPtr<void>& pOther) const
			{
				bcBYTE flag = _compare(pOther.get());
				return (flag == 1 || flag == 0);
			}

			bcInline bcBOOL operator >=(const void* pOther) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == 1 || flag == 0);
			}

			bcInline operator void*() const
			{
				return get();
			}

			bcInline bcBOOL operator !() const
			{
				return get() == bcNULL;
			}
		};

#pragma endregion

#pragma region bcScopePtr
		//		// == Ptr ===================================================================================== MRB ==
		//		template < typename T > class bcScopePtr : bcPtrBase<T>
		//		{
		//		private:
		//
		//		protected:
		//			T* mPointer;
		//
		//			bcInline void _construct(void* pPointer)
		//			{
		//				mPointer = reinterpret_cast<T*>(pPointer);
		//
		//				_baseConstruct(get());
		//			}
		//			bcInline void _deconstruct()
		//			{
		//				_baseDeconstruct(get());
		//
		//				bcDelete(mPointer);
		//			}
		//			template<typename T1>
		//			bcInline bcScopePtr<T>& _assign(const bcScopePtr<T1>& pOther)
		//			{
		//				if (this != &pOther) // Avoid self assignment
		//				{
		//					_deconstruct();
		//					_construct(pOther.get());
		//				}
		//
		//				return *this;
		//			}
		//			bcInline bcBYTE _compare(const void* pPointer) const
		//			{
		//				return (get() == pPointer) ? 0 : ((get() > pPointer) ? 1 : ((get() < pPointer) ? -1 : 0));
		//			}
		//
		//		public:
		//
		//			bcScopePtr() : bcPtrBase()
		//			{
		//				mPointer = bcNULL;
		//			}
		//
		//			bcScopePtr(T* pPointer) : bcPtrBase()
		//			{
		//				_construct(pPointer);
		//			}
		//
		//			bcScopePtr(const bcScopePtr<T>& pOther)
		//			{
		//				_construct(pOther.get());
		//			}
		//
		//			template< typename T1 >
		//			bcScopePtr(const bcScopePtr<T1>& pOther)
		//			{
		//				_construct(pOther.get());
		//			}
		//
		//			bcScopePtr(const bcScopePtr<void>& pOther)
		//			{
		//				_construct(pOther.get());
		//			}
		//
		//			~bcScopePtr()
		//			{
		//				_deconstruct();
		//			}
		//
		//			bcInline T& operator* () const
		//			{
		//#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEBUG)
		//				//MemBlock* lMemBlock = _getMemBlock();
		//				//bcAssert((lMemBlock != bcNULL && lMemBlock->free() != bcTRUE));
		//#endif
		//				return *mPointer;
		//			}
		//
		//			bcInline T* operator-> () const
		//			{
		//#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEBUG)
		//				//MemBlock* lMemBlock = _getMemBlock();
		//				//bcAssert((lMemBlock != bcNULL && lMemBlock->free() != bcTRUE));
		//#endif
		//				return mPointer;
		//			}
		//
		//			bcInline T* get() const
		//			{
		//				return mPointer;
		//			}
		//
		//			bcScopePtr<T>& operator =(std::nullptr_t hp)
		//			{
		//				_deconstruct();
		//				mPointer = bcNULL;
		//				return *this;
		//			}
		//
		//			bcScopePtr<T>& operator =(const bcScopePtr<T>& pOther)
		//			{
		//				return _assign(pOther);
		//			}
		//
		//			template< typename T1 >
		//			bcScopePtr<T>& operator =(const bcScopePtr<T1>& pOther)
		//			{
		//				return _assign(pOther);
		//			}
		//
		//			bcScopePtr<T>& operator =(const bcScopePtr<void>& pOther)
		//			{
		//				return _assign(pOther);
		//			}
		//
		//			bcInline bcBOOL operator ==( const bcScopePtr<T>& pOther ) const
		//			{
		//				return (_compare(pOther.get()) == 0);
		//			}
		//
		//			bcInline bcBOOL operator ==( const T* pOther ) const
		//			{
		//				return (_compare(pOther) == 0);
		//			}
		//
		//			bcInline bcBOOL operator !=( const bcScopePtr<T>& pOther ) const
		//			{
		//				return !(_compare(pOther.get()) == 0);
		//			}
		//
		//			bcInline bcBOOL operator !=( const T* pOther ) const
		//			{
		//				return !(_compare(pOther) == 0);
		//			}
		//
		//			bcInline bcBOOL operator <( const bcScopePtr<T>& pOther ) const
		//			{
		//				return (_compare(pOther.get()) == -1);
		//			}
		//
		//			bcInline bcBOOL operator <( const T* pOther ) const
		//			{
		//				return (_compare(pOther) == -1);
		//			}
		//
		//			bcInline bcBOOL operator <=( const bcScopePtr<T>& pOther ) const
		//			{
		//				bcBYTE flag = _compare(pOther.get());
		//				return (flag == -1 || flag == 0);
		//			}
		//
		//			bcInline bcBOOL operator <=( const T* pOther ) const
		//			{
		//				bcBYTE flag = _compare(pOther);
		//				return (flag == -1 || flag == 0);
		//			}
		//
		//			bcInline bcBOOL operator >( const bcScopePtr<T>& pOther ) const
		//			{
		//				return (_compare(pOther.get()) == 1);
		//			}
		//
		//			bcInline bcBOOL operator >( const T* pOther ) const
		//			{
		//				return (_compare(pOther) == 1);
		//			}
		//
		//			bcInline bcBOOL operator >=( const bcScopePtr<T>& pOther ) const
		//			{
		//				bcBYTE flag = _compare(pOther.get());
		//				return (flag == 1 || flag == 0);
		//			}
		//
		//			bcInline bcBOOL operator >=( const T* pOther ) const
		//			{
		//				bcBYTE flag = _compare(pOther);
		//				return (flag == 1 || flag == 0);
		//			}
		//
		//			bcInline operator T*() const
		//			{
		//				return get();
		//			}
		//
		//			bcInline bcBOOL operator !() const 
		//			{
		//				return get() == bcNULL;
		//			}
		//		};

#pragma endregion

#pragma region bcUniquePtr
		// == bcUniquePtr ================================================================================ MRB ==
		template < typename T, typename Deleter = bcSmartPtrDeleter<T> >
		class bcUniquePtr : bcPtrBase<T>
		{
		private:

		protected:
			T* mPointer;
			Deleter mDeleter;

			bcInline void _construct(void* pPointer)
			{
				mPointer = reinterpret_cast<T*>(pPointer);

				_baseConstruct(get());
			}
			bcInline void _deconstruct()
			{
				_baseDeconstruct(get());

				if (mPointer != bcNULL)
				{
					mDeleter.delete_(mPointer);
					mPointer = bcNULL;
				}
			}
			template<typename T1>
			bcInline bcUniquePtr<T>& _assign(bcUniquePtr<T1>&& pOther)
			{
				if (this != &pOther) // Avoid self assignment
				{
					_deconstruct();
					_construct(pOther.get());
					pOther.mPointer = bcNULL;
				}

				return *this;
			}
			bcInline bcBYTE _compare(const void* pPointer) const
			{
				return (get() == pPointer) ? 0 : ((get() > pPointer) ? 1 : ((get() < pPointer) ? -1 : 0));
			}

		public:

			bcUniquePtr() : mPointer(bcNULL), bcPtrBase()
			{}

			bcUniquePtr(T* pPointer) : bcPtrBase()
			{
				_construct(pPointer);
			}

			bcUniquePtr(const bcUniquePtr<T>&) = delete;

			bcUniquePtr(bcUniquePtr<T>&& pOther) : mPointer(bcNULL)
			{
				_assign(std::forward<bcUniquePtr<T>>(pOther));
			}

			template< typename T1 >
			bcUniquePtr(bcUniquePtr<T1>&& pOther) : mPointer(bcNULL)
			{
				_assign(std::forward<bcUniquePtr<T1>>(pOther));
			}

			/*bcUniquePtr(const bcUniquePtr<void>&& pOther)
			{
			_assign(pOther);
			}*/

			~bcUniquePtr()
			{
				_deconstruct();
			}

			bcInline T& operator* () const
			{
#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEBUG)
				//MemBlock* lMemBlock = _getMemBlock();
				//bcAssert((lMemBlock != bcNULL && lMemBlock->free() != bcTRUE));
#endif
				return *mPointer;
			}

			bcInline T* operator-> () const
			{
#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEBUG)
				//MemBlock* lMemBlock = _getMemBlock();
				//bcAssert((lMemBlock != bcNULL && lMemBlock->free() != bcTRUE));
#endif
				return mPointer;
			}

			bcInline T* get() const
			{
				return mPointer;
			}

			bcInline T* release()
			{
				T* lPointer = get();
				mPointer = bcNULL;
				return lPointer;
			}

			bcUniquePtr<T>& operator =(std::nullptr_t hp)
			{
				_deconstruct();
				mPointer = bcNULL;
				return *this;
			}

			bcUniquePtr<T>& operator =(const bcUniquePtr<T>&) = delete;

			bcUniquePtr<T>& operator =(bcUniquePtr<T>&& pOther)
			{
				return _assign(std::forward<bcUniquePtr<T>>(pOther));
			}

			template< typename T1 >
			bcUniquePtr<T>& operator =(bcUniquePtr<T1>&& pOther)
			{
				return _assign(std::forward<bcUniquePtr<T1>>(pOther));
			}

			/*bcScopePtr<T>& operator =(const bcScopePtr<void>&& pOther)
			{
			return _assign(pOther);
			}*/

			/*bcInline bcBOOL operator ==(const bcScopePtr<T>& pOther) const
			{
			return (_compare(pOther.get()) == 0);
			}*/

			bcInline bcBOOL operator ==(const T* pOther) const
			{
				return (_compare(pOther) == 0);
			}

			/*bcInline bcBOOL operator !=(const bcScopePtr<T>& pOther) const
			{
			return !(_compare(pOther.get()) == 0);
			}*/

			bcInline bcBOOL operator !=(const T* pOther) const
			{
				return !(_compare(pOther) == 0);
			}

			/*bcInline bcBOOL operator <(const bcScopePtr<T>& pOther) const
			{
			return (_compare(pOther.get()) == -1);
			}*/

			bcInline bcBOOL operator <(const T* pOther) const
			{
				return (_compare(pOther) == -1);
			}

			/*bcInline bcBOOL operator <=(const bcScopePtr<T>& pOther) const
			{
			bcBYTE flag = _compare(pOther.get());
			return (flag == -1 || flag == 0);
			}*/

			bcInline bcBOOL operator <=(const T* pOther) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == -1 || flag == 0);
			}

			/*bcInline bcBOOL operator >(const bcScopePtr<T>& pOther) const
			{
			return (_compare(pOther.get()) == 1);
			}*/

			bcInline bcBOOL operator >(const T* pOther) const
			{
				return (_compare(pOther) == 1);
			}

			/*bcInline bcBOOL operator >=(const bcScopePtr<T>& pOther) const
			{
			bcBYTE flag = _compare(pOther.get());
			return (flag == 1 || flag == 0);
			}*/

			bcInline bcBOOL operator >=(const T* pOther) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == 1 || flag == 0);
			}

			bcInline operator T*() const
			{
				return get();
			}

			bcInline bcBOOL operator !() const
			{
				return get() == bcNULL;
			}
		};

#pragma endregion

#pragma region bcSharedPtr
		// == Ptr ===================================================================================== MRB ==
		template < typename T, typename Deleter = bcSmartPtrDeleter<T> >
		class bcSharedPtr : bcPtrBase<T>
		{
		private:

		protected:
			T* mPointer;
#ifndef BC_MEMORY_ENABLE
			bcUINT32* mRefCount;
#endif
			Deleter mDeleter;

			bcInline void _incReferenceCount()
			{
#ifndef BC_MEMORY_ENABLE
				++mRefCount;
#else
				_baseIncReferenceCount(mPointer);
#endif
			}
			bcInline void _decReferenceCount()
			{
#ifndef BC_MEMORY_ENABLE
				--mRefCount;
				if (mRefCount <= 0)
					bcDelete(mPointer);
#else
				if (!_baseDecReferenceCount(mPointer))
				{
					mDeleter.delete_(mPointer);
					mPointer = bcNULL;
				}
#endif
			}

			bcInline void _construct(void* pPointer)
			{
				mPointer = reinterpret_cast<T*>(pPointer);
#ifndef BC_MEMORY_ENABLE
				mRefCount = reinterpret_cast<bcUINT32*>(
					bcAlloc(
					sizeof(bcUINT32),
					bcAlloc_Type::Alloc_Unknown,
					bcHeap_Category::Heap_MemoryManagment));
#endif 


				_baseConstruct(mPointer);
				_incReferenceCount();
			}
			bcInline void _deconstruct()
			{
				_baseDeconstruct(get());
				_decReferenceCount();

#ifndef BC_MEMORY_ENABLE
				bcFree(mRefCount);
#endif
			}
			template<typename T1>
			bcInline bcSharedPtr<T>& _assign(bcSharedPtr<T1>& pOther)
			{
				if (this != &pOther) // Avoid self assignment
				{
					_deconstruct();
					_construct(pOther.get());
				}

				return *this;
			}
			bcInline bcBYTE _compare(const void* pPointer) const
			{
				return (get() == pPointer) ? 0 : ((get() > pPointer) ? 1 : ((get() < pPointer) ? -1 : 0));
			}

		public:

			bcSharedPtr() : bcPtrBase()
			{
				mPointer = bcNULL;
			}

			bcSharedPtr(T* pPointer) : bcPtrBase()
			{
				_construct(pPointer);
			}

			bcSharedPtr(const bcSharedPtr<T>& pOther)
			{
				_construct(pOther.get());
			}

			template< typename T1 >
			bcSharedPtr(const bcSharedPtr<T1>& pOther)
			{
				_construct(pOther.get());
			}

			/*bcSharedPtr(const bcSharedPtr<void>& pOther)
			{
			_construct(pOther.get());
			}*/

			~bcSharedPtr()
			{
				_deconstruct();
			}

			bcInline T& operator* () const
			{
#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEBUG)
				//MemBlock* lMemBlock = _getMemBlock();
				//bcAssert((lMemBlock != bcNULL && lMemBlock->free() != bcTRUE));
#endif
				return *mPointer;
			}

			bcInline T* operator-> () const
			{
#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEBUG)
				//MemBlock* lMemBlock = _getMemBlock();
				//bcAssert((lMemBlock != bcNULL && lMemBlock->free() != bcTRUE));
#endif
				return mPointer;
			}

			bcInline T* get() const
			{
				return mPointer + mOffset;
			}

			bcSharedPtr<T>& operator =(std::nullptr_t hp)
			{
				_deconstruct();
				mPointer = bcNULL;
				return *this;
			}

			bcSharedPtr<T>& operator =(const bcSharedPtr<T>& pOther)
			{
				return _assign(pOther);
			}

			template< typename T1 >
			bcSharedPtr<T>& operator =(const bcSharedPtr<T1>& pOther)
			{
				return _assign(pOther);
			}

			/*bcSharedPtr<T>& operator =(const bcSharedPtr<void>& pOther)
			{
			return _assign(pOther);
			}*/

			bcInline bcBOOL operator ==(const bcSharedPtr<T>& pOther) const
			{
				return (_compare(pOther.get()) == 0);
			}

			bcInline bcBOOL operator ==(const T* pOther) const
			{
				return (_compare(pOther) == 0);
			}

			bcInline bcBOOL operator !=(const bcSharedPtr<T>& pOther) const
			{
				return !(_compare(pOther.get()) == 0);
			}

			bcInline bcBOOL operator !=(const T* pOther) const
			{
				return !(_compare(pOther) == 0);
			}

			bcInline bcBOOL operator <(const bcSharedPtr<T>& pOther) const
			{
				return (_compare(pOther.get()) == -1);
			}

			bcInline bcBOOL operator <(const T* pOther) const
			{
				return (_compare(pOther) == -1);
			}

			bcInline bcBOOL operator <=(const bcSharedPtr<T>& pOther) const
			{
				bcBYTE flag = _compare(pOther.get());
				return (flag == -1 || flag == 0);
			}

			bcInline bcBOOL operator <=(const T* pOther) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == -1 || flag == 0);
			}

			bcInline bcBOOL operator >(const bcSharedPtr<T>& pOther) const
			{
				return (_compare(pOther.get()) == 1);
			}

			bcInline bcBOOL operator >(const T* pOther) const
			{
				return (_compare(pOther) == 1);
			}

			bcInline bcBOOL operator >=(const bcSharedPtr<T>& pOther) const
			{
				bcBYTE flag = _compare(pOther.get());
				return (flag == 1 || flag == 0);
			}

			bcInline bcBOOL operator >=(const T* pOther) const
			{
				bcBYTE flag = _compare(pOther);
				return (flag == 1 || flag == 0);
			}

			bcInline operator T*() const
			{
				return get();
			}

			bcInline bcBOOL operator !() const
			{
				return get() == bcNULL;
			}
		};

#pragma endregion

#else

//#define P(T) T*
//#define RP(T) T*

#define bcAlloc(pSize, pAllocType, pCategory)						malloc(pSize)
#define bcFree(pMemory)												if(pMemory) { free(pMemory); pMemory = NULL; }
#define bcReAlloc(pMemory, pNewSize, pAllocType)					realloc(pMemory, pNewSize)
#define bcAlignedAlloc(pSize, pAlignment, pAllocType, pCategory)	_aligned_malloc(pSize, pAlignment)
#define bcAlignedFree(pMemory)										_aligned_free(pMemory)
#define bcAlignedReAlloc(pMemory, pNewSize, pAlignment, pAllocType)	_aligned_realloc(pMemory, pNewSize, pAlignment)

#define bcNew(pT, pAllocType, pCategory)							new pT
#define bcNewArray(pT, pArrayLenth, pAllocType, pCategory)			new pT[pArrayLenth]
#define bcDelete(pT)												if(pMemory) { delete(pT); pMemory = NULL; }
#define bcDeleteArray(pT)											if(pMemory) { delete[](pT); pMemory = NULL; }

#endif

	}
}