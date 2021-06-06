// [06/04/2021 MRB]

#include "CoreUnitTest/CoreUnitTestPCH.h"
#include "Core/Container/bcStateMachine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace black_cat
{
	namespace core_unit_test
	{
		class bc_open_state;
		class bc_close_state;
		class bc_door_state_machine;

		class bc_open_event
		{
		};

		class bc_close_event
		{
		};

		class bc_assert_event
		{
		public:
			enum class state
			{
				open, close
			} m_state;
		};
		
		class bc_open_state : public core::bc_state<bc_door_state_machine, bc_close_event, bc_assert_event>
		{
		private:
			state_transition handle(const bc_close_event& p_event) override
			{
				return state_transition::transfer_to<bc_close_state>();
			}

			state_transition handle(const bc_assert_event& p_event) override
			{
				Assert::IsTrue(p_event.m_state == bc_assert_event::state::open);
				return state_transition::empty();
			}
		};

		class bc_close_state : public core::bc_state<bc_door_state_machine, bc_open_event, bc_assert_event>
		{
		private:
			state_transition handle(const bc_open_event& p_event) override
			{
				return state_transition::transfer_to<bc_open_state>();
			}

			state_transition handle(const bc_assert_event& p_event) override
			{
				Assert::IsTrue(p_event.m_state == bc_assert_event::state::close);
				return state_transition::empty();
			}
		};

		class bc_door_state_machine : public core::bc_state_machine<bc_open_state, bc_close_state>
		{
		public:
			bc_door_state_machine()
				: bc_state_machine
				(
					bc_open_state(), 
					bc_close_state()
				)
			{
			}
		};
		
		TEST_CLASS(bc_state_machine_test)
		{
		public:
			TEST_METHOD(overal_test)
			{
				constexpr auto l = sizeof(bc_open_state);
				
				bc_door_state_machine l_machine;
				l_machine.transfer_state<bc_open_state>();

				l_machine.process_event(bc_assert_event{ bc_assert_event::state::open });
				l_machine.process_event(bc_open_event());
				l_machine.process_event(bc_assert_event{ bc_assert_event::state::open });
				l_machine.process_event(bc_close_event());
				l_machine.process_event(bc_assert_event{ bc_assert_event::state::close });
				l_machine.process_event(bc_open_event());
				l_machine.process_event(bc_assert_event{ bc_assert_event::state::open });
			}
		};
	}
}