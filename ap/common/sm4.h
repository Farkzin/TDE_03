// Declara a máquina de estados responsável pelo controle do aquecedor R1

#ifndef SM4_H
#define SM4_H

#include "sm.h"

extern StateMachine sm4;

STATE(sm4_init);
STATE(sm4_r1_desligada);
STATE(sm4_r1_ligada);
STATE(sm4_err);

const char *sm4_state_name();

#endif
