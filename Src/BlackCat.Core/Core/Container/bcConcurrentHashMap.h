//  [9/23/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcConditionVariable.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Container/bcConcurrentList.h"

namespace black_cat
{
	namespace core
	{
		template< typename K, typename T, typename Hasher, typename Allocator, bcSIZE ...tLevelSizes >
		class bcConcurrentHashMapBase;

		template< typename K, typename T, typename Hasher, typename Allocator, bcSIZE ...tLevelSizes >
		class bcConcurrentHashMapBase : public bc_concurrent_container<T, Allocator>, public bcConcurrentIteratorProvider<std::pair<K, T&>>
		{
		public:
			typedef Hasher HashType;
			typedef K KeyType;
			//typedef typename std::result_of<HashType>::type HashKeyType;
			typedef typename Hasher::result_type HashKeyType;
			typedef std::pair<KeyType, ValueType&> KeyValuePairType;
			struct Node;
			struct Chunk;
			/*typedef bcConcurrentList< Node, Allocator > NodeListType;
			typedef typename NodeListType::IteratorType NodeIterator;*/
			typedef bcConcurrentHashMapBase< KeyType, ValueType, HashType, AllocType, tLevelSizes... > ThisType;

		private:

		protected:
			struct Node : bc_container_node<ValueType>
			{
			private:

			protected:
				
			public:
				const Chunk* mChunk;
				HashKeyType mKey;

				Node()
				{
					mKey = nullptr;
				}
				Node(const Chunk* pChunk, const HashKeyType& pKey, const ValueType& pValue) : 
					bc_container_node(pValue), 
					mChunk(pChunk), 
					mKey(pKey) {}
				Node(const Chunk* pChunk, const HashKeyType& pKey, ValueType&& pValue) : 
					bc_container_node(std::move(pValue)), 
					mChunk(pChunk), 
					mKey(pKey) {}
				~Node() {}
			};
			typedef Node NodeType;

			struct Chunk
			{
			private:

			public:
				Chunk* mParent;
				Chunk* mSubLevels;
				bcINT16 mDepth; // Index from 0 /
				bcINT16 mIndex; // Position in parent sub levels, start from 0 /
				bcReference<ValueType> mFirstItem;
				bcReference<ValueType> mLastItem;

			public:
				Chunk()
				{
					mSubLevels = nullptr;
					mFirstItem = nullptr;
					mLastItem = nullptr;
				}
				~Chunk() {}
			};

			class NodeListType : public bcConcurrentList<Node, Allocator>
			{
			public:
				typedef typename bcConcurrentList::NodeType NodeType;
			private:

			protected:

			public:
				NodeListType(bcUINT32 pNumThread, bcUINT32 pMaxIterator) : bcConcurrentList(pNumThread, pMaxIterator) {}
				~NodeListType() {}

				void referenceNode(NodeType* pNode)
				{
					mMemMng.ref(pNode);
				}
				// In _nextNode and _prevNode we create an iterator from passed node, but passed node to this
				// functions has a reference and we must release their reference after creating iterator from them /
				void releasedNode(NodeType* pNode)
				{
					mMemMng.releaseRef(pNode);
				}
			};
			typedef typename ThisType::NodeListType::node_type NodeListNodeType;
			typedef typename ThisType::NodeListType::IteratorType NodeIterator;

			const bcSIZE mLevelCount;
			bcSIZE mLevelSizes[sizeof...(tLevelSizes)];

			HashType mHash;

			Chunk* mChunks;
			NodeListType mNodeList;

			bc_mutex mMutext;
			bc_atomic_flag mFlag;

			void _freeChunk(Chunk* pChunk, bcSIZE pLevel)
			{
				// For visibility of mSubLevels for other threads(cache coherency) we use mFlag /
				bc_atomic_flag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Acquire);

				for (bcUINT32 c = 0; c < mLevelSizes[pLevel]; ++c)
				{
					Chunk* lSubLeveles = &pChunk[c].mSubLevels;
					if (lSubLeveles)
					{
						_freeChunk(lSubLeveles, pLevel + 1);
					}
				}
				delete[] pChunk;
			}

			// Get successor chunk, this function will iterate top and down to found first next chunk /
			BC_INLINE Chunk* _getSuccessorChunk(const Chunk* pChunk)
			{
				const Chunk* lChunk = pChunk;
				Chunk* lNextChunk = nullptr;

				// For visibility of mSubLevels for other threads(cache coherency) we use mFlag /
				bc_atomic_flag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Acquire);

				while (lChunk->mSubLevels != nullptr)
				{
					bc_atomic_flag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Acquire);
					lChunk = lChunk->mSubLevels[0];
				}
				if (lChunk->mIndex != mLevelSizes[lChunk->mDepth] - 1) // If current chunk isn't last child of it's parent /
				{
					lNextChunk = lChunk + 1;
					return lNextChunk;
				}
				else
				{
					Chunk* lParentChunk = lChunk->mParent;
					if (lParentChunk != nullptr)
					{
						while (lParentChunk->mIndex == mLevelSizes[lParentChunk->mDepth] - 1) // If parent is last chunk continue to up /
						{
							Chunk lParentOfParent = lParentChunk->mParent;
							if (lParentOfParent != nullptr)
								lParentChunk = lParentOfParent;
							else
								return false;
						}
						lParentChunk++;
						return _getSuccessorChunk(lParentChunk);
					}
				}
			}

			// Get preDecessor chunk, this function will iterate top and down to found first prev chunk /
			BC_INLINE Chunk* _getPreDecessorChunk(const Chunk* pChunk)
			{
				const Chunk* lChunk = pChunk;
				Chunk* lPrevChunk = nullptr;

				// For visibility of mSubLevels for other threads(cache coherency) we use mFlag /
				bc_atomic_flag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Acquire);

				while (lChunk->mSubLevels != nullptr)
				{
					bc_atomic_flag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Acquire);
					lChunk = lChunk->mSubLevels[mLevelSizes[lChunk->mDepth] - 1];
				}
				if (lChunk->mIndex != 0) // If current chunk isn't first child of it's parent /
				{
					lPrevChunk = lChunk - 1;
					return lPrevChunk;
				}
				else
				{
					Chunk* lParentChunk = lChunk->mParent;
					if (lParentChunk != nullptr)
					{
						while (lParentChunk->mIndex == 0) // If parent is first chunk continue to up /
						{
							Chunk lParentOfParent = lParentChunk->mParent;
							if (lParentOfParent != nullptr)
								lParentChunk = lParentOfParent;
							else
								return false;
						}
						lParentChunk--;
						return _getPreDecessorChunk(lParentChunk);
					}
				}
			}

			BC_INLINE NodeListNodeType* _nextNode(NodeListNodeType* pNode, bool pReleaseParameterNode = true)
			{
				NodeType* lNode = &pNode->mValue;
				Chunk* lChunk = lNode->mChunk;
				NodeIterator lNodeIterator = mNodeList.createIteratorFromReference(bcReference(pNode));
				// Because we create an iterator from node, we must release external reference to node /
				if (pReleaseParameterNode)
					mNodeList.releasedNode(pNode);
				NodeIterator lCurrentEnd = mNodeList.createIteratorFromReference(lChunk->mLastItem);

				++lNodeIterator;
				if (lNodeIterator == lCurrentEnd)
				{
					while (true)
					{
						lChunk = _getSuccessorChunk(lChunk);
						if (!lChunk) return nullptr;

						lNodeIterator = mNodeList.createIteratorFromReference(lChunk->mFirstItem);
						lCurrentEnd = mNodeList.createIteratorFromReference(lChunk->mLastItem);
						++lNodeIterator;

						if (lNodeIterator != lCurrentEnd) return lNodeIterator;
					}
				}

				// Release end iterator because we want an additional reference for result /
				lCurrentEnd.releaseReference(); 
				NodeListNodeType* lResult = lNodeIterator->getNode();
				mNodeList.referenceNode(lResult);
				return lResult;
			}

			BC_INLINE NodeListNodeType* _prevNode(NodeListNodeType* pNode, bool pReleaseParameterNode = true)
			{
				NodeType* lNode = &pNode->mValue;
				Chunk* lChunk = lNode->mChunk;
				NodeIterator lNodeIterator = mNodeList.createIteratorFromReference(bcReference(pNode));
				// Because we create an iterator from node, we must release external reference to node /
				if (pReleaseParameterNode)
					mNodeList.releasedNode(pNode);
				NodeIterator lCurrentStart = mNodeList.createIteratorFromReference(lChunk->mFirstItem);

				--lNodeIterator;
				if (lNodeIterator == lCurrentStart)
				{
					while (lNodeIterator == lCurrentStart && !lChunk)
					{
						lChunk = _getPreDecessorChunk(lChunk);
						if (!lChunk) return nullptr;

						lNodeIterator = mNodeList.createIteratorFromReference(lChunk->mLastItem);
						lCurrentStart = mNodeList.createIteratorFromReference(lChunk->mFirstItem);
						--lNodeIterator;

						if (lNodeIterator != lCurrentStart) return lNodeIterator;
					}
					
				}

				// Release begin iterator because we want an additional reference for result /
				lCurrentStart.releaseReference();
				NodeListNodeType* lResult = lNodeIterator->getNode();
				mNodeList.referenceNode(lResult);
				return lResult;
			}

			BC_INLINE Chunk* _getFirstChuck()
			{
				// For visibility of mSubLevels for other threads(cache coherency) we use mFlag /
				bc_atomic_flag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Acquire);

				Chunk* lChunk = &mChunks[0];
				while (lChunk->mSubLevels)
				{
					bc_atomic_flag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Acquire);
					lChunk = &lChunk->mSubLevels[0];
				}

				return lChunk;
			}
			BC_INLINE Chunk* _getLastChunk()
			{
				// For visibility of mSubLevels for other threads(cache coherency) we use mFlag /
				bc_atomic_flag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Acquire);

				bcUINT32 lDepth = 0;
				Chunk* lChunk = &mChunks[mLevelSizes[lDepth] - 1];
				while (lChunk->mSubLevels)
				{
					bc_atomic_flag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Acquire);

					++lDepth;
					lChunk = &lChunk->mSubLevels[mLevelSizes[lDepth] - 1];
				}

				return lChunk;
			}

			BC_INLINE NodeListNodeType* _getFirstNode()
			{
				Chunk* lChunk = _getFirstChuck();
				NodeIterator lBeginIterator = mNodeList.createIteratorFromReference(lChunk->mFirstItem);
				NodeType* lBeginNode = lBeginIterator.getNode();
				lBeginIterator.releaseReference();
				return _nextNode(lBeginNode, false);
			}
			BC_INLINE NodeListNodeType* _getLastNode()
			{
				Chunk* lChunk = _getLastChunk();
				NodeIterator lEndIterator = mNodeList.createIteratorFromReference(lChunk->lEndIterator);
				NodeType* lEndNode = lEndIterator.getNode();
				lEndIterator.releaseReference();
				return _prevNode(lEndNode, false);
			}

			BC_INLINE HashKeyType _hashKey(const KeyType& pValue)
			{
				return mHash(pValue);
			}

			BC_INLINE Chunk* _findChunk(HashKeyType pKey)
			{
				bcUINT32 lDepth = 1;
				Chunk* lTargetChunk = &mChunks[pKey % mLevelSizes[0]];

				for (bcUINT32 l = 1; l < mLevelCount; ++l)
				{
					++lDepth;
					if (!lTargetChunk->mSubLevels)
					{
						bc_lock_guard lGaurd(mMutext);

						if (!lTargetChunk->mSubLevels)
						{
							Chunk* lChunks = new Chunk[mLevelSizes[l]];
							if (lChunks == nullptr)
							{
								// TODO check for allocation fail here
								BC_ASSERT(false);
							}
							for (bcUINT32 i = 0; i < mLevelSizes[l]; ++i)
							{
								Chunk* lCurrentChunk = &lChunks[i];
								lCurrentChunk->mParent = lTargetChunk;
								lCurrentChunk->mDepth = lDepth - 1;
								lCurrentChunk->mIndex = i;
								lCurrentChunk->mSubLevels = nullptr;
								if (lDepth == mLevelCount)
								{
									/*Node item;
									item.mKey = reinterpret_cast<void*>(i);*/
									lCurrentChunk->mFirstItem = mNodeList.pushBack().toReference();
									lCurrentChunk->mLastItem = mNodeList.pushBack().toReference();
								}
							}
							lTargetChunk->mSubLevels = lChunks;

							bc_atomic_flag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_AcqRel);
						}
					}

					lTargetChunk = &lTargetChunk->mSubLevels[pKey % mLevelSizes[l]];
				}

				return lTargetChunk;
			}

			NodeListNodeType* _insert(const KeyType& pKey, const ValueType& pValue)
			{
				HashKeyType lKey = _hashKey(pKey);
				Chunk* lChunk = _findChunk(lKey);

				NodeIterator lBegin = mNodeList.createIteratorFromReference(lChunk->mFirstItem);
				NodeIterator lInsertedItem = mNodeList.insertAfter(lBegin, Node(lChunk, lKey, pValue));

				mSize.fetch_add(1U, platform::bc_memory_order::seqcst);

				return lInsertedItem.getNode();
			}

			NodeListNodeType* _insert(KeyType&& pKey, ValueType&& pValue)
			{
				HashKeyType lKey = _hashKey(pKey);
				Chunk* lChunk = _findChunk(lKey);

				NodeIterator lBegin = mNodeList.createIteratorFromReference(lChunk->mFirstItem);
				NodeIterator lInsertedItem = mNodeList.insertAfter(lBegin, Node(lChunk, lKey, std::move(pValue)));

				mSize.fetch_add(1U, platform::bc_memory_order::seqcst);

				return lInsertedItem.getNode();
			}

			bool _remove(const KeyType& pKey, bcNullable< ValueType >* pData = nullptr)
			{
				HashKeyType lKey = _hashKey(pKey);
				Chunk* lChunk = _findChunk(lKey);

				NodeIterator lBegin = mNodeList.createIteratorFromReference(lChunk->mFirstItem);
				NodeIterator lEnd = mNodeList.createIteratorFromReference(lChunk->mLastItem);

				while (++lBegin != lEnd)
				{
					if (lBegin->mKey == lKey)
						break;
				}

				if (lBegin != lEnd)
				{
					bcNullable<NodeType> lNodeHolder;
					bcNullable<NodeType>* lNodeHodlerPointer = (pData) ? &lNodeHolder : nullptr;

					mNodeList.erase(lBegin, lNodeHodlerPointer);
					mSize.fetch_sub(1U, platform::bc_memory_order::seqcst);

					if (pData) *pData = lNodeHolder.mValue;

					return true;
				}
				else
				{
					BC_ASSERT(false);
					if (pData != nullptr) *pData = nullptr;
					return false;
				}				
			}

			bool _get(const KeyType& pKey, bcNullable< ValueType >* pData = nullptr)
			{
				HashKeyType lKey = _hashKey(pKey);
				Chunk* lChunk = _findChunk(lKey);

				NodeIterator lBegin = mNodeList.createIteratorFromReference(lChunk->mFirstItem);
				NodeIterator lEnd = mNodeList.createIteratorFromReference(lChunk->mLastItem);

				while (++lBegin != lEnd)
				{
					if (lBegin->mKey == lKey)
						break;
				}

				if (lBegin != lEnd)
				{
					pData = lBegin->second;
					return true;
				}
				else
				{
					BC_ASSERT(false);
					if (pData != nullptr) *pData = nullptr;
					return false;
				}
			}

		public:
			bcConcurrentHashMapBase(bcUINT32 pNumThread) :
				mLevelCount(sizeof...(tLevelSizes)),
				mNodeList(pNumThread, 2) // TODO change iterator count to local count + external iterator count
			{
				BC_ASSERT(mLevelCount > 0);

				bcSIZE lTempLevelSizes[sizeof...(tLevelSizes)] = { tLevelSizes... };
				std::memcpy(&mLevelSizes[0], &lTempLevelSizes, mLevelCount * sizeof(bcSIZE));

				mChunks = new Chunk[mLevelSizes[0]];
				for (bcUINT32 i = 0; i < mLevelSizes[0]; ++i)
				{
					Chunk* lCurrentChunk = &mChunks[i];
					lCurrentChunk->mDepth = 0;
					lCurrentChunk->mIndex = i;
					lCurrentChunk->mParent = nullptr;
					lCurrentChunk->mSubLevels.store((Chunk*)nullptr, platform::bc_memory_order::seqcst);
					if (mLevelCount == 1)
					{
						lCurrentChunk->mFirstItem = mNodeList.pushBack().toReference();
						lCurrentChunk->mLastItem = mNodeList.pushBack().toReference();
					}
					else
					{
						lCurrentChunk->mFirstItem = nullptr;
						lCurrentChunk->mLastItem = nullptr;
					}
				}
				
				bc_atomic_flag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Release);
					
				mSize.store(0U, platform::bc_memory_order::seqcst);
			}

			virtual ~bcConcurrentHashMapBase()
			{
				_freeChunk(mChunks, 0);
			}

			KeyValuePairType& nodeValue(NodeBaseType* pNode) override
			{
				NodeType* lNode = &(static_cast<NodeListNodeType*>(pNode))->mValue;
				return KeyValuePairType(lNode->mKey, lNode->mValue);
			}
			NodeBaseType* moveForward(NodeBaseType* pNode) override
			{
				NodeListNodeType* lNode = static_cast<NodeListNodeType*>(pNode);
				lNode = _nextNode(lNode);
				return lNode;
			}
			NodeBaseType* moveForward(NodeBaseType* pNode, bcUINT32 pCount) override
			{
				NodeListNodeType* lNode = static_cast<NodeListNodeType*>(pNode);
				for (bcUINT32 i = 0; i < pCount; ++i)
				{
					lNode = _nextNode(lNode);
				}
				return lNode;
			}
			NodeBaseType* moveBackward(NodeBaseType* pNode) override
			{
				NodeListNodeType* lNode = static_cast<NodeListNodeType*>(pNode);
				lNode = _prevNode(lNode);
				return lNode;
			}
			NodeBaseType* moveBackward(NodeBaseType* pNode, bcUINT32 pCount) override
			{
				NodeListNodeType* lNode = static_cast<NodeListNodeType*>(pNode);
				for (bcUINT32 i = 0; i < pCount; ++i)
				{
					lNode = _prevNode(lNode);
				}
				return lNode;
			}
			bool isValid(const NodeBaseType* pNode) const override
			{
				const NodeListNodeType* lNode = static_cast<const NodeListNodeType*>(pNode);
				return (lNode && !lNode->del());
			}

			void referenceIterator(bcConcurrentIteratorBase<KeyValuePairType>* pIte) override
			{
				NodeListNodeType* lNode = static_cast<NodeListNodeType*>(pIte->getNode());
				mNodeList.referenceNode(lNode);
			}
			void releaseIterator(bcConcurrentIteratorBase<KeyValuePairType>* pIte) override
			{
				NodeType* lNode = static_cast<NodeType*>(pIte->getNode());
				mNodeList.releasedNode(lNode);
			}
		};

		template< typename K, typename T, typename Hasher = std::hash<K>, typename Allocator = bcContainerAllocation<>, bcSIZE ...tLevelSizes >
		class bcConcurrentHashMap : public bcConcurrentHashMapBase< K, T, Hasher, Allocator, tLevelSizes... >
		{
		public:
			typedef bcConcurrentIterator< KeyValuePairType > IteratorType;
			typedef bcConstConcurrentIterator< KeyValuePairType > ConstIteratorType;
			typedef bcReference< KeyValuePairType > ReferenceType;
			typedef bcConstReference< KeyValuePairType > ConstReferenceType;

		private:

		protected:

		public:
			bcConcurrentHashMap(bcUINT32 pNumThread) : bcConcurrentHashMapBase(pNumThread)
			{

			}
			~bcConcurrentHashMap() {}

			IteratorType Insert(const KeyType& pKey, const ValueType& pValue)
			{
				return IteratorType(_insert(pKey, pValue), this);
			}
			IteratorType Insert(const KeyType& pKey, ValueType&& pValue)
			{
				return IteratorType(_insert(pKey, pValue), this);
			}

			bool Remove(const KeyType& pKey, bcNullable<ValueType>* pValue = nullptr)
			{
				return _remove(pKey, pValue);
			}

			bool Get(KeyType& pKey, bcNullable<ValueType>* pValue = nullptr)
			{
				return _get(pKey, pValue);
			}

			IteratorType begin()
			{
				NodeListNodeType* lNode = _getFirstNode();
				IteratorType lIterator(lNode, this);
				if (lNode)
					mNodeList.releasedNode(lNode);

				return lIterator;
			}
			IteratorType end()
			{
				IteratorType(nullptr, this);
			}
		};
	}
}