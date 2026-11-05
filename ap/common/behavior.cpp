// Coordena a inicialização e a execução das máquinas de estados do sistema

#include "behavior.h"
#include "hal.h"

// Indica que alguma máquina entrou em erro e o sistema inteiro foi travado.
static bool falha_geral = false;

// Inicializa o hardware e as máquinas de estados.
void behavior_init(){
    init_hal();

    INIT(sm1, sm1_init);
    INIT(sm2, sm2_init);
    INIT(sm3, sm3_init);
    INIT(sm4, sm4_init);
}

// Executa um ciclo de cada máquina de estados.
void behavior_step(){
    // Se já houve falha, não executa mais nada: o sistema fica parado até desligar a placa.
    if(falha_geral){
        return;
    }

    EXEC(sm1);
    EXEC(sm2);
    EXEC(sm3);
    EXEC(sm4);

    // Se qualquer máquina entrou em erro, desliga todos os atuadores e trava tudo.
    if(sm1.ptr == sm1_err || sm2.ptr == sm2_err || sm3.ptr == sm3_err || sm4.ptr == sm4_err){
        set_v1(false);
        set_v2(false);
        set_b1(false);
        set_r1(false);
        falha_geral = true;
    }
}

// Mantém a execução contínua do comportamento do sistema.
void behavior(){
    behavior_init();
    while(1){
        behavior_step();
    }
}
