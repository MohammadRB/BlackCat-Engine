#pragma once

#ifndef bcTree_H
#define bcTree_H

#include "CorePCH.h"
#include "bcContainer.h"
#include "CoreUtility.h"

namespace BlackCat
{
	namespace Core
	{

		// == bcTree_ConstIterator =======================================
//		template < class Tree >
//		class bcTree_ConstIterator : public bcIteratorBase
//		{
//		public:
//			typedef typename Tree::NodePtr NodePtr;
//			typedef typename Tree::Value Value;
//			typedef typename Tree::Reference Reference;
//			typedef const Reference Const_Reference;
//			typedef typename Tree::Pointer Pointer;
//			typedef const Pointer Const_Pointer;
//			typedef typename Tree::RawPointer RawPointer;
//			typedef const RawPointer Const_RawPointer;
//		private:
//
//		protected:
//			NodePtr mPtr;
//
//		public:
//			bcTree_ConstIterator() : bcIteratorBase(NULL), mPtr()
//			{
//			};
//			bcTree_ConstIterator(NodePtr pPtr, RP(bcContainer_Base) pCont) : bcIteratorBase(pCont), mPtr(pPtr)
//			{
//			};
//			bcTree_ConstIterator(const bcTree_ConstIterator<Tree>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//			};
//			~bcTree_ConstIterator()
//			{
//			};
//
//			Pointer Get()
//			{
//				return mPtr;
//			};
//			bcBOOL Validate()
//			{
//				return (mPtr
//						&& mCont);
//			};
//
//			Const_Reference operator* () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont);
//#endif
//				return Tree::NodeValue(mPtr);
//			};
//			Const_RawPointer operator-> () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont);
//#endif
//				return &Tree::NodeValue(mPtr);
//			};
//			bcTree_ConstIterator<Tree>& operator= (bcTree_ConstIterator<Tree>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//
//				return *this;
//			};
//
//			bcBOOL operator== (bcTree_ConstIterator<Tree>& pOther) const
//			{
//				if(mPtr == pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator!= (bcTree_ConstIterator<Tree>& pOther) const
//			{
//				if(mPtr != pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			/*bcBOOL operator> (bcTree_ConstIterator<Tree>& pOther) const
//			{
//				if(mPtr > pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator>= (bcTree_ConstIterator<Tree>& pOther) const
//			{
//				if(mPtr >= pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator< (bcTree_ConstIterator<Tree>& pOther) const
//			{
//				if(mPtr < pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator<= (bcTree_ConstIterator<Tree>& pOther) const
//			{
//				if(mPtr <= pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};*/
//
//			bcTree_ConstIterator<Tree> operator+ (bcINT32 pNum) const
//			{
//				bcTree_ConstIterator<Tree> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
//					lTemp.mPtr = Tree::NodeSuccessor(lTemp.mPtr);
//#ifdef BC_CONTAINER_DEBUG
//					bcAssert(lTemp.mPtr);
//#endif
//				}
//				return lTemp;
//			};
//			bcTree_ConstIterator<Tree>& operator+= (bcINT32 pNum)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
//					mPtr = Tree::NodeSuccessor(mPtr);
//#ifdef BC_CONTAINER_DEBUG
//					bcAssert(mPtr);
//#endif
//				}
//				return *this;
//			};
//			void operator++ ()
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont);
//#endif
//				mPtr = Tree::NodeSuccessor(mPtr);
////#ifdef BC_CONTAINER_DEBUG
////				bcAssert(mPtr);
////#endif
//			};
//			void operator++ (bcINT32)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont);
//#endif
//				mPtr = Tree::NodeSuccessor(mPtr);
////#ifdef BC_CONTAINER_DEBUG
////				bcAssert(mPtr);
////#endif
//			};
//			bcTree_ConstIterator<Tree> operator- (bcINT32 pNum) const
//			{
//				bcTree_ConstIterator<Tree> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
//					lTemp.mPtr = Tree::NodePreDecessor(lTemp.mPtr);
////#ifdef BC_CONTAINER_DEBUG
////					bcAssert(lTemp.mPtr);
////#endif
//				}
//				return lTemp;
//			};
//			bcTree_ConstIterator<Tree>& operator-= (bcINT32 pNum)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
//					mPtr = Tree::NodePreDecessor(mPtr);
////#ifdef BC_CONTAINER_DEBUG
////					bcAssert(mPtr);
////#endif
//				}
//				return *this;
//			};
//			void operator-- ()
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont );
//#endif
//				mPtr = Tree::NodePreDecessor(mPtr);
////#ifdef BC_CONTAINER_DEBUG
////				bcAssert(mPtr);
////#endif
//			};
//			void operator-- (bcINT32)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont);
//#endif
//				mPtr = Tree::NodePreDecessor(mPtr);
////#ifdef BC_CONTAINER_DEBUG
////				bcAssert(mPtr);
////#endif
//			};
//		};
//
//		// == bcTree_Iterator ============================================
//		template < class Tree >
//		class bcTree_Iterator : public bcTree_ConstIterator< Tree >
//		{
//		private:
//
//		protected:
//			
//		public:
//			bcTree_Iterator() : bcTree_ConstIterator()
//			{
//			};
//			bcTree_Iterator(NodePtr pPtr, RP(bcContainer_Base) pCont) : bcTree_ConstIterator(pPtr, pCont)
//			{
//			};
//			bcTree_Iterator(const bcTree_Iterator<Tree>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//			};
//			~bcTree_Iterator()
//			{
//			};
//
//			Reference operator* () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont);
//#endif
//				return Tree::NodeValue(mPtr);
//			};
//			RawPointer operator-> () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont);
//#endif
//				return &Tree::NodeValue(mPtr);
//			};
//			bcTree_Iterator<Tree>& operator= (bcTree_Iterator<Tree>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//
//				return *this;
//			};
//
//			bcTree_Iterator<Tree> operator+ (bcINT32 pNum) const
//			{
//				bcList_Iterator<Vec> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
//					lTemp.mPtr = Tree::NodeSuccessor(lTemp.mPtr);
////#ifdef BC_CONTAINER_DEBUG
////					bcAssert(lTemp.mPtr);
////#endif
//				}
//				return lTemp;
//			};
//			bcTree_Iterator<Tree> operator- (bcINT32 pNum) const
//			{
//				bcTree_Iterator<Tree> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
//					lTemp.mPtr = Tree::NodePreDecessor(lTemp.mPtr);
////#ifdef BC_CONTAINER_DEBUG
////					bcAssert(lTemp.mPtr);
////#endif
//				}
//				return lTemp;
//			};
//		};

		// == bcTree_Node ================================================
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		class bcTree_Node : public bcContainer< T, Allocator >
		{
		public:
			typedef KT KeyType;
			typedef CMP KeyCompareType;

		protected:
			struct Node : public bcContainerNode< ValueType >
			{
				//Value mVal;
				KeyType mKey;
				Node* mParent;
				Node* mLeft;
				Node* mRight;
				bcUINT16 mLeftHeight;
				bcUINT16 mRightHeight;

				Node() {};
				template < typename KeyT >
				Node(KeyT&& pKey) : mKey(std::forward<KeyT>(pKey)), mValue()
				{};
				template < typename KeyT, typename ValueT >
				Node(KeyT&& pKey, ValueT&& pValue)
				{
					mKey = std::forward<KeyT>(pKey);
					mValue = std::forward<ValueT>(pValue);
				};

				~Node() {}
			};
			typedef Node NodeType;

		protected:
			// Return true if we rotate pNode
			bcInline bcBOOL CheckBalance(NodeType*& pRoot, NodeType*& pNode)
			{
				if(abs(NodeLeftHeight(pNode) - NodeRightHeight(pNode)) > MaxDiff)
				{
					NodeType* lChild;
					if(NodeLeftHeight(pNode) > NodeRightHeight(pNode))
					{
						lChild = NodeLeft(pNode);
						if(NodeRight(lChild))
							NodeLeftHeight(pNode) = std::max<bcUINT16>(NodeLeftHeight(NodeRight(lChild)), NodeRightHeight(NodeRight(lChild))) + 1;
						else
							NodeLeftHeight(pNode) = 0;
						NodeRightHeight(lChild) = std::max<bcUINT16>(NodeLeftHeight(pNode), NodeRightHeight(pNode)) + 1;
						NodeRotateRight(pRoot, pNode);
						return bcTRUE;
					}
					else
					{
						lChild = NodeRight(pNode);
						if(NodeLeft(lChild))
							NodeRightHeight(pNode) = std::max<bcUINT16>(NodeLeftHeight(NodeLeft(lChild)), NodeRightHeight(NodeLeft(lChild))) + 1;
						else
							NodeRightHeight(pNode) = 0;
						NodeLeftHeight(lChild) = std::max<bcUINT16>(NodeLeftHeight(pNode), NodeRightHeight(pNode)) + 1;
						NodeRotateLeft(pRoot, pNode);
						return bcTRUE;
					}
				}
				return bcFALSE;
			};
			// pNode : added node or node that will be removed
			// pAddRem : 1 if add a node, -1 if remove a node
			bcInline void RefreshNodeHeights(NodeType*& pRoot, NodeType*& pNode, bcINT16 pAddRem)
			{
				// Update parents left and right heights
				NodeType* lCurrNode = pNode;
				NodeType* lPNode = NodeParent(lCurrNode);

				// If current node is not root
				if(lPNode)
				{
					// Plus or mines one from parent
					if(NodeLeft(lPNode) == lCurrNode)
					{
						NodeLeftHeight(lPNode) += pAddRem;
						// If we want to remove a node, check parent balance
						if(pAddRem == -1)
						{
							if(CheckBalance(pRoot, lPNode))
								// Because we rotate parent, parent has moved one
								// level down, so we move it to up
								lPNode = NodeParent(lPNode);
						}
					}
					else
					{
						NodeRightHeight(lPNode) += pAddRem;
						// If we want to remove a node, check parent balance
						if(pAddRem == -1)
						{
							if(CheckBalance(pRoot, lPNode))
								// Because we rotate parent, parent has moved one
								// level down, so we move it to up
								lPNode = NodeParent(lPNode);
						}
					}
					lCurrNode = lPNode;
					lPNode = NodeParent(lCurrNode);
					while(lPNode)
					{
						bcUINT16 lMax = std::max<bcUINT16>(NodeLeftHeight(lCurrNode), NodeRightHeight(lCurrNode));
						if(NodeLeft(lPNode) == lCurrNode)
						{
							//if(lMax + 1 == NodeLeftHeight(lPNode))
							//	// No Update require, because right sub-tree is higher
							//	// or equal to left and add node or delete node in left 
							//	// dont impact on parent height
							//	break;
							//else
								NodeLeftHeight(lPNode) = lMax + 1;
						}
						else
						{
							//if(lMax + 1 == NodeRightHeight(lPNode))
							//	// No Update require, because left sub-tree is higher
							//	// or equal to right and add node or delete node in right
							//	// dont impact on parent height
							//	break;
							//else
								NodeRightHeight(lPNode) = lMax + 1;
						}

						// If currParent is not balance rotate it to left or
						// right and currect some node heights
						if(CheckBalance(pRoot, lPNode))
							// Because we rotated parent, parent have moved to under
							// curr, so we move parent to point to curr. in next lines
							// parent will become parent of lCurrNode
							lPNode = lCurrNode;

						lCurrNode = lPNode;
						lPNode = NodeParent(lCurrNode);
					}
				}
			};

			KeyCompareType mComp;

			// Perfect forwarding
			template < typename KeyType>
			bcInline NodeType* NewNode(NodeType*& pRoot, KeyType&& pKey)
			{
				NodeType* lNode = (NodeType*)mAllocator.alloc(sizeof(NodeType));
				//new ((RawPointer)lNode) Value;
				new (lNode)NodeType(std::forward<KeyType>(pKey));

				//NodeKey(lNode) = pKey;
				NodeLeft(lNode) = NULL;
				NodeRight(lNode) = NULL;
				NodeLeftHeight(lNode) = 0;
				NodeRightHeight(lNode) = 0;

				NodeType* lCurrNode = pRoot;
				NodeType* lPNode = nullptr;

				while(lCurrNode)
				{
					lPNode = lCurrNode;
					if(mComp(pKey, NodeKey(lCurrNode)))
						lCurrNode = NodeLeft(lCurrNode);
					else
						lCurrNode = NodeRight(lCurrNode);
				}

				NodeParent(lNode) = lPNode;

				if(!lPNode)
					pRoot = lNode;
				else if(mComp(pKey, NodeKey(lPNode)))
					NodeLeft(lPNode) = lNode;
				else
					NodeRight(lPNode) = lNode;

				RefreshNodeHeights(pRoot, lNode, 1);

				return lNode;
			};
			template < typename KeyType, typename ValueType>
			bcInline NodeType* NewNode(NodeType*& pRoot, KeyType&& pKey, ValueType&& pVal)
			{
				NodeType* lNode = (NodeType*)mAllocator.alloc(sizeof(NodeType));
				//new ((RawPointer)lNode) Value(pVal);
				new (lNode)NodeType(std::forward<KeyType>(pKey), std::forward<ValueType>(pVal));

				//NodeKey(lNode) = pKey;
				NodeLeft(lNode) = NULL;
				NodeRight(lNode) = NULL;
				NodeLeftHeight(lNode) = 0;
				NodeRightHeight(lNode) = 0;

				NodeType* lCurrNode = pRoot;
				NodeType* lPNode = nullptr;

				while(lCurrNode)
				{
					lPNode = lCurrNode;
					if(mComp(pKey, NodeKey(lCurrNode)))
						lCurrNode = NodeLeft(lCurrNode);
					else
						lCurrNode = NodeRight(lCurrNode);
				}

				NodeParent(lNode) = lPNode;

				if(!lPNode)
					pRoot = lNode;
				else if(mComp(pKey, NodeKey(lPNode)))
					NodeLeft(lPNode) = lNode;
				else
					NodeRight(lPNode) = lNode;

				RefreshNodeHeights(pRoot, lNode, 1);

				return lNode;
			};
			//bcInline NodePtr NewNode(NodePtrRef pRoot, KeyReference pKey, Value&& pVal)
			//{
			//	NodePtr lNode = (NodePtr)mAllocator.Alloc(sizeof(Node));
			//	//new ((RawPointer)lNode) Value(pVal);
			//	new ((RawPointer)lNode) Node(pKey, pVal);

			//	//NodeKey(lNode) = pKey;
			//	NodeLeft(lNode) = NULL;
			//	NodeRight(lNode) = NULL;
			//	NodeLeftHeight(lNode) = 0;
			//	NodeRightHeight(lNode) = 0;

			//	NodePtr lCurrNode = pRoot;
			//	NodePtr lPNode = nullptr;

			//	while(lCurrNode)
			//	{
			//		lPNode = lCurrNode;
			//		if(mComp(pKey, NodeKey(lCurrNode)))
			//			lCurrNode = NodeLeft(lCurrNode);
			//		else
			//			lCurrNode = NodeRight(lCurrNode);
			//	}

			//	NodeParent(lNode) = lPNode;

			//	if(!lPNode)
			//		pRoot = lNode;
			//	else if(mComp(pKey, NodeKey(lPNode)))
			//		NodeLeft(lPNode) = lNode;
			//	else
			//		NodeRight(lPNode) = lNode;

			//	RefreshNodeHeights(pRoot, lNode, 1);

			//	return lNode;
			//};
			bcInline void FreeNode(NodeType*& pRoot, NodeType*& pNode)
			{
				if(!NodeLeft(pNode) && !NodeRight(pNode))
				{
					RefreshNodeHeights(pRoot, pNode, -1);

					// If curr node is not root
					if(NodeParent(pNode))
					{
						if(NodeLeft(NodeParent(pNode)) == pNode)
							NodeLeft(NodeParent(pNode)) = NULL;
						else
							NodeRight(NodeParent(pNode)) = NULL;
					}
					else
						pRoot = NULL;

					(pNode)->~Node();
					mAllocator.free(pNode);
				}
				else if((NodeLeft(pNode) && !NodeRight(pNode)) || (!NodeLeft(pNode) && NodeRight(pNode)))
				{
					RefreshNodeHeights(pRoot, pNode, -1);

					// If curr node is not root
					if(NodeParent(pNode))
					{
						if(NodeLeft(NodeParent(pNode)) == pNode)
							if(NodeLeft(pNode))
							{
								NodeLeft(NodeParent(pNode)) = NodeLeft(pNode);
								NodeParent(NodeLeft(pNode)) = NodeParent(pNode);
							}
							else
							{
								NodeLeft(NodeParent(pNode)) = NodeRight(pNode);
								NodeParent(NodeRight(pNode)) = NodeParent(pNode);
							}
						else
							if(NodeLeft(pNode))
							{
								NodeRight(NodeParent(pNode)) = NodeLeft(pNode);
								NodeParent(NodeLeft(pNode)) = NodeParent(pNode);
							}
							else
							{
								NodeRight(NodeParent(pNode)) = NodeRight(pNode);
								NodeParent(NodeRight(pNode)) = NodeParent(pNode);
							}
					}
					else
					{
						if(NodeLeft(pNode))
						{
							NodeParent(NodeLeft(pNode)) = NULL;
							pRoot = NodeLeft(pNode);
						}
						else
						{
							NodeParent(NodeRight(pNode)) = NULL;
							pRoot = NodeRight(pNode);
						}
					}

					(pNode)->~Node();
					mAllocator.free(pNode);
				}
				else
				{
					NodeType* lSucc = NodeSuccessor(pNode);
					NodeKey(pNode) = NodeKey(lSucc);
					NodeValue(pNode) = NodeValue(lSucc);
					FreeNode(pRoot, lSucc);
				}
			};
			bcInline NodeType* CopyNodeAndSubTree(NodeType* pCpyNode, NodeType* pParent)
			{
				NodeType* lNode = (NodeType*)mAllocator.alloc(sizeof(NodeType));
				new (lNode) NodeType(NodeKey(pCpyNode), NodeValue(pCpyNode));

				//NodeValue(lNode) = NodeValue(pCpyNode);
				//NodeKey(lNode) = NodeKey(pCpyNode);
				NodeLeftHeight(lNode) = NodeLeftHeight(pCpyNode);
				NodeRightHeight(lNode) = NodeRightHeight(pCpyNode);

				NodeParent(lNode) = pParent;
				if(NodeLeft(pCpyNode))
					NodeLeft(lNode) = CopyNodeAndSubTree(NodeLeft(pCpyNode), lNode);
				else
					NodeLeft(lNode) = NULL;
				if(NodeRight(pCpyNode))
					NodeRight(lNode) = CopyNodeAndSubTree(NodeRight(pCpyNode), lNode);
				else
					NodeRight(lNode) = NULL;

				return lNode;
			};

			bcInline /*static*/ NodeType* NodeFind(NodeType* pRoot, const KeyType& pKey)
			{
				NodeType* lCurrNode = pRoot;

				while(lCurrNode && NodeKey(lCurrNode) != pKey)
				{
					if(mComp(pKey, NodeKey(lCurrNode)))
						lCurrNode = NodeLeft(lCurrNode);
					else
						lCurrNode = NodeRight(lCurrNode);
				}

				return lCurrNode;
			};

			bcInline /*static*/ void NodeRotateLeft(NodeType*& pRoot, NodeType*& pNode)
			{
				NodeType* lRight = NodeRight(pNode);

				NodeRight(pNode) = NodeLeft(lRight);
				if(NodeLeft(lRight))
					NodeParent(NodeLeft(lRight)) = pNode;
				NodeParent(lRight) = NodeParent(pNode);
				if(NodeParent(pNode) == NULL)
					pRoot = lRight;
				else if(pNode == NodeLeft(NodeParent(pNode)))
					NodeLeft(NodeParent(pNode)) = lRight;
					else 
						NodeRight(NodeParent(pNode)) = lRight;
				NodeLeft(lRight) = pNode;
				NodeParent(pNode) = lRight;
			};
			bcInline /*static*/ void NodeRotateRight(NodeType*& pRoot, NodeType*& pNode)
			{
				NodeType* lLeft = NodeLeft(pNode);

				NodeLeft(pNode) = NodeRight(lLeft);
				if(NodeRight(lLeft))
					NodeParent(NodeRight(lLeft)) = pNode;
				NodeParent(lLeft) = NodeParent(pNode);
				if(NodeParent(pNode) == NULL)
					pRoot = lLeft;
				else if(pNode == NodeLeft(NodeParent(pNode)))
					NodeLeft(NodeParent(pNode)) = lLeft;
					else 
						NodeRight(NodeParent(pNode)) = lLeft;
				NodeRight(lLeft) = pNode;
				NodeParent(pNode) = lLeft;
			};

			bcInline static NodeType*& NodeParent(NodeType* pNode)
			{
				return pNode->mParent;
			};
			bcInline static NodeType*& NodeLeft(NodeType* pNode)
			{
				return pNode->mLeft;
			};
			bcInline static NodeType*& NodeRight(NodeType* pNode)
			{
				return pNode->mRight;
			};
			bcInline static ValueType& NodeValue(NodeType* pNode)
			{
				return pNode->mValue;
			};
			bcInline static KeyType& NodeKey(NodeType* pNode)
			{
				return pNode->mKey;
			};
			bcInline static bcUINT16& NodeLeftHeight(NodeType* pNode)
			{
				return pNode->mLeftHeight;
			};
			bcInline static bcUINT16& NodeRightHeight(NodeType* pNode)
			{
				return pNode->mRightHeight;
			};

			bcInline static NodeType*& NodeSuccessor(NodeType* pNode)
			{
				NodeType* lCurrNode = pNode;
				NodeType* lPNode;

				if(NodeRight(lCurrNode))
					return NodeMin(NodeRight(lCurrNode));

				lPNode = NodeParent(lCurrNode);
				while(lPNode && lCurrNode == NodeRight(lPNode))
				{
					lCurrNode = lPNode;
					lPNode = NodeParent(lPNode);
				}

				return lPNode;
			};
			bcInline static NodeType*& NodePreDecessor(NodeType* pNode)
			{
				NodeType* lCurrNode = pNode;
				NodeType* lPNode;

				if(NodeLeft(lCurrNode))
					return NodeMax(NodeLeft(lCurrNode));

				lPNode = NodeParent(lCurrNode);
				while(lPNode && lCurrNode == NodeLeft(lPNode))
				{
					lCurrNode = lPNode;
					lPNode = NodeParent(lPNode);
				}

				return lPNode;
			};

			bcInline static NodeType*& NodeMax(NodeType* pNode)
			{
				NodeType* lNode = pNode;
				while(NodeRight(lNode) != NULL)
					lNode = NodeRight(lNode);
				return lNode;
			};
			bcInline static NodeType*& NodeMin(NodeType* pNode)
			{
				NodeType* lNode = pNode;
				while(NodeLeft(lNode) != NULL)
					lNode = NodeLeft(lNode);
				return lNode;
			};

		public:
			bcTree_Node() {};
			~bcTree_Node() {};
		};

		// == bcTree_Base ================================================
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		class bcTree_Base : public bcTree_Node < KT, T, MaxDiff, CMP, Allocator >, public bcIteratorProvider< T >
		{
		private:

		protected:
			NodeType* mRoot;

		public:
			bcTree_Base() 
			{
				mRoot = NULL;
				bcAssert(MaxDiff > 0);
			};
			~bcTree_Base() {};

			ValueType& nodeValue(NodeBaseType* pNode) override
			{
				return NodeValue(static_cast<NodeType*>(pNode));
			}
			NodeBaseType* moveForward(NodeBaseType* pNode) override
			{
				return NodeSuccessor(static_cast<NodeType*>(pNode));
			}
			NodeBaseType* moveForward(NodeBaseType* pNode, bcUINT32 pCount) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				for (bcUINT32 i = 0; i < pCount; ++i)
				{ 
					lNode = NodeSuccessor(lNode); 
				}
				return lNode;
			}
			NodeBaseType* moveBackward(NodeBaseType* pNode) override
			{
				return NodePreDecessor(static_cast<NodeType*>(pNode));
			}
			NodeBaseType* moveBackward(NodeBaseType* pNode, bcUINT32 pCount) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				for (bcUINT32 i = 0; i < pCount; ++i)
				{
					lNode = NodePreDecessor(lNode);
				}
				return lNode;
			}
			bcBOOL isValid(const NodeBaseType* pNode) const override
			{ return (pNode != bcNULL); }
		};

		// == bcTree =====================================================
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator = bcContainerAllocation<> >
		class bcTree : public bcTree_Base< KT, T, MaxDiff, CMP, Allocator >
		{
		public:
			typedef bcTree < KT, T, MaxDiff, CMP, Allocator > ThisType;
			//typedef bcTree_Iterator< bcTree_Base< KT, T, MaxDiff, CMP, Allocator, PT > > Iterator;
			//typedef bcTree_ConstIterator< bcTree_Base< KT, T, MaxDiff, CMP, Allocator, PT > > Const_Iterator;
			typedef bcIterator< T > Iterator;
			typedef bcConstIterator< T > ConstIterator;
			typedef bcReference< T > Reference;
			typedef bcConstReference< T > ConstReference;
			typedef bcPair< KT, T > Pair;
			typedef const Pair Const_Pair;
		private:

		protected:

		public:
			bcTree();
			bcTree(const ThisType& pOther);
			bcTree(ThisType&& pOther);
			~bcTree();

			ThisType& operator=(const ThisType& pOther);
			void Swap(ThisType& pOther);

			Iterator       Begin();
			ConstIterator  Begin() const;

			Iterator       RBegin();
			ConstIterator  RBegin() const;
        
			Iterator       End();
			ConstIterator  End() const;

			Iterator       REnd();
			ConstIterator  REnd() const;

			/*Pointer       Data();
			Const_Pointer Data() const;*/

			Reference       operator[](const KeyType& n);
			ConstReference  operator[](const KeyType& n) const;

			Reference       At(const KeyType& n);
			ConstReference  At(const KeyType& n) const;

			Iterator       Find(const KeyType& key);
			ConstIterator  Find(const KeyType& key) const;

			//void PushBack(Const_Pair& pValue);
			Iterator PushBack(Pair& pValue);
			Iterator PushBack(Pair&& pValue);
			/*void PushBack();
			Iterator PushBack();*/

			void PopBack(const KeyType& pKey);

			//void Insert(Const_Pair& pValue);
			Iterator Insert(Pair& pValue);
			Iterator Insert(Pair&& pValue);

			void Erase(Iterator pPosition);
			void Erase(Iterator pFirst, Iterator pLast);

			void Clear();
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		bcTree< KT, T, MaxDiff, CMP, Allocator >::bcTree()
		{
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		bcTree< KT, T, MaxDiff, CMP, Allocator >::bcTree(const ThisType& pOther)
		{
			Clear();
			mSize = pOther.mSize;
			mAllocator = pOther.mAllocator;
			mComp = pOther.mComp;
			mRoot = CopyNodeAndSubTree(pOther.mRoot, mRoot);
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		bcTree< KT, T, MaxDiff, CMP, Allocator >::bcTree(ThisType&& pOther)
		{
			Clear();

			mSize = pOther.mSize;
			mAllocator = pOther.mAllocator;
			mComp = pOther.mComp;

			mRoot = pOther.mRoot;

			pOther.mRoot = nullptr;

			pOther.Clear();
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		bcTree< KT, T, MaxDiff, CMP, Allocator >::~bcTree()
		{
			Clear();
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::ThisType& 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::operator=(const ThisType& pOther)
		{
			if(this != &pOther)
			{
				Clear();
				mSize = pOther.mSize;
				mAllocator = pOther.mAllocator;
				mComp = pOther.mComp;
				mRoot = CopyNodeAndSubTree(pOther.mRoot, mRoot);
			}
			return *this;
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		void bcTree< KT, T, MaxDiff, CMP, Allocator >::Swap(ThisType& pOther)
		{
			std::swap(mSize, pOther.mSize);
			std::swap(mAllocator, pOther.mAllocator);
			std::swap(mComp, pOther.mComp);
			std::swap(mRoot, pOther.mRoot);
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::Iterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::Begin()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mRoot);
#endif
			return Iterator(NodeMin(mRoot), this);
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::ConstIterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::Begin() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mRoot);
#endif
			return ConstIterator(NodeMin(mRoot), this);
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::Iterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::RBegin()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mRoot);
#endif
			return Iterator(NodeMax(mRoot), this);
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::ConstIterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::RBegin() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mRoot);
#endif
			return ConstIterator(NodeMax(mRoot), this);
		};
        
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::Iterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::End()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mRoot);
#endif
			// We use null iterator for indicating End, because
			// NodeSuccessor and NodePreDecessor return Null pointer
			// when there is not any next or prev node in tree
			return Iterator(bcNULL, this);
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::ConstIterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::End() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mRoot);
#endif
			// We use null iterator for indicating End, because
			// NodeSuccessor and NodePreDecessor return Null pointer
			// when there is not any next or prev node in tree
			return ConstIterator(bcNULL, this);
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::Iterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::REnd()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mRoot);
#endif
			// We use null iterator for indicating End, because
			// NodeSuccessor and NodePreDecessor return Null pointer
			// when there is not any next or prev node in tree
			return Iterator(bcNULL, this);
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::ConstIterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::REnd() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mRoot);
#endif
			// We use null iterator for indicating End, because
			// NodeSuccessor and NodePreDecessor return Null pointer
			// when there is not any next or prev node in tree
			return ConstIterator(bcNULL, this);
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::Reference 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::operator[](const KeyType& n)
		{
//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(mRoot);
//#endif
			NodeType* lNode = NodeFind(mRoot, n);

//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(lNode);
//#endif
			return  Reference(lNode);
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::ConstReference
		bcTree< KT, T, MaxDiff, CMP, Allocator >::operator[](const KeyType& n) const
		{
//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(mRoot);
//#endif
			NodeType* lNode = NodeFind(mRoot, n);

//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(lNode);
//#endif
			return  ConstReference(lNode);
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::Reference
		bcTree< KT, T, MaxDiff, CMP, Allocator >::At(const KeyType& n)
		{
//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(mRoot);
//#endif
			NodeType* lNode = NodeFind(mRoot, n);

//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(lNode);
//#endif
			return  Reference(lNode);
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::ConstReference
		bcTree< KT, T, MaxDiff, CMP, Allocator >::At(const KeyType& n) const
		{
//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(mRoot);
//#endif
			NodeType* lNode = NodeFind(mRoot, n);

//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(lNode);
//#endif
			return  ConstReference(lNode);
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::Iterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::Find(const KeyType& key)
		{
//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(mRoot);
//#endif
			NodeType* lNode = NodeFind(mRoot, key);

//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(lNode);
//#endif
			return  Iterator(lNode, this);
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::ConstIterator
		bcTree< KT, T, MaxDiff, CMP, Allocator >::Find(const KeyType& key) const
		{
//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(mRoot);
//#endif
			NodeType* lNode = NodeFind(mRoot, key);

//#ifdef BC_CONTAINER_DEBUG
//			bcAssert(lNode);
//#endif
			return  ConstIterator(lNode, this);
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::Iterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::PushBack(Pair& pValue)
		{
			++mSize;
			return Iterator(NewNode(mRoot, pValue.mFirst, pValue.mSecond), this);
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::Iterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::PushBack(Pair&& pValue)
		{
			++mSize;
			return Iterator(NewNode(mRoot, std::forward<KeyType>(pValue.mFirst), std::forward<ValueType>(pValue.mSecond)), this);
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		void bcTree< KT, T, MaxDiff, CMP, Allocator >::PopBack(const KeyType& pKey)
		{
			NodeType* lNode = NodeFind(mRoot, pKey);
#ifdef BC_CONTAINER_DEBUG
			bcAssert(lNode);
#endif
			--mSize;
			FreeNode(mRoot, lNode);
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::Iterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::Insert(Pair& pValue)
		{
			++mSize;
			return Iterator(NewNode(mRoot, pValue.mFirst, pValue.mSecond), this);
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		typename bcTree< KT, T, MaxDiff, CMP, Allocator >::Iterator 
		bcTree< KT, T, MaxDiff, CMP, Allocator >::Insert(Pair&& pValue)
		{
			++mSize;
			return Iterator(NewNode(mRoot, std::forward<KeyType>(pValue.mFirst), std::forward<ValueType>(pValue.mSecond)), this);
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		void bcTree< KT, T, MaxDiff, CMP, Allocator >::Erase(Iterator pPosition)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(pPosition.validate() && mSize > 0);
#endif
			--mSize;
			NodeType* lP = const_cast<NodeType*>(static_cast<const NodeType*>(pPosition.getNode()));
			FreeNode(mRoot, lP);
		};
		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		void bcTree< KT, T, MaxDiff, CMP, Allocator >::Erase(Iterator pFirst, Iterator pLast)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(pFirst.Validate() && pLast.Validate() && mSize > 0);
			// Check first key is lower than last
			const NodeType* lP1 = static_cast<const NodeType*>(pFirst.getNode());
			const NodeType* lP2 = static_cast<const NodeType*>(pLast.getNode());
			bcAssert(mComp(NodeKey(lP1), NodeKey(lP2)));
#endif
			Iterator lDel;
			while(pFirst != pLast)
			{
				lDel = pFirst;
				++pFirst;
				Erase(lDel);
			}
			Erase(pLast);
		};

		template < class KT, typename T, bcUINT32 MaxDiff, class CMP, class Allocator >
		void bcTree< KT, T, MaxDiff, CMP, Allocator >::Clear()
		{
			NodeType* lCurrNode = mRoot;
			NodeType* lDelNode;
			while(lCurrNode)
			{
				if(NodeLeft(lCurrNode))
					lCurrNode = NodeLeft(lCurrNode);
				else if(NodeRight(lCurrNode))
					lCurrNode = NodeRight(lCurrNode);
				else
				{
					lDelNode = lCurrNode;
					lCurrNode = NodeParent(lCurrNode);
					// If we don't want delete mRoot
					if(lCurrNode)
					{
						if(lDelNode == NodeLeft(lCurrNode))
							NodeLeft(lCurrNode) = NULL;
						else
							NodeRight(lCurrNode) = NULL;
					}

					lDelNode->~Node();
					mAllocator.free(lDelNode);
				}
			}

			mSize = 0;
			mRoot = NULL;
		};

	}
}

#endif