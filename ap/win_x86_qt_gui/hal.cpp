// Implementa a interface de acesso ao hardware simulado da aplicação

#include "../common/hal.h"
#include "sim_pins.h"
#include <chrono>

SimPins sim_pins;

// Inicializa todas as saídas do hardware simulado
void init_hal(){
    sim_pins.v1 = false;
    sim_pins.b1 = false;
    sim_pins.v2 = false;
    sim_pins.r1 = false;
    sim_pins.led_sm1_err = false;
    sim_pins.led_sm2_err = false;
    sim_pins.led_sm3_err = false;
    sim_pins.led_sm4_err = false;
}

// Retorna o tempo decorrido desde o início da simulação
uint32_t now(){
    using namespace std::chrono;
    static const steady_clock::time_point t0 = steady_clock::now();
    return (uint32_t)duration_cast<milliseconds>(steady_clock::now() - t0).count();
}

// Controla os atuadores do sistema
void set_v1(bool state){ sim_pins.v1 = state; }
void set_b1(bool state){ sim_pins.b1 = state; }
void set_v2(bool state){ sim_pins.v2 = state; }
void set_r1(bool state){ sim_pins.r1 = state; }

// Lê os sensores, a temperatura e o botão de reset
bool  get_s11(){ return sim_pins.s11; }
bool  get_s12(){ return sim_pins.s12; }
bool  get_s21(){ return sim_pins.s21; }
bool  get_s22(){ return sim_pins.s22; }
bool  get_s31(){ return sim_pins.s31; }
bool  get_s32(){ return sim_pins.s32; }
float get_st1(){ return sim_pins.st1; }
bool  get_btn_reset(){ return sim_pins.btn_reset; }

// Controla os LEDs de erro das máquinas de estados
void set_led_sm1_err(bool state){ sim_pins.led_sm1_err = state; }
void set_led_sm2_err(bool state){ sim_pins.led_sm2_err = state; }
void set_led_sm3_err(bool state){ sim_pins.led_sm3_err = state; }
void set_led_sm4_err(bool state){ sim_pins.led_sm4_err = state; }
