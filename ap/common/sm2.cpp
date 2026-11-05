// Implementa a máquina de estados responsável pelo controle da bomba B1

#include "sm2.h"
#include "hal.h"
#include "definitions.h"
#include "chronometer.h"

StateMachine sm2;
static Chronometer sm2_chronometer;

// Verifica se os sensores da bomba B1 estão em uma condição incoerente
static bool sm2_sensores_incoerentes(){
    return get_s22() && !get_s21();
}

// Inicializa a máquina de estados e define o estado da bomba B1
STATE(sm2_init){
    if(ENTRY){
        set_b1(false);
        set_led_sm2_err(false);
    }

    if(sm2_sensores_incoerentes()){
        NEXT_STATE(sm2_err);
    } else if(get_s22()){
        NEXT_STATE(sm2_b1_desligada);
    } else if(get_s11()){
        NEXT_STATE(sm2_b1_ligada);
    } else {
        NEXT_STATE(sm2_aguardando_agua);
    }
}

// Mantém a bomba B1 desligada durante o tempo definido
STATE(sm2_b1_desligada){
    if(ENTRY){
        set_b1(false);
        set_led_sm2_err(false);
        sm2_chronometer.start(TEMPO_B1_DESLIGADA);
    }

    if(sm2_sensores_incoerentes()){
        NEXT_STATE(sm2_err);
    } else if(!get_s22()){
        if(!get_s21() || sm2_chronometer.is_finished()){
            if(get_s11()){
                NEXT_STATE(sm2_b1_ligada);
            } else {
                NEXT_STATE(sm2_aguardando_agua);
            }
        }
    }
}

// Mantém a bomba B1 ligada enquanto houver água disponível
STATE(sm2_b1_ligada){
    if(ENTRY){
        set_b1(true);
        set_led_sm2_err(false);
    }

    if(sm2_sensores_incoerentes()){
        NEXT_STATE(sm2_err);
    } else if(get_s22()){
        NEXT_STATE(sm2_b1_desligada);
    } else if(!get_s11()){
        NEXT_STATE(sm2_aguardando_agua);
    }
}

// Aguarda a presença de água antes de ligar a bomba B1
STATE(sm2_aguardando_agua){
    if(ENTRY){
        set_b1(false);
        set_led_sm2_err(false);
        sm2_chronometer.start(TEMPO_B1_AGUARDA_AGUA);
    }

    if(sm2_sensores_incoerentes()){
        NEXT_STATE(sm2_err);
    } else if(get_s22()){
        NEXT_STATE(sm2_b1_desligada);
    } else if(!get_s11()){
        sm2_chronometer.start(TEMPO_B1_AGUARDA_AGUA);
    } else if(sm2_chronometer.is_finished()){
        NEXT_STATE(sm2_b1_ligada);
    }
}

// Desliga a bomba B1 e aguarda o comando de reset
STATE(sm2_err){
    if(ENTRY){
        set_b1(false);
        set_led_sm2_err(true);
    }

    if(get_btn_reset() && !sm2_sensores_incoerentes()){
        NEXT_STATE(sm2_init);
    }
}

// Retorna o nome do estado atual da máquina de estados
const char *sm2_state_name(){
    pStateFunc s = sm2.ptr;
    if(s == sm2_init)             return "sm2_init";
    if(s == sm2_b1_desligada)     return "sm2_b1_desligada";
    if(s == sm2_b1_ligada)        return "sm2_b1_ligada";
    if(s == sm2_aguardando_agua)  return "sm2_aguardando_agua";
    if(s == sm2_err)              return "sm2_err";
    return "?";
}
