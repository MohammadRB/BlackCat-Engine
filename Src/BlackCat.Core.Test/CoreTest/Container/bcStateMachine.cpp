// [04/06/2021 MRB]

#include "CoreTest/CoreTestPCH.h"
#include "Core/Container/bcStateMachine.h"

namespace black_cat::core_test
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
		state_transition handle(bc_close_event& p_event) override
		{
			return state_transition::transfer_to<bc_close_state>();
		}

		state_transition handle(bc_assert_event& p_event) override
		{
			EXPECT_TRUE(p_event.m_state == bc_assert_event::state::open);
			return state_transition::empty();
		}
	};

	class bc_close_state : public core::bc_state<bc_door_state_machine, bc_open_event, bc_assert_event>
	{
	private:
		state_transition handle(bc_open_event& p_event) override
		{
			return state_transition::transfer_to<bc_open_state>();
		}

		state_transition handle(bc_assert_event& p_event) override
		{
			EXPECT_TRUE(p_event.m_state == bc_assert_event::state::close);
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
		
	TEST(StateMachine, OverlTest)
	{
		bc_door_state_machine l_machine;
		l_machine.transfer_state<bc_open_state>();

		bc_open_event l_open_event{ };
		bc_close_event l_close_event{ };
		bc_assert_event l_open_assert_event{ bc_assert_event::state::open };
		bc_assert_event l_close_assert_event{ bc_assert_event::state::close };
			
		l_machine.process_event(l_open_assert_event);
		l_machine.process_event(l_open_event);
		l_machine.process_event(l_open_assert_event);
		l_machine.process_event(l_close_event);
		l_machine.process_event(l_close_assert_event);
		l_machine.process_event(l_open_event);
		l_machine.process_event(l_open_assert_event);
	}
}
