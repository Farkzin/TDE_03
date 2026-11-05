#include "../common/hal.h"
#include "../win_x86_qt_gui/sim_pins.h"

SimPins sim_pins;
static uint32_t virtual_ms = 0;

void advance_ms(uint32_t ms){ virtual_ms += ms; }

void init_hal(){
    sim_pins.v1=false; sim_pins.b1=false; sim_pins.v2=false; sim_pins.r1=false;
    sim_pins.led_sm1_err=false; sim_pins.led_sm2_err=false;
    sim_pins.led_sm3_err=false; sim_pins.led_sm4_err=false;
}
uint32_t now(){ return virtual_ms; }
void set_v1(bool s){ sim_pins.v1=s; }
void set_b1(bool s){ sim_pins.b1=s; }
void set_v2(bool s){ sim_pins.v2=s; }
void set_r1(bool s){ sim_pins.r1=s; }
bool get_s11(){ return sim_pins.s11; }
bool get_s12(){ return sim_pins.s12; }
bool get_s21(){ return sim_pins.s21; }
bool get_s22(){ return sim_pins.s22; }
bool get_s31(){ return sim_pins.s31; }
bool get_s32(){ return sim_pins.s32; }
float get_st1(){ return sim_pins.st1; }
bool get_btn_reset(){ return sim_pins.btn_reset; }
void set_led_sm1_err(bool s){ sim_pins.led_sm1_err=s; }
void set_led_sm2_err(bool s){ sim_pins.led_sm2_err=s; }
void set_led_sm3_err(bool s){ sim_pins.led_sm3_err=s; }
void set_led_sm4_err(bool s){ sim_pins.led_sm4_err=s; }
