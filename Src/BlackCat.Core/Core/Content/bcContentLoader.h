// [04/08/2016 MRB]

#pragma once

#include "Core/bcExport.h"
#include "Core/bcException.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Utility/bcDataDrivenParameter.h"
#include "Core/Utility/bcNullable.h"
#include "Core/File/bcStream.h"
#include "Core/Content/bcContent.h"

namespace black_cat
{
	namespace core
	{
		using bc_content_loader_parameter = bc_data_driven_parameter;
		
		class bc_content_loader_result
		{
		public:
			template< class TContent >
			explicit bc_content_loader_result(bc_unique_ptr<TContent>&& p_result)
				: m_succeeded(true),
				m_exception(nullptr),
				m_result(std::move(p_result))
			{
				static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherit from bc_icontent");
			}

			explicit bc_content_loader_result(bc_io_exception p_exception)
				: m_succeeded(false),
				m_exception(std::move(p_exception)),
				m_result(nullptr)
			{
			}

			bc_content_loader_result(bc_content_loader_result&& p_other) noexcept
				: m_succeeded(p_other.m_succeeded),
				m_exception(std::move(p_other.m_exception)),
				m_result(std::move(p_other.m_result))
			{
			}

			~bc_content_loader_result() = default;

			bc_content_loader_result& operator=(bc_content_loader_result&& p_other) noexcept
			{
				m_succeeded = p_other.m_succeeded;
				m_exception = std::move(p_other.m_exception);
				m_result = std::move(p_other.m_result);

				return *this;
			}

			bool succeeded() const
			{
				return m_succeeded;
			}

			template< class TContent >
			TContent* get_result()
			{
				static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherit from bc_icontent");
				
				if (!m_succeeded)
				{
					throw m_exception;
				}

				bcAssert(m_result != nullptr);
				
				return static_cast<TContent*>(m_result.get());
			}
			
			template< class TContent >
			bc_unique_ptr< TContent > release_result()
			{
				static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherit from bc_icontent");

				if (!m_succeeded)
				{
					throw m_exception;
				}

				bcAssert(m_result != nullptr);
				
				return static_pointer_cast<TContent>(m_result);
			}

		private:
			bool m_succeeded;
			bc_io_exception m_exception;
			bc_unique_ptr<bc_icontent> m_result;
		};

		class bc_content_loading_context : public bc_object_allocator
		{
		public:
			bc_content_loading_context()
				: m_file_path(nullptr),
				m_file(),
				m_file_buffer(nullptr),
				m_file_buffer_size(0),
				m_parameters(nullptr),
				m_instance_parameters(bc_alloc_type::frame),
				m_result(nullptr)

			{
			}

			bc_content_loading_context(bc_content_loading_context&&) noexcept = default;

			~bc_content_loading_context() = default;

			bc_content_loading_context& operator=(bc_content_loading_context&&) noexcept = default;

			template
			<
				typename TContent,
				typename = std::enable_if_t< !std::is_same_v< std::decay_t< TContent >, bc_io_exception > >
			>
			void set_result(TContent&& p_result)
			{
				static_assert(std::is_base_of< bc_icontent, TContent >::value, "Content must inherit from bc_icontent");

				bc_unique_ptr<TContent> l_content_result = allocate<TContent>(std::forward<TContent>(p_result));
				m_result.reset(bc_content_loader_result(std::move(l_content_result)));
			}

			void set_result(bc_io_exception p_exception)
			{
				m_result.reset(bc_content_loader_result(std::move(p_exception)));
			}
			
			const bcECHAR* m_file_path;							// Used to give loader access to content and offline content file path
			bc_stream m_file;									// Used to give loader access to content and offline content file
			bc_unique_ptr<bcBYTE> m_file_buffer;				// Used to give loader access to file content
			bcSIZE m_file_buffer_size;							// Used to give loader access to file content size
			const bc_content_loader_parameter* m_parameters;	// Used to pass constant parameters to loader
			bc_content_loader_parameter m_instance_parameters;	// Used to pass instance parameters to loader
			bc_nullable< bc_content_loader_result > m_result;	// Used to pass result from loader to caller
		};

		class bc_content_saving_context
		{
		public:
			bc_content_saving_context() = default;

			bc_content_saving_context(bc_content_saving_context&&) noexcept = default;

			~bc_content_saving_context() = default;

			bc_content_saving_context& operator=(bc_content_saving_context&&) noexcept = default;

			const bcECHAR* m_file_path;							// Used to give saver access to content file path
			bc_stream m_file;									// Used to give saver access to content file
			bc_icontent* m_content;								// Used to give saver access to content
		};

		/**
		 * \brief Stateless and thread-safe class.
		 * \n
		 * \b non-movable
		 */
		class BC_CORE_DLL bc_icontent_loader
		{
		public:
			virtual ~bc_icontent_loader() = default;

			virtual bool support_offline_processing() const = 0;

			/**
			 * \brief Will be called when offline content file opened for processing
			 * \param p_context 
			 */
			virtual void content_offline_file_open_succeeded(bc_content_loading_context& p_context) const = 0;

			/**
			 * \brief Will be called when offline content file does't exist and it must be created
			 * \param p_context 
			 */
			virtual void content_offline_processing(bc_content_loading_context& p_context) const = 0;

			/**
			 * \brief Will be called when content file opened for offline processing
			 * \param p_context 
			 */
			virtual void content_file_open_succeeded(bc_content_loading_context& p_context) const = 0;

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
			 * \brief Read file into specified buffer
			 * \param p_context 
			 */
			void content_offline_file_open_succeeded(bc_content_loading_context& p_context) const override;

			/**
			* \brief Throw exception
			* \param p_context
			*/
			void content_offline_processing(bc_content_loading_context& p_context) const override;

			/**
			 * \brief Read file into specified buffer
			 * \param p_context 
			 */
			void content_file_open_succeeded(bc_content_loading_context& p_context) const override;

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
		};

		template< class TContent >
		class _bc_content_loader_guard
		{
		public:
			_bc_content_loader_guard(bc_icontent_loader& p_loader, bc_content_loading_context& p_context)
				: m_loader(p_loader),
				m_loading_context(&p_context),
				m_saving_context(nullptr)
			{
			}

			_bc_content_loader_guard(bc_icontent_loader& p_loader, bc_content_saving_context& p_context)
				: m_loader(p_loader),
				m_loading_context(nullptr),
				m_saving_context(&p_context)
			{
			}

			~_bc_content_loader_guard()
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

		private:
			bc_icontent_loader& m_loader;
			bc_content_loading_context* m_loading_context;
			bc_content_saving_context* m_saving_context;
		};
	}
}