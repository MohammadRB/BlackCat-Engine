// [04/11/2021 MRB]

#include "CoreTest/CoreTestPCH.h"

#include "Core/Utility/bcParameterPack.h"
#include "CoreTest/bcTestFixture.h"

namespace black_cat
{
	namespace core_test
	{
		TEST(ParameterPack, LeakTest)
		{
			bc_test_init_memory();

			{
				core::bc_any l_any1;
				core::bc_any l_any2;

				l_any1.set_value(core::bc_string("10"));
				l_any2 = std::move(l_any1);

				EXPECT_FALSE(l_any1.has_value());
				EXPECT_TRUE(l_any2.has_value());
			}

#ifdef BC_DEBUG
			EXPECT_EQ(core::bc_memory_manager::get().report_memory_leaks(), 0);
#endif
			
			bc_test_close_memory();
		}
	}
}