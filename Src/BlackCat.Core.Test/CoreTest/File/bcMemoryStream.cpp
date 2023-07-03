// [07/06/2021 MRB]

#include "CoreTest/CoreTestPCH.h"

#include "Core/File/bcMemoryStream.h"
#include "CoreTest/bcTestFixture.h"

namespace black_cat
{
	namespace core_test
	{
		TEST(MemoryStream, OveralTest)
		{
			bc_test_init();

			{
				core::bc_memory_stream l_stream;

				EXPECT_TRUE(l_stream.get_eof_position() == 0);
				EXPECT_TRUE(l_stream.get_position() == 0);

				bcBYTE l_read_buffer[100];
				auto l_bytes_read = l_stream.read(&l_read_buffer[0], 100);

				EXPECT_TRUE(l_bytes_read == 0);

				auto l_bytes_write = l_stream.write(&l_read_buffer[0], 10);

				EXPECT_TRUE(l_bytes_write == 10);
				EXPECT_TRUE(l_stream.get_position() == 10);

				l_stream.set_position(core::bc_stream_seek::start, 5);
				l_bytes_read = l_stream.read(&l_read_buffer[0], 100);

				EXPECT_TRUE(l_bytes_read == 5);

				l_bytes_write = l_stream.write(&l_read_buffer[0], 100);

				EXPECT_TRUE(l_bytes_write == 100);
				EXPECT_TRUE(l_stream.get_position() == 105);

				l_stream.set_position(core::bc_stream_seek::current, 100);
				l_bytes_write = l_stream.write(&l_read_buffer[0], 10);

				EXPECT_TRUE(l_bytes_write == 10);
				EXPECT_TRUE(l_stream.get_position() == 215);
			}

			bc_test_close();
		}
	}
}