// Coordena a inicialização e a execução das máquinas de estados do sistema

#include "behavior.h"
#include "hal.h"

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
    EXEC(sm1);
    EXEC(sm2);
    EXEC(sm3);
    EXEC(sm4);
}

// Mantém a execução contínua do comportamento do sistema.
void behavior(){
    behavior_init();
    while(1){
        behavior_step();
    }
}
