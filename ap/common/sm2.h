// Declara a máquina de estados responsável pelo controle da bomba B1

#ifndef SM2_H
#define SM2_H

#include "sm.h"

extern StateMachine sm2;

STATE(sm2_init);
STATE(sm2_b1_desligada);
STATE(sm2_b1_ligada);
STATE(sm2_aguardando_agua);
STATE(sm2_err);

const char *sm2_state_name();

#endif
