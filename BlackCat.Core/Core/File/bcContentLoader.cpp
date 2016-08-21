// [04/11/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/File/bcContentLoader.h"

namespace black_cat
{
	namespace core
	{
		// -- Base content loader --------------------------------------------------------------------------------
		bc_base_content_loader::bc_base_content_loader()
		{
		}

		bc_base_content_loader::bc_base_content_loader(bc_base_content_loader&& p_other)
			: bc_icontent_loader(std::move(p_other))
		{
		}

		bc_base_content_loader::~bc_base_content_loader()
		{
		}

		bc_base_content_loader& bc_base_content_loader::operator=(bc_base_content_loader&& p_other)
		{
			bc_icontent_loader::operator=(std::move(p_other));

			return *this;
		}

		void bc_base_content_loader::content_file_open_successed(bc_content_loader_context& p_context)
		{
			auto l_file_size = p_context.m_file->length();

			p_context.m_data = bc_vector_frame<bcBYTE>(l_file_size); // Use this method of initialization to change size of vector

			p_context.m_file->read(p_context.m_data.data(), l_file_size);
		}

		void bc_base_content_loader::content_file_open_failed(bc_content_loader_context& p_context)
		{
			throw bc_io_exception("Cann't open content file");
		}

		void bc_base_content_loader::content_offline_saving(bc_content_loader_context& p_context)
		{
			p_context.m_file->write(p_context.m_data.data(), p_context.m_data.size());
		}

		void bc_base_content_loader::content_offline_file_open_successed(bc_content_loader_context& p_context)
		{
			auto l_file_size = p_context.m_file->length();

			p_context.m_data = bc_vector_frame<bcBYTE>(l_file_size);

			p_context.m_file->read(p_context.m_data.data(), l_file_size);
		}

		bc_content_loader_result bc_base_content_loader::finish(bc_content_loader_context& p_context)
		{
			if (p_context.m_result != nullptr)
			{
				auto l_result = std::move(p_context.m_result.get());
				p_context.m_result.reset();

				return l_result;
			}

			return bc_content_loader_result(bc_io_exception("Error in loading content"));
		}

		void bc_base_content_loader::cleanup(bc_content_loader_context& p_context)
		{
			p_context.m_file.reset();
			p_context.m_parameter.reset();
			p_context.m_data.clear();
			p_context.m_data.shrink_to_fit();
			p_context.m_result.reset();
		}
	}
}