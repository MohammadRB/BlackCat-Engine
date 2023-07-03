// [01/10/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/File/bcFileStream.h"
#include "Core/Utility/bcLogger.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_file_logger : public core::bci_log_listener
		{
		public:
			bc_file_logger(const bcECHAR* p_path, const bcECHAR* p_app_name);

			bc_file_logger(bc_file_logger&&) noexcept;

			~bc_file_logger() override;

			bc_file_logger& operator=(bc_file_logger&&) noexcept;
			
			bool is_file() override;
			
			void on_log(core::bc_log_type p_type, const bcECHAR* p_log) override;

		private:
			core::bc_file_stream m_file;
			platform::bc_atomic<bcSIZE> m_buffer_length;
		};
	}
}