// Declara as funções de acesso e controle do hardware do sistema

#ifndef HAL_H
#define HAL_H

#include <stdint.h>

void     init_hal();
uint32_t now();

void set_v1(bool state);
void set_b1(bool state);
void set_v2(bool state);
void set_r1(bool state);

bool  get_s11();
bool  get_s12();
bool  get_s21();
bool  get_s22();
bool  get_s31();
bool  get_s32();
float get_st1();
bool  get_btn_reset();

void set_led_sm1_err(bool state);
void set_led_sm2_err(bool state);
void set_led_sm3_err(bool state);
void set_led_sm4_err(bool state);

#endif
