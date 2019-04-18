# DinghyThingy

## FSM Structure

advance_state() is called once every cycle, and in turn calls the correct
update_state_XXX() method based on the current state.

To enter a new state, use set_state(STATE_T new_state).
Provide your desired new state and the associated enter_state_XXX() method
will be called.
