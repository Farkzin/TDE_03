// Implementa a máquina de estados responsável pelo controle do aquecedor R1

#include "sm4.h"
#include "hal.h"
#include "definitions.h"

StateMachine sm4;

// Verifica se a temperatura medida está fora dos limites válidos
static bool sm4_temperatura_invalida(){
    float t = get_st1();
    return t < TEMP_MIN_VALIDA || t > TEMP_MAX_VALIDA;
}

// Verifica se há água no tanque 3 e se a temperatura está baixa o suficiente para ligar o aquecedor
static bool sm4_deve_ligar(){
    return get_s31() && get_st1() <= TEMP_LIGA;
}

// Desliga o aquecedor e define o estado inicial conforme a temperatura e o nível do tanque
STATE(sm4_init){
    if(ENTRY){
        set_r1(false);
        set_led_sm4_err(false);
    }

    if(sm4_temperatura_invalida()){
        NEXT_STATE(sm4_err);
    } else if(sm4_deve_ligar()){
        NEXT_STATE(sm4_r1_ligada);
    } else {
        NEXT_STATE(sm4_r1_desligada);
    }
}

// Mantém o aquecedor desligado até que as condições de acionamento sejam atendidas
STATE(sm4_r1_desligada){
    if(ENTRY){
        set_r1(false);
        set_led_sm4_err(false);
    }

    if(sm4_temperatura_invalida()){
        NEXT_STATE(sm4_err);
    } else if(sm4_deve_ligar()){
        NEXT_STATE(sm4_r1_ligada);
    }
}

// Mantém o aquecedor ligado até atingir a temperatura de desligamento
STATE(sm4_r1_ligada){
    if(ENTRY){
        set_r1(true);
        set_led_sm4_err(false);
    }

    if(sm4_temperatura_invalida()){
        NEXT_STATE(sm4_err);
    } else if(!get_s31()){ // Desliga se não houver água no tanque 3
        NEXT_STATE(sm4_r1_desligada);
    } else if(get_st1() >= TEMP_DESLIGA){ // Desliga ao atingir a temperatura de desligamento
        NEXT_STATE(sm4_r1_desligada);
    }
}

// Desliga o aquecedor e aguarda o comando de reset
STATE(sm4_err){
    if(ENTRY){
        set_r1(false);
        set_led_sm4_err(true);
    }

    if(get_btn_reset() && !sm4_temperatura_invalida()){
        NEXT_STATE(sm4_init);
    }
}

// Retorna o nome do estado atual da máquina de estados
const char *sm4_state_name(){
    pStateFunc s = sm4.ptr;
    if(s == sm4_init)          return "sm4_init";
    if(s == sm4_r1_desligada)  return "sm4_r1_desligada";
    if(s == sm4_r1_ligada)     return "sm4_r1_ligada";
    if(s == sm4_err)           return "sm4_err";
    return "?";
}
