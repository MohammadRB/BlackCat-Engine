// [12/23/2016 MRB]

#pragma once

#include "Core/Memory/bcAlloc.h"
#include "Core/Utility/bcRefCountPtr.h"
#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/PipelineStage/bcInputAssemblerStage.h"
#include "GraphicImp/PipelineStage/bcStreamOutputStage.h"
#include "GraphicImp/PipelineStage/bcRasterizerStage.h"
#include "GraphicImp/PipelineStage/bcOutputMergerStage.h"
#include "GraphicImp/PipelineStage/bcVertexStage.h"
#include "GraphicImp/PipelineStage/bcHullStage.h"
#include "GraphicImp/PipelineStage/bcDomainStage.h"
#include "GraphicImp/PipelineStage/bcGeometryStage.h"
#include "GraphicImp/PipelineStage/bcPixelStage.h"
#include "GraphicImp/PipelineStage/bcComputeStage.h"

namespace black_cat
{
	namespace graphic
	{
		/**
		 * \brief Used to simulate reference counting for objects that does not have native equivalent
		 * \tparam T 
		 */
		template< typename T >
		class bc_refcount_proxy : public T, public core::bc_ref_count
		{
		public:
			bc_refcount_proxy();

			bc_refcount_proxy(const bc_refcount_proxy&);

			~bc_refcount_proxy();

			bc_refcount_proxy& operator=(const bc_refcount_proxy&);

			void add_ref();

			/**
			 * \brief Return true if proxy has been released
			 * \return 
			 */
			bool release();
		};

		template< typename T >
		bc_refcount_proxy<T>::bc_refcount_proxy() = default;

		template< typename T >
		bc_refcount_proxy<T>::bc_refcount_proxy(const bc_refcount_proxy&) = default;

		template< typename T >
		bc_refcount_proxy<T>::~bc_refcount_proxy() = default;

		template< typename T >
		bc_refcount_proxy<T>& bc_refcount_proxy<T>::operator=(const bc_refcount_proxy&) = default;

		template< typename T >
		void bc_refcount_proxy<T>::add_ref()
		{
			bc_ref_count::add_ref();
		}

		template< typename T >
		bool bc_refcount_proxy<T>::release()
		{
			if(bc_ref_count::dec_ref())
			{
				bc_refcount_proxy* l_this = this;
				BC_DELETE(l_this);
				return true;
			}

			return false;
		}
	}
}