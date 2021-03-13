// [04/11/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/Content/bcContentLoader.h"

namespace black_cat
{
	namespace core
	{
		bc_base_content_loader::bc_base_content_loader() = default;

		bc_base_content_loader::bc_base_content_loader(bc_base_content_loader&& p_other) noexcept
			: bci_content_loader(std::move(p_other))
		{
		}

		bc_base_content_loader::~bc_base_content_loader() = default;

		bc_base_content_loader& bc_base_content_loader::operator=(bc_base_content_loader&& p_other) noexcept
		{
			bci_content_loader::operator=(std::move(p_other));

			return *this;
		}

		void bc_base_content_loader::content_offline_file_open_succeeded(bc_content_loading_context& p_context) const
		{
			p_context.m_file_buffer_size = p_context.m_file.length();
			p_context.m_file_buffer.reset(static_cast<bcBYTE*>(BC_ALLOC(p_context.m_file_buffer_size, bc_alloc_type::frame)));

			p_context.m_file.read(p_context.m_file_buffer.get(), p_context.m_file_buffer_size);
		}

		void bc_base_content_loader::content_offline_processing(bc_content_loading_context& p_context) const
		{
			const auto l_file_name = bc_to_string_frame(p_context.m_file_path);
			const auto l_error_msg = bc_string_frame("Content offline processing is not supported: ") + l_file_name;

			throw bc_io_exception(l_error_msg.c_str());
		}

		void bc_base_content_loader::content_file_open_succeeded(bc_content_loading_context& p_context) const
		{
			p_context.m_file_buffer_size = p_context.m_file.length();
			p_context.m_file_buffer.reset(static_cast<bcBYTE*>(BC_ALLOC(p_context.m_file_buffer_size, bc_alloc_type::frame)));

			p_context.m_file.read(p_context.m_file_buffer.get(), p_context.m_file_buffer_size);
		}

		void bc_base_content_loader::content_file_open_failed(bc_content_loading_context& p_context) const
		{
			const auto l_file_name = bc_to_string_frame(p_context.m_file_path);
			const auto l_error_msg = bc_string_frame("Cannot open content file: ") + l_file_name;

			throw bc_io_exception(l_error_msg.c_str());
		}

		void bc_base_content_loader::content_file_open_failed(bc_content_saving_context& p_context) const
		{
			const auto l_file_name = bc_to_string_frame(p_context.m_file_path);
			const auto l_error_msg = bc_string_frame("Cannot open content file: ") + l_file_name;

			throw bc_io_exception(l_error_msg.c_str());
		}

		void bc_base_content_loader::content_processing(bc_content_saving_context& p_context) const
		{
			const auto l_file_name = bc_to_string_frame(p_context.m_file_path);
			const auto l_error_msg = bc_string_frame("Content saving is not supported: ") + l_file_name;

			throw bc_io_exception(l_error_msg.c_str());
		}

		bc_content_loader_result bc_base_content_loader::finish(bc_content_loading_context& p_context) const
		{
			if (p_context.m_result != nullptr)
			{
				auto l_result = std::move(p_context.m_result.get());
				p_context.m_result.reset();

				return l_result;
			}

			throw bc_io_exception("Content load result is not set");
		}

		void bc_base_content_loader::cleanup(bc_content_loading_context& p_context) const
		{
			if(p_context.m_file.is_open())
			{
				p_context.m_file.close();
			}

			p_context.m_file_buffer.reset();
			p_context.m_instance_parameters.reset();
			p_context.m_result.reset();
		}

		void bc_base_content_loader::cleanup(bc_content_saving_context& p_context) const
		{
			if (p_context.m_file.is_open())
			{
				p_context.m_file.close();
			}
		}
	}
}