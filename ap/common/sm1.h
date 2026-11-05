// Declara a máquina de estados responsável pelo controle da válvula V1

#ifndef SM1_H
#define SM1_H

#include "sm.h"

extern StateMachine sm1;

STATE(sm1_init);
STATE(sm1_v1_aberta);
STATE(sm1_v1_fechada);
STATE(sm1_err);

const char *sm1_state_name();

#endif
