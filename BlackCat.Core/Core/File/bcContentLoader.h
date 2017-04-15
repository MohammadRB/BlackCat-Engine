// [04/08/2016 MRB]

#pragma once

#include "Core/bcExport.h"
#include "Core/bcException.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcDataDrivenParameter.h"
#include "Core/Utility/bcNullable.h"
#include "Core/Container/bcVector.h"
#include "Core/File/bcStream.h"
#include "Core/File/bcContent.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_loader_result
		{
		public:
			template< class TContent >
			explicit bc_content_loader_result(bc_object_allocator::ptr<TContent>&& p_result)
				: m_successed(true),
				m_exception(nullptr),
				m_result(p_result.release(), reinterpret_cast< bc_object_allocator::ptr<bc_icontent>::deleter_type >(p_result.get_deleter()))
			{
				static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherite from bc_icontent");
			}

			explicit bc_content_loader_result(bc_io_exception p_exception)
				: m_successed(false),
				m_exception(std::move(p_exception)),
				m_result(nullptr)
			{
			}

			bc_content_loader_result(bc_content_loader_result&& p_other) noexcept
				: m_successed(p_other.m_successed),
				m_exception(std::move(p_other.m_exception)),
				m_result(std::move(p_other.m_result))
			{
			}

			~bc_content_loader_result() = default;

			bc_content_loader_result& operator=(bc_content_loader_result&& p_other) noexcept
			{
				m_successed = p_other.m_successed;
				m_exception = std::move(p_other.m_exception);
				m_result = std::move(p_other.m_result);

				return *this;
			}

			bool successed() const
			{
				return m_successed;
			}

			template< class TContent >
			bc_object_allocator::ptr< TContent > get_result()
			{
				static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherite from bc_icontent");

				if (!m_successed)
				{
					throw m_exception;
				}

				bcAssert(m_result != nullptr);

				return bc_object_allocator::ptr< TContent >
				(
					static_cast< TContent* >(m_result.release()),
					reinterpret_cast< typename bc_object_allocator::ptr< TContent >::deleter_type >(m_result.get_deleter())
				);
			}

		protected:

		private:
			bool m_successed;
			bc_io_exception m_exception;
			bc_object_allocator::ptr<bc_icontent> m_result;
		};
		
		using bc_content_loader_parameter = bc_data_driven_parameter;

		class bc_content_loading_context : public bc_object_allocator
		{
		public:
			bc_content_loading_context() = default;

			bc_content_loading_context(bc_content_loading_context&&) = default;

			~bc_content_loading_context() = default;

			bc_content_loading_context& operator=(bc_content_loading_context&&) = default;

			template
			<
				typename TContent,
				typename = typename std::enable_if< !std::is_same< bc_io_exception, typename std::decay< TContent >::type >::value >::type
			>
			void set_result(TContent&& p_result)
			{
				static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherite from bc_icontent");

				ptr<TContent> l_content_result = allocate<TContent>(std::forward<TContent>(p_result));
				m_result.reset(bc_content_loader_result(std::move(l_content_result)));
			}

			void set_result(bc_io_exception p_exception)
			{
				m_result.reset(bc_content_loader_result(std::move(p_exception)));
			}

			bc_estring_frame m_file_path;						// Used to give access loader to content and offline content file path
			bc_nullable< bc_stream > m_file;					// Used to give access loader to content and offline content file
			bc_content_loader_parameter m_parameter;			// Used to pass additional parameters to loader
			bc_vector_frame<bcBYTE> m_data;						// Used to pass stream data around within loader functions
			bc_nullable< bc_content_loader_result > m_result;	// Used to pass result from loader to caller
		protected:

		private:
		};

		class bc_content_saving_context
		{
		public:
			bc_content_saving_context() = default;

			bc_content_saving_context(bc_content_saving_context&&) = default;

			~bc_content_saving_context() = default;

			bc_content_saving_context& operator=(bc_content_saving_context&&) = default;

			bc_estring_frame m_file_path;						// Used to give access saver to content file path
			bc_nullable< bc_stream > m_file;					// Used to give access saver to content file
			bc_icontent* m_content;								// Used to give access saver to content
		};

		/**
		 * \brief Stateless and threadsafe class.
		 * \non-movale
		 */
		class BC_CORE_DLL bc_icontent_loader
		{
		public:
			virtual ~bc_icontent_loader() = default;

			/**
			 * \brief Will be called when offline content file opened for processing
			 * \param p_context 
			 */
			virtual void content_offline_file_open_successed(bc_content_loading_context& p_context) const = 0;

			/**
			 * \brief Will be called when offline content file doesn't exist and it must be created
			 * \param p_context 
			 */
			virtual void content_offline_processing(bc_content_loading_context& p_context) const = 0;

			/**
			 * \brief Will be called when offline file processing finish and it's time to save offline file
			 * \param p_context 
			 */
			virtual void content_offline_saving(bc_content_loading_context& p_context) const = 0;

			/**
			 * \brief Will be called when content file opened for offline processing
			 * \param p_context 
			 */
			virtual void content_file_open_successed(bc_content_loading_context& p_context) const = 0;

			/**
			 * \brief Will be called when there is a problem with opening content or offline file
			 * \param p_context 
			 */
			virtual void content_file_open_failed(bc_content_loading_context& p_context) const = 0;

			/**
			* \brief Will be called when there is a problem with opening offline file
			* \param p_context
			*/
			virtual void content_file_open_failed(bc_content_saving_context& p_context) const = 0;

			/**
			 * \brief Will be called when offline content file exist and it is valid
			 * \param p_context 
			 */
			virtual void content_processing(bc_content_loading_context& p_context) const = 0;

			/**
			 * \brief Will be called to save content to file
			 * \param p_context 
			 */
			virtual void content_processing(bc_content_saving_context& p_context) const = 0;

			/**
			 * \brief Will be called to get the result
			 * \param p_context 
			 * \return 
			 */
			virtual bc_content_loader_result finish(bc_content_loading_context& p_context) const = 0;

			/**
			 * \brief Will be called when content processing has been finished, or an exception has been thrown during pipeline
			 * \param p_context 
			 */
			virtual void cleanup(bc_content_loading_context& p_context) const = 0;

			/**
			* \brief Will be called when content processing has been finished, or an exception has been thrown during pipeline
			* \param p_context
			*/
			virtual void cleanup(bc_content_saving_context& p_context) const = 0;

		protected:
			bc_icontent_loader() = default;

			bc_icontent_loader(bc_icontent_loader&&) = default;

			bc_icontent_loader& operator=(bc_icontent_loader&&) = default;
		};

		class BC_CORE_DLL bc_base_content_loader : public bc_icontent_loader
		{
		public:
			virtual ~bc_base_content_loader();

			/**
			 * \brief Load file in bc_vector_frame<bcBYTE>
			 * \param p_context 
			 */
			void content_offline_file_open_successed(bc_content_loading_context& p_context) const override;

			/**
			 * \brief Write bc_vector_frame<bcBYTE> to file
			 * \param p_context 
			 */
			void content_offline_saving(bc_content_loading_context& p_context) const override;

			/**
			 * \brief Load file in bc_vector_frame<bcBYTE>
			 * \param p_context 
			 */
			void content_file_open_successed(bc_content_loading_context& p_context) const override;

			/**
			 * \brief Throw exception
			 * \param p_context 
			 */
			void content_file_open_failed(bc_content_loading_context& p_context) const override;

			/**
			* \brief Throw exception
			* \param p_context
			*/
			void content_file_open_failed(bc_content_saving_context& p_context) const override;

			/**
			* \brief Throw exception
			* \param p_context
			*/
			void content_processing(bc_content_saving_context& p_context) const override;

			bc_content_loader_result finish(bc_content_loading_context& p_context) const override;

			void cleanup(bc_content_loading_context& p_context) const override;

			void cleanup(bc_content_saving_context& p_context) const override;

		protected:
			bc_base_content_loader();

			bc_base_content_loader(bc_base_content_loader&& p_other) noexcept;

			bc_base_content_loader& operator=(bc_base_content_loader&& p_other) noexcept;
			
		private:
		};

		template< class TContent >
		class _bc_content_loader_gaurd
		{
		public:
			_bc_content_loader_gaurd(bc_icontent_loader& p_loader, bc_content_loading_context& p_context)
				: m_loader(p_loader),
				m_loading_context(&p_context),
				m_saving_context(nullptr)
			{
			}

			_bc_content_loader_gaurd(bc_icontent_loader& p_loader, bc_content_saving_context& p_context)
				: m_loader(p_loader),
				m_loading_context(nullptr),
				m_saving_context(&p_context)
			{
			}

			~_bc_content_loader_gaurd()
			{
				if(m_loading_context)
				{
					m_loader.cleanup(*m_loading_context);
				}
				else
				{
					m_loader.cleanup(*m_saving_context);
				}
			}

		protected:

		private:
			bc_icontent_loader& m_loader;
			bc_content_loading_context* m_loading_context;
			bc_content_saving_context* m_saving_context;
		};
	}
}