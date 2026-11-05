// Declara a máquina de estados responsável pelo controle da válvula V2

#ifndef SM3_H
#define SM3_H

#include "sm.h"

extern StateMachine sm3;

STATE(sm3_init);
STATE(sm3_v2_fechada);
STATE(sm3_v2_aberta);
STATE(sm3_err);

const char *sm3_state_name();

#endif
