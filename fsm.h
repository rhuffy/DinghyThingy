#ifndef __FSM_H
#define __FSM_H

typedef enum {
	STATE_ROOT,
  STATE_READY,
  STATE_SENSE,
  STATE_WRITEFLASH,
	STATE_UPLOAD
} STATE_T;

void advance_state();
void init_state();
void set_state(STATE_T new_state);
void enter_state_root();
void update_state_root();

void enter_state_ready();
void update_state_ready();

void enter_state_sense();
void update_state_sense();

void enter_state_writeflash();
void update_state_writeflash();

void enter_state_upload();
void update_state_upload();


#endif // __FSM_H
