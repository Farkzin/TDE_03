#include "../common/hal.h"
#include <Arduino.h>

// Define quais pinos de entrada do arduino correspondem a cada um dos sensor dos tanques 
#define PIN_S11   2
#define PIN_S12   3
#define PIN_S21   4
#define PIN_S22   5
#define PIN_S31   6
#define PIN_S32   7

// Define quais pinos de saída do arduino correspondem a cada um dos atuadores (válvulas, bomba e resistência)
#define PIN_V1    8
#define PIN_V2    9
#define PIN_B1    10
#define PIN_R1    11

#define PIN_ST1   A0 // Define o pino de entrada do sensor de temperatura do tanque 3

// Define quais pinos de saída do arduino correspondem a cada um dos LEDs de erro
#define PIN_LED_SM1_ERR  12
#define PIN_LED_SM2_ERR  13
#define PIN_LED_SM3_ERR  A1
#define PIN_LED_SM4_ERR  A2

// Define os valores mínimo e máximo da temperatura do potenciômetro
#define TEMP_POT_MIN  40.0f
#define TEMP_POT_MAX  60.0f

// Inicializa os pinos do Arduino e define os estados iniciais dos atuadores e LEDs de erro
void init_hal(){

    // Configura os pinos das chaves como entrada
    pinMode(PIN_S11, INPUT);
    pinMode(PIN_S12, INPUT);
    pinMode(PIN_S21, INPUT);
    pinMode(PIN_S22, INPUT);
    pinMode(PIN_S31, INPUT);
    pinMode(PIN_S32, INPUT);

    // Configura os pinos dos LEDs dos atuadores como saída
    pinMode(PIN_V1, OUTPUT);
    pinMode(PIN_V2, OUTPUT);
    pinMode(PIN_B1, OUTPUT);
    pinMode(PIN_R1, OUTPUT);

    // Configura os pinos dos LEDs de erro como saída
    pinMode(PIN_LED_SM1_ERR, OUTPUT);
    pinMode(PIN_LED_SM2_ERR, OUTPUT);
    pinMode(PIN_LED_SM3_ERR, OUTPUT);
    pinMode(PIN_LED_SM4_ERR, OUTPUT);

    // Desliga todos os atuadores e LEDs de erro para o sistema começar em um estado seguro
    set_v1(false);
    set_v2(false);
    set_b1(false);
    set_r1(false);
    set_led_sm1_err(false);
    set_led_sm2_err(false);
    set_led_sm3_err(false);
    set_led_sm4_err(false);
}

// Retorna o tempo atual em milissegundos desde que o Arduino foi iniciado
uint32_t now(){
    return millis(); 
}

// Chamadas pela state machine, liga ou desliga os LEDs dos atuadores
void set_v1(bool state){ digitalWrite(PIN_V1, state); }
void set_v2(bool state){ digitalWrite(PIN_V2, state); }
void set_b1(bool state){ digitalWrite(PIN_B1, state); }
void set_r1(bool state){ digitalWrite(PIN_R1, state); }

// Chamadas pela state machine, retorna o estado atual de cada sensor dos tanques
bool get_s11(){ return digitalRead(PIN_S11); }
bool get_s12(){ return digitalRead(PIN_S12); }
bool get_s21(){ return digitalRead(PIN_S21); }
bool get_s22(){ return digitalRead(PIN_S22); }
bool get_s31(){ return digitalRead(PIN_S31); }
bool get_s32(){ return digitalRead(PIN_S32); }

// Lê o potenciômetro pelo conversor AD (valor de 0 a 1023) e converte para graus Celsius (40 a 60)
float get_st1(){
    int valorLido = analogRead(PIN_ST1);

    // Interpolação linear entre TEMP_POT_MIN e TEMP_POT_MAX com base no valor lido do potenciômetro (0 a 1023)
    return TEMP_POT_MIN + (valorLido / 1023.0f) * (TEMP_POT_MAX - TEMP_POT_MIN);
}

// Como não há botão de reset físico, esta função sempre retorna false (nunca será possível resetar o sistema via botão)
bool get_btn_reset(){
    return false;
}

// Acende ou apaga os LEDs de erro de cada máquina de estados
void set_led_sm1_err(bool state){ digitalWrite(PIN_LED_SM1_ERR, state); }
void set_led_sm2_err(bool state){ digitalWrite(PIN_LED_SM2_ERR, state); }
void set_led_sm3_err(bool state){ digitalWrite(PIN_LED_SM3_ERR, state); }
void set_led_sm4_err(bool state){ digitalWrite(PIN_LED_SM4_ERR, state); }
