#pragma once

#ifndef bcString_H
#define bcString_H

#include "CorePCH.h"
#include "bcContainer.h"
//#include "CoreUtility.h"

namespace BlackCat
{
	namespace Core
	{
		/*template< typename PT = P(T) >*/ struct bcCharTrait
		{	// properties of a string or stream char element
			typedef bcCHAR Element;
			typedef Element CharType;
			/*typedef int int_type;
			typedef streampos pos_type;
			typedef streamoff off_type;
			typedef _Mbstatet state_type;*/
			/*typedef PT Pointer;
			typedef const Pointer Const_Pointer;*/
			//typedef Element* RawPointer;
			//typedef const Element* Const_RawPointer;

			static bcCHAR End()
			{
				return '\0';
			}

			static bcINT32 Compare(const Element* pFirst, const Element* pSecond, bcSIZE pCount)
			{	// compare [_First1, _First1 + _Count) with [_First2, ...)
				return (std::memcmp(pFirst, pSecond, pCount));
			}

			static bcSIZE Length(const Element* pStr)
			{	// find length of null-terminated string
				return (std::strlen(pStr));
			}

			static Element* Copy(Element* pDst, const Element* pSrc, bcSIZE pCount)
			{	// copy [_First1, _First1 + _Count) to [_First2, ...)
				return ((Element*)std::memcpy(pDst, pSrc, pCount));
			}

			//static _Elem *__CLRCALL_OR_CDECL _Copy_s(
			//_Out_cap_(_Size_in_bytes) _Elem *_First1, size_t _Size_in_bytes,
			//_In_count_(_Count) const _Elem *_First2, size_t _Count)
			//{	// copy [_First1, _First1 + _Count) to [_First2, ...)
			//_CRT_SECURE_MEMCPY(_First1, _Size_in_bytes, _First2, _Count);
			//return _First1;
			//}

			static const Element* Find(const Element* pStr, bcSIZE pCount, const Element& pCh)
			{	// look for _Ch in [_First, _First + _Count)
				return ((const Element*)std::memchr(pStr, pCh, pCount));
			}

			static Element* Move(Element* pDst, const Element* pSrc, bcSIZE pCount)
			{	// copy [_First1, _First1 + _Count) to [_First2, ...)
				return ((Element*)std::memmove(pDst, pSrc, pCount));
			}

			static Element* Assign(Element* pDst, bcSIZE pCount, Element pCh)
			{	// assign _Count * _Ch to [_First, ...)
				return ((Element*)std::memset(pDst, pCh, pCount));
			}

			static void Assign(Element& pLeft, const Element& pRight)
			{	// assign an element
				pLeft = pRight;
			}

			static bool Equal(const Element& pLeft, const Element& pRight)
			{	// test for element equality
				return (pLeft == pRight);
			}

			static bool Little(const Element& pLeft, const Element& pRight)
			{	// test if _Left precedes _Right
				return ((unsigned char)pLeft < (unsigned char)pRight);
			}

			//static _Elem __CLRCALL_OR_CDECL to_char_type(const int_type& _Meta)
			//	{	// convert metacharacter to character
			//	return ((_Elem)_Meta);
			//	}

			//static int_type __CLRCALL_OR_CDECL to_int_type(const _Elem& _Ch)
			//	{	// convert character to metacharacter
			//	return ((unsigned char)_Ch);
			//	}

			//static bool __CLRCALL_OR_CDECL eq_int_type(const int_type& _Left,
			//	const int_type& _Right)
			//	{	// test for metacharacter equality
			//	return (_Left == _Right);
			//	}

			//static int_type __CLRCALL_OR_CDECL not_eof(const int_type& _Meta)
			//	{	// return anything but EOF
			//	return (_Meta != eof() ? _Meta : !eof());
			//	}

			//static int_type __CLRCALL_OR_CDECL eof()
			//	{	// return end-of-file metacharacter
			//	return (EOF);
			//	}
		};

		/*template< typename PT = P(T) >*/ struct bcWCharTrait
		{	// properties of a string or stream char element
			typedef bcWCHAR Element;
			typedef Element CharType;
			/*typedef int int_type;
			typedef streampos pos_type;
			typedef streamoff off_type;
			typedef _Mbstatet state_type;*/
			/*typedef PT Pointer;
			typedef const Pointer Const_Pointer;*/
			//typedef Element* RawPointer;
			//typedef const Element* Const_RawPointer;

			static bcWCHAR End()
			{
				return '\0';
			}

			static bcINT32 Compare(const Element* pFirst, const Element* pSecond, bcSIZE pCount)
			{	// compare [_First1, _First1 + _Count) with [_First2, ...)
				return (std::wmemcmp(pFirst, pSecond, pCount));
			}

			static bcSIZE Length(const Element* pStr)
			{	// find length of null-terminated string
				return (std::wcslen(pStr));
			}

			static Element* Copy(Element* pDst, const Element* pSrc, bcSIZE pCount)
			{	// copy [_First1, _First1 + _Count) to [_First2, ...)
				return ((Element*)std::wmemcpy(pDst, pSrc, pCount));
			}

			//static _Elem *__CLRCALL_OR_CDECL _Copy_s(
			//_Out_cap_(_Size_in_bytes) _Elem *_First1, size_t _Size_in_bytes,
			//_In_count_(_Count) const _Elem *_First2, size_t _Count)
			//{	// copy [_First1, _First1 + _Count) to [_First2, ...)
			//_CRT_SECURE_MEMCPY(_First1, _Size_in_bytes, _First2, _Count);
			//return _First1;
			//}

			static const Element* Find(const Element* pStr, bcSIZE pCount, const Element& pCh)
			{	// look for _Ch in [_First, _First + _Count)
				return ((const Element*)std::wmemchr(pStr, pCh, pCount));
			}

			static Element* Move(Element* pDst, const Element* pSrc, bcSIZE pCount)
			{	// copy [_First1, _First1 + _Count) to [_First2, ...)
				return ((Element*)std::wmemmove(pDst, pSrc, pCount));
			}

			static Element* Assign(Element* pDst, bcSIZE pCount, Element pCh)
			{	// assign _Count * _Ch to [_First, ...)
				return ((Element*)std::wmemset(pDst, pCh, pCount));
			}

			static void Assign(Element& pLeft, const Element& pRight)
			{	// assign an element
				pLeft = pRight;
			}

			static bool Equal(const Element& pLeft, const Element& pRight)
			{	// test for element equality
				return (pLeft == pRight);
			}

			static bool Little(const Element& pLeft, const Element& pRight)
			{	// test if _Left precedes _Right
				return ((unsigned char)pLeft < (unsigned char)pRight);
			}

			//static _Elem __CLRCALL_OR_CDECL to_char_type(const int_type& _Meta)
			//	{	// convert metacharacter to character
			//	return ((_Elem)_Meta);
			//	}

			//static int_type __CLRCALL_OR_CDECL to_int_type(const _Elem& _Ch)
			//	{	// convert character to metacharacter
			//	return ((unsigned char)_Ch);
			//	}

			//static bool __CLRCALL_OR_CDECL eq_int_type(const int_type& _Left,
			//	const int_type& _Right)
			//	{	// test for metacharacter equality
			//	return (_Left == _Right);
			//	}

			//static int_type __CLRCALL_OR_CDECL not_eof(const int_type& _Meta)
			//	{	// return anything but EOF
			//	return (_Meta != eof() ? _Meta : !eof());
			//	}

			//static int_type __CLRCALL_OR_CDECL eof()
			//	{	// return end-of-file metacharacter
			//	return (EOF);
			//	}
		};

		template < typename T, class Trait, class Allocator >
		class bcString_Base : public bcContainer< T, Allocator >
		{
		public:
		private:

		protected:
			PointerType mFirst;

		public:
			bcString_Base() 
			{
				mFirst = NULL;
			};
			~bcString_Base() {};
		};

		template < typename T, class Trait, class Allocator = bcContainerAllocation<> >
		class bcBaseString : public bcString_Base< T, Trait, Allocator >
		{
		public:
			typedef bcBaseString< T, Trait, Allocator > ThisType;
		private:
			
		protected:
			bcInline void _Assign(const ValueType* pString)
			{
				Clear();
				mSize = Trait::Length(pString);
				mFirst = (PointerType)mAllocator.alloc(sizeof(ValueType) * (mSize + 1));
				Trait::Copy(mFirst, pString, mSize);

				*(mFirst + mSize) = Trait::End();
			};
			bcInline void _Append(const ValueType* pString)
			{
				bcUINT32 lStrSize = Trait::Length(pString);
				mFirst = (PointerType)mAllocator.reAlloc(mFirst, sizeof(ValueType) * (mSize + lStrSize + 1));
				Trait::Copy(mFirst + mSize, pString, lStrSize);
				mSize += lStrSize;

				*(mFirst + mSize) = Trait::End();
			};
			bcInline void _Insert(bcUINT32 pPosition, const ValueType* pString)
			{
				bcUINT32 lStrSize = Trait::Length(pString);
				bcUINT32 lNewSize = mSize + lStrSize;
				mFirst = (PointerType)mAllocator.reAlloc(mFirst, sizeof(ValueType) * (lNewSize + 1));
				Trait::Move(mFirst + pPosition + lStrSize, mFirst + pPosition, mSize - pPosition);
				Trait::Copy(mFirst + pPosition, pString, lStrSize);
				mSize = lNewSize;

				*(mFirst + mSize) = Trait::End();
			};
			bcInline void _Erase(bcUINT32 pPosition, bcUINT32 n)
			{
				bcUINT32 lStrSize = n;
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mSize - lStrSize >= 0);
#endif
				bcUINT32 lNewSize = mSize - lStrSize;
				Trait::Move(mFirst + pPosition, mFirst + pPosition + lStrSize, mSize - (pPosition + lStrSize));
				mSize = lNewSize;
				mFirst = (PointerType)mAllocator.reAlloc(mFirst, sizeof(ValueType) * (mSize + 1));

				*(mFirst + mSize) = Trait::End();
			};
			bcInline bcINT32 _Compare(const ValueType* pString) const
			{
				bcUINT32 lStrSize = Trait::Length(pString);

				if(mSize > lStrSize)
					return 1;
				else if(mSize < lStrSize)
					return -1;
				else
				{
					bcINT32 lResult = Trait::Compare(mFirst, pString, mSize);
					return (lResult == 0)? 0: ((lResult > 1)? 1: -1);
				}
			};

		public:
			bcBaseString();
			bcBaseString(ValueType pChar);
			bcBaseString(const ValueType* pString);
			bcBaseString(const ThisType& pOther);
			bcBaseString(ThisType&& pOther);
			~bcBaseString();

			ThisType& Assign(ValueType pChar);
			ThisType& Assign(const ValueType* pString);
			ThisType& Assign(const ThisType& pOther);

			ThisType& operator=(ValueType pChar);
			ThisType& operator=(const ValueType* pString);
			ThisType& operator=(const ThisType& pOther);

			ThisType& Append(ValueType pChar);
			ThisType& Append(const ValueType* pString);
			ThisType& Append(const ThisType& pOther);

			ThisType& operator+=(ValueType pChar);
			ThisType& operator+=(const ValueType* pString);
			ThisType& operator+=(const ThisType& pOther);

			ThisType& operator+(ValueType pChar);
			ThisType& operator+(const ValueType* pString);
			ThisType& operator+(const ThisType& pOther);

			void Swap(ThisType& pOther);

			/*Iterator       Begin();
			Const_Iterator Begin() const;
        
			Iterator       End();
			Const_Iterator End() const;*/

			PointerType       Data();
			const PointerType Data() const;
			operator ValueType*() const;
			const ValueType* c_str() const;

			ValueType&       operator[](bcUINT32 n);
			const ValueType& operator[](bcUINT32 n) const;

			ValueType&		At(bcUINT32 n);
			const ValueType& At(bcUINT32 n) const;

			void PushBack(ValueType pChar);
			//void PushBack(Const_Pointer pString);
			void PushBack(const ValueType* pString);
			void PushBack(const ThisType& pOther);
			//Iterator PushBack(const ReferenceType pValue);
			//void PushBack();
			//Iterator PushBack();

			void PopBack();

			void Insert(bcUINT32 pPosition, ValueType pChar);
			//void Insert(bcUINT32 pPosition, Const_Pointer pString);
			void Insert(bcUINT32 pPosition, const ValueType* pString);
			void Insert(bcUINT32 pPosition, const ThisType& pOther);
			//Iterator Insert(Iterator pPosition, const Reference pValue);

			//Iterator Erase(Iterator pPosition);
			//Iterator Erase(Iterator pFirst, Iterator pLast);
			void Erase(bcUINT32 pPosition);
			void Erase(bcUINT32 pPosition, bcINT32 n);
			void Erase(bcUINT32 pFirst, bcUINT32 pLast);

			void Clear();

			bcUINT32 Hash() const;

			//ThisType&  Replace(bcUINT32 pPosition, bcUINT32 n, const ThisType& pOther);
			//ThisType&  Replace(bcUINT32 pos1, bcUINT32 n1, const ThisType& pOther, bcUINT32 pos2, bcUINT32 n2);
			//ThisType&  Replace(bcUINT32 pPosition, bcUINT32 n1, Const_RawPointer pOther, bcUINT32 n2);
			//ThisType&  Replace(bcUINT32 pPosition, bcUINT32 n1, Const_RawPointer pOther);
			//ThisType&  Replace(bcUINT32 pPosition, bcUINT32 n1, bcUINT32 n2, Value c);

			//size_type      copy(value_type* p, size_type n, size_type position = 0) const;

	        //// Find operations
	        //size_type find(const basic_string& x, size_type position = 0) const; 
	        //size_type find(const value_type* p, size_type position = 0) const;
	        //size_type find(const value_type* p, size_type position, size_type n) const;
	        //size_type find(value_type c, size_type position = 0) const;

	        //// Reverse find operations
	        //size_type rfind(const basic_string& x, size_type position = npos) const; 
	        //size_type rfind(const value_type* p, size_type position = npos) const;
	        //size_type rfind(const value_type* p, size_type position, size_type n) const;
	        //size_type rfind(value_type c, size_type position = npos) const;

	        //// Find first-of operations
	        //size_type find_first_of(const basic_string& x, size_type position = 0) const;
	        //size_type find_first_of(const value_type* p, size_type position = 0) const;
	        //size_type find_first_of(const value_type* p, size_type position, size_type n) const;
	        //size_type find_first_of(value_type c, size_type position = 0) const;

	        //// Find last-of operations
	        //size_type find_last_of(const basic_string& x, size_type position = npos) const;
	        //size_type find_last_of(const value_type* p, size_type position = npos) const;
	        //size_type find_last_of(const value_type* p, size_type position, size_type n) const;
	        //size_type find_last_of(value_type c, size_type position = npos) const;

	        //// Find first not-of operations
	        //size_type find_first_not_of(const basic_string& x, size_type position = 0) const;
	        //size_type find_first_not_of(const value_type* p, size_type position = 0) const;
	        //size_type find_first_not_of(const value_type* p, size_type position, size_type n) const;
	        //size_type find_first_not_of(value_type c, size_type position = 0) const;

	        //// Find last not-of operations
	        //size_type find_last_not_of(const basic_string& x,  size_type position = npos) const;
	        //size_type find_last_not_of(const value_type* p, size_type position = npos) const;
	        //size_type find_last_not_of(const value_type* p, size_type position, size_type n) const;
	        //size_type find_last_not_of(value_type c, size_type position = npos) const;

	        //// Substring functionality
	        //basic_string substr(size_type position = 0, size_type n = npos) const;

	        //// Comparison operations
	        //int        compare(const basic_string& x) const;
	        //int        compare(size_type pos1, size_type n1, const basic_string& x) const;
	        //int        compare(size_type pos1, size_type n1, const basic_string& x, size_type pos2, size_type n2) const;
	        //int        compare(const value_type* p) const;
	        //int        compare(size_type pos1, size_type n1, const value_type* p) const;
	        //int        compare(size_type pos1, size_type n1, const value_type* p, size_type n2) const;
	        //static int compare(const value_type* pBegin1, const value_type* pEnd1, const value_type* pBegin2, const value_type* pEnd2);

			bcINT32 Compare(const ValueType* pString) const;
			bcINT32 Compare(const ThisType& pOther) const;
			bcBOOL operator==(const ValueType* pString) const;
			bcBOOL operator==(const ThisType& pOther) const;
			bcBOOL operator!=(const ValueType* pString) const;
			bcBOOL operator!=(const ThisType& pOther) const;
			bcBOOL operator<(const ValueType* pString) const;
			bcBOOL operator<(const ThisType& pOther) const;
			bcBOOL operator>(const ValueType* pString) const;
			bcBOOL operator>(const ThisType& pOther) const;

	        //// Case-insensitive comparison functions. Not part of C++ basic_string. Only ASCII-level locale functionality is supported. Thus this is not suitable for localization purposes.
	        //int        comparei(const basic_string& x) const;
	        //int        comparei(const value_type* p) const;
	        //static int comparei(const value_type* pBegin1, const value_type* pEnd1, const value_type* pBegin2, const value_type* pEnd2);

	        //// Misc functionality, not part of C++ basic_string.
	        //void            make_lower();
	        //void            make_upper();
	        //void            ltrim();
	        //void            rtrim();
	        //void            trim();
	        //basic_string    left(size_type n) const;
	        //basic_string    right(size_type n) const;
		};

		template < typename T, class Trait, class Allocator >
		bcBaseString< T, Trait, Allocator >::bcBaseString()
		{
		};
		template < typename T, class Trait, class Allocator >
		bcBaseString< T, Trait, Allocator >::bcBaseString(ValueType pChar)
		{
			Clear();
			mSize = 1;
			mFirst = (PointerType)mAllocator.alloc(sizeof(ValueType) * (mSize + 1));
			Trait::Copy(mFirst, &pChar, mSize);

			*(mFirst + mSize) = Trait::End();
		};
		template < typename T, class Trait, class Allocator >
		bcBaseString< T, Trait, Allocator >::bcBaseString(const ValueType* pString)
		{
			_Assign(pString);
		};
		template < typename T, class Trait, class Allocator >
		bcBaseString< T, Trait, Allocator >::bcBaseString(const ThisType& pOther)
		{
			_Assign((ValueType*)pOther.mFirst);
		};
		template < typename T, class Trait, class Allocator >
		bcBaseString< T, Trait, Allocator >::bcBaseString(ThisType&& pOther)
		{
			Clear();

			mFirst = pOther.mFirst;
			mSize = pOther.mSize;
			mAllocator = pOther.mAllocator;

			pOther.mFirst = nullptr;
			pOther.Clear();
		};
		template < typename T, class Trait, class Allocator >
		bcBaseString< T, Trait, Allocator >::~bcBaseString()
		{
			Clear();
		};

		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::Assign(ValueType pChar)
		{
			Clear();
			mSize = 1;
			mFirst = (PointerType)mAllocator.alloc(sizeof(ValueType) * (mSize + 1));
			Trait::Copy(mFirst, &pChar, mSize);

			*(mFirst + mSize) = Trait::End();

			return *this;
		};
		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::Assign(const ValueType* pString)
		{
			_Assign(pString);

			return *this;
		};
		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::Assign(const ThisType& pOther)
		{
			_Assign((ValueType*)pOther.mFirst);

			return *this;
		};

		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::operator=(ValueType pChar)
		{
			Assign(pChar);

			return *this;
		};
		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::operator=(const ValueType* pString)
		{
			Assign(pString);

			return *this;
		};
		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::operator=(const ThisType& pOther)
		{
			Assign(pOther);

			return *this;
		};

		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::Append(ValueType pChar)
		{
			bcUINT32 lStrSize = 1;
			mFirst = (PointerType)mAllocator.reAlloc(mFirst, sizeof(ValueType) * (mSize + lStrSize + 1));
			Trait::Copy(mFirst + mSize, &pChar, lStrSize);
			mSize += lStrSize;

			*(mFirst + mSize) = Trait::End();

			return *this;
		};
		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::Append(const ValueType* pString)
		{
			_Append(pString);

			return *this;
		};
		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::Append(const ThisType& pOther)
		{
			_Append((ValueType*)pOther.mFirst);

			return *this;
		};

		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::operator+=(ValueType pChar)
		{
			Append(pChar);

			return *this;
		};
		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::operator+=(const ValueType* pString)
		{
			Append(pString);

			return *this;
		};
		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::operator+=(const ThisType& pOther)
		{
			Append(pOther);

			return *this;
		};

		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::operator+(ValueType pChar)
		{
			Append(pChar);

			return *this;
		};
		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::operator+(const ValueType* pString)
		{
			Append(pString);

			return *this;
		};
		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ThisType&
		bcBaseString< T, Trait, Allocator >::operator+(const ThisType& pOther)
		{
			Append(pOther);

			return *this;
		};

		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::Swap(ThisType& pOther)
		{
			std::swap(mSize, pOther.mSize);
			std::swap(mAllocator, pOther.mAllocator);
			std::swap(mFirst, pOther.mFirst);
		};

		/*Iterator       Begin();
		Const_Iterator Begin() const;
        
		Iterator       End();
		Const_Iterator End() const;*/

		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::PointerType
		bcBaseString< T, Trait, Allocator >::Data()
		{
			return mFirst;
		};
		template < typename T, class Trait, class Allocator >
		const typename bcBaseString< T, Trait, Allocator >::PointerType
		bcBaseString< T, Trait, Allocator >::Data() const
		{
			return mFirst;
		};
		template < typename T, class Trait, class Allocator >
		bcBaseString< T, Trait, Allocator >::operator RawPointerType() const
		{
			return (ValueType*)mFirst;
		};
		template < typename T, class Trait, class Allocator >
		const typename bcBaseString< T, Trait, Allocator >::ValueType*
		bcBaseString< T, Trait, Allocator >::c_str() const
		{
			return (ValueType*)mFirst;
		};

		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ValueType&
		bcBaseString< T, Trait, Allocator >::operator[](bcUINT32 n)
		{
			return *(mFirst + n);
		};
		template < typename T, class Trait, class Allocator >
		const typename bcBaseString< T, Trait, Allocator >::ValueType&
		bcBaseString< T, Trait, Allocator >::operator[](bcUINT32 n) const
		{
			return *(mFirst + n);
		};

		template < typename T, class Trait, class Allocator >
		typename bcBaseString< T, Trait, Allocator >::ValueType&
		bcBaseString< T, Trait, Allocator >::At(bcUINT32 n)
		{
			return *(mFirst + n);
		};
		template < typename T, class Trait, class Allocator >
		const typename bcBaseString< T, Trait, Allocator >::ValueType&
		bcBaseString< T, Trait, Allocator >::At(bcUINT32 n) const
		{
			return *(mFirst + n);
		};

		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::PushBack(ValueType pChar)
		{
			mFirst = (PointerType)mAllocator.reAlloc(mFirst, sizeof(ValueType) * (mSize + 1 + 1));
			*(mFirst + mSize) = pChar;
			++mSize;

			*(mFirst + mSize) = Trait::End();
		};
		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::PushBack(const ValueType* pString)
		{
			_Append(pString);
		};
		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::PushBack(const ThisType& pOther)
		{
			_Append((ValueType*)pOther.mFirst);
		};
		//Iterator PushBack(const ReferenceType pValue);
		//void PushBack();
		//Iterator PushBack();

		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::PopBack()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			mFirst = (PointerType)mAllocator.reAlloc(mFirst, sizeof(ValueType) * (mSize - 1 + 1));
			--mSize;

			*(mFirst + mSize) = Trait::End();
		};

		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::Insert(bcUINT32 pPosition, ValueType pChar)
		{
			bcUINT32 lStrSize = 1;
			bcUINT32 lNewSize = mSize + lStrSize;
			mFirst = (PointerType)mAllocator.reAlloc(mFirst, sizeof(ValueType) * (lNewSize + 1));
			Trait::Move(mFirst + pPosition + lStrSize, mFirst + pPosition, mSize - pPosition);
			Trait::Copy(mFirst + pPosition, &pChar, lStrSize);
			mSize = lNewSize;

			*(mFirst + mSize) = Trait::End();
		};
		/*template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::Insert(bcUINT32 pPosition, Const_Pointer pString)
		{
			_Insert(pPosition, pString);
		};*/
		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::Insert(bcUINT32 pPosition, const ValueType* pString)
		{
			_Insert(pPosition, pString);
		};
		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::Insert(bcUINT32 pPosition, const ThisType& pOther)
		{
			_Insert(pPosition, (ValueType*)pOther.mFirst);
		};
		//Iterator Insert(Iterator pPosition, const Reference pValue);

		//Iterator Erase(Iterator pPosition);
		//Iterator Erase(Iterator pFirst, Iterator pLast);
		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::Erase(bcUINT32 pPosition)
		{
			_Erase(pPosition, 1);
		};
		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::Erase(bcUINT32 pPosition, bcINT32 n)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(n != 0);
#endif
			if(n > 0)
				_Erase(pPosition, n);
			else if(n < 0)
				_Erase(pPosition - abs(n), n);
		};
		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::Erase(bcUINT32 pFirst, bcUINT32 pLast)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(pLast - pFirst > 0);
#endif
			_Erase(pFirst, pLast - pFirst);
		};

		template < typename T, class Trait, class Allocator >
		void bcBaseString< T, Trait, Allocator >::Clear()
		{
			if(mFirst)
				mAllocator.free(mFirst);
			mSize = 0;
		};

		template < typename T, class Trait, class Allocator >
		bcUINT32 bcBaseString< T, Trait, Allocator >::Hash() const
		{
			bcUINT32 lHash = 0;
			bcUINT32 lLength = mSize;
			ValueType* lRaw = (ValueType*)mFirst;
			bcUINT32 i;
			for( i = 0; i < lLength; i++ )
			{
				lHash += ( (i + 1) * lRaw[i] );
			}
			return lHash;
		};

		template < typename T, class Trait, class Allocator >
		bcINT32 bcBaseString< T, Trait, Allocator >::Compare(const ValueType* pString) const
		{
			return _Compare(pString);
		};
		template < typename T, class Trait, class Allocator >
		bcINT32 bcBaseString< T, Trait, Allocator >::Compare(const ThisType& pOther) const
		{
			return _Compare((ValueType*)pOther.mFirst);
		};
		template < typename T, class Trait, class Allocator >
		bcBOOL bcBaseString< T, Trait, Allocator >::operator==(const ValueType* pString) const
		{
			return (_Compare(pString) == 0)? bcTRUE: bcFALSE;
		};
		template < typename T, class Trait, class Allocator >
		bcBOOL bcBaseString< T, Trait, Allocator >::operator==(const ThisType& pOther) const
		{
			return (_Compare((ValueType*)pOther.mFirst) == 0)? bcTRUE: bcFALSE; 
		};
		template < typename T, class Trait, class Allocator >
		bcBOOL bcBaseString< T, Trait, Allocator >::operator!=(const ValueType* pString) const
		{
			return (_Compare(pString) != 0)? bcTRUE: bcFALSE;
		};
		template < typename T, class Trait, class Allocator >
		bcBOOL bcBaseString< T, Trait, Allocator >::operator!=(const ThisType& pOther) const
		{
			return (_Compare((ValueType*)pOther.mFirst) != 0)? bcTRUE: bcFALSE; 
		};
		template < typename T, class Trait, class Allocator >
		bcBOOL bcBaseString< T, Trait, Allocator >::operator<(const ValueType* pString) const
		{
			return (_Compare(pString) < 0)? bcTRUE: bcFALSE;
		};
		template < typename T, class Trait, class Allocator >
		bcBOOL bcBaseString< T, Trait, Allocator >::operator<(const ThisType& pOther) const
		{
			return (_Compare((ValueType*)pOther.mFirst) < 0)? bcTRUE: bcFALSE; 
		};
		template < typename T, class Trait, class Allocator >
		bcBOOL bcBaseString< T, Trait, Allocator >::operator>(const ValueType* pString) const
		{
			return (_Compare(pString) > 0)? bcTRUE: bcFALSE;
		};
		template < typename T, class Trait, class Allocator >
		bcBOOL bcBaseString< T, Trait, Allocator >::operator>(const ThisType& pOther) const
		{
			return (_Compare((ValueType*)pOther.mFirst) > 0)? bcTRUE: bcFALSE; 
		};

		typedef bcBaseString< bcCHAR, bcCharTrait/*< P(bcCHAR) >*/, bcContainerAllocation<> > bcString;
		typedef bcBaseString< bcWCHAR, bcWCharTrait/*< P(bcWCHAR) >*/, bcContainerAllocation<> > bcWString;

		// Parses a string into a number
		template<typename T> bcInline T Parse(const bcWString& pStr)
		{
			std::wistringstream stream(pStr.c_str());
			wchar_t c;
			T x;
			if (!(str >> x) || stream.get(c))
				throw Exception(L"Can't parse string \"" + str + L"\"");
			return x;
		}

		// Converts a number to a string
		template<typename T> bcInline bcString ToString(const T& val)
		{
			std::ostringstream stream;
			if (!(stream << val))
				throw Exception(L"Error converting value to string");
			return bcString(stream.str().c_str());
		}

		// Converts a number to a string
		template<typename T> bcInline bcWString ToStringW(const T& val)
		{
			std::wostringstream stream;
			if (!(stream << val))
				throw Exception(L"Error converting value to string");
			return bcWString(stream.str().c_str());
		}

		// Converts a number to an ansi string
		template<typename T> bcInline bcString ToAnsiString(const T& val)
		{
			std::ostringstream stream;
			if (!(stream << val))
				throw Exception(L"Error converting value to string");
			return bcString(stream.str().c_str());
		}
	}
}

#endif