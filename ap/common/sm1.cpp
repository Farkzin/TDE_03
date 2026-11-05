// Implementa a máquina de estados responsável pelo controle da válvula V1

#include "sm1.h"
#include "hal.h"
#include "definitions.h"
#include "chronometer.h"

StateMachine sm1;
static Chronometer sm1_chronometer;

// Verifica se os sensores da válvula V1 estão em uma condição incoerente
static bool sm1_sensores_incoerentes(){
    return get_s12() && !get_s11();
}

// Inicializa a máquina de estados e define o estado da válvula V1
STATE(sm1_init){
    if(ENTRY){
        set_v1(false);
        set_led_sm1_err(false);
    }

    if(sm1_sensores_incoerentes()){
        NEXT_STATE(sm1_err);
    } else if(get_s12()){
        NEXT_STATE(sm1_v1_fechada);
    } else {
        NEXT_STATE(sm1_v1_aberta);
    }
}

// Mantém a válvula V1 aberta até detectar seu fechamento
STATE(sm1_v1_aberta){
    if(ENTRY){
        set_v1(true);
        set_led_sm1_err(false);
    }

    if(sm1_sensores_incoerentes()){
        NEXT_STATE(sm1_err);
    } else if(get_s12()){
        NEXT_STATE(sm1_v1_fechada);
    }
}

// Mantém a válvula V1 fechada durante o tempo definido
STATE(sm1_v1_fechada){
    if(ENTRY){
        set_v1(false);
        set_led_sm1_err(false);
        sm1_chronometer.start(TEMPO_V1_FECHADA);
    }

    if(sm1_sensores_incoerentes()){
        NEXT_STATE(sm1_err);
    } else if(!get_s12() && sm1_chronometer.is_finished()){
        NEXT_STATE(sm1_v1_aberta);
    }
}

// Desliga a válvula V1 e aguarda o comando de reset
STATE(sm1_err){
    if(ENTRY){
        set_v1(false);
        set_led_sm1_err(true);
    }

    if(get_btn_reset() && !sm1_sensores_incoerentes()){
        NEXT_STATE(sm1_init);
    }
}

// Retorna o nome do estado atual da máquina de estados
const char *sm1_state_name(){
    pStateFunc s = sm1.ptr;
    if(s == sm1_init)        return "sm1_init";
    if(s == sm1_v1_aberta)   return "sm1_v1_aberta";
    if(s == sm1_v1_fechada)  return "sm1_v1_fechada";
    if(s == sm1_err)         return "sm1_err";
    return "?";
}
