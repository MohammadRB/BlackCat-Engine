// [06/07/2021 MRB]

#include "CoreUnitTest/CoreUnitTestPCH.h"
#include "Core/File/bcMemoryStream.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace black_cat
{
	namespace core_unit_test
	{
		TEST_CLASS(bc_memory_stream)
		{
		public:
			TEST_METHOD(overal_test)
			{
				core::bc_memory_manager::startup(1, 16, 10, 16, 16, 1024, 1024, 1024);
				
				{
					core::bc_memory_stream l_stream;

					Assert::IsTrue(l_stream.get_eof_position() == 0);
					Assert::IsTrue(l_stream.get_position() == 0);

					bcBYTE l_read_buffer[100];
					auto l_bytes_read = l_stream.read(&l_read_buffer[0], 100);

					Assert::IsTrue(l_bytes_read == 0);

					auto l_bytes_write = l_stream.write(&l_read_buffer[0], 10);

					Assert::IsTrue(l_bytes_write == 10);
					Assert::IsTrue(l_stream.get_position() == 10);

					l_stream.set_position(core::bc_stream_seek::start, 5);
					l_bytes_read = l_stream.read(&l_read_buffer[0], 100);

					Assert::IsTrue(l_bytes_read == 5);

					l_bytes_write = l_stream.write(&l_read_buffer[0], 100);

					Assert::IsTrue(l_bytes_write == 100);
					Assert::IsTrue(l_stream.get_position() == 105);

					l_stream.set_position(core::bc_stream_seek::current, 100);
					l_bytes_write = l_stream.write(&l_read_buffer[0], 10);

					Assert::IsTrue(l_bytes_write == 10);
					Assert::IsTrue(l_stream.get_position() == 215);
				}

				core::bc_memory_manager::close();
			}
		};
	}
}