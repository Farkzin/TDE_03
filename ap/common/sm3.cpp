// Implementa a máquina de estados responsável pelo controle da válvula V2

#include "sm3.h"
#include "hal.h"

StateMachine sm3;

// Verifica se os sensores do tanque 3 estão em uma condição incoerente
static bool sm3_sensores_incoerentes(){
    return get_s32() && !get_s31();
}

// Verifica se há água no tanque 2 e espaço disponível no tanque 3 para abrir a válvula V2
static bool sm3_deve_abrir(){
    return !get_s32() && get_s21();
}

// Desliga a válvula V2 e define o estado inicial conforme os níveis dos tanques 2 e 3
STATE(sm3_init){
    if(ENTRY){
        set_v2(false);
        set_led_sm3_err(false);
    }

    if(sm3_sensores_incoerentes()){
        NEXT_STATE(sm3_err);
    } else if(sm3_deve_abrir()){
        NEXT_STATE(sm3_v2_aberta);
    } else {
        NEXT_STATE(sm3_v2_fechada);
    }
}

// Mantém a válvula V2 fechada até atender à condição de abertura
STATE(sm3_v2_fechada){
    if(ENTRY){
        set_v2(false);
        set_led_sm3_err(false);
    }

    if(sm3_sensores_incoerentes()){
        NEXT_STATE(sm3_err);
    } else if(sm3_deve_abrir()){
        NEXT_STATE(sm3_v2_aberta);
    }
}

// Mantém a válvula V2 aberta enquanto as condições forem atendidas
STATE(sm3_v2_aberta){
    if(ENTRY){
        set_v2(true);
        set_led_sm3_err(false);
    }

    if(sm3_sensores_incoerentes()){
        NEXT_STATE(sm3_err);
    } else if(get_s32() || !get_s21()){ // Fecha a válvula quando o tanque 3 está cheio ou o tanque 2 está vazio
        NEXT_STATE(sm3_v2_fechada);
    }
}

// Desliga a válvula V2 e aguarda o comando de reset
STATE(sm3_err){
    if(ENTRY){
        set_v2(false);
        set_led_sm3_err(true);
    }

    if(get_btn_reset() && !sm3_sensores_incoerentes()){
        NEXT_STATE(sm3_init);
    }
}

// Retorna o nome do estado atual da máquina de estados
const char *sm3_state_name(){
    pStateFunc s = sm3.ptr;
    if(s == sm3_init)        return "sm3_init";
    if(s == sm3_v2_fechada)  return "sm3_v2_fechada";
    if(s == sm3_v2_aberta)   return "sm3_v2_aberta";
    if(s == sm3_err)         return "sm3_err";
    return "?";
}
