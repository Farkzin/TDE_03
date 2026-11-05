// Declara os sinais simulados de entrada e saída do sistema

#ifndef SIM_PINS_H
#define SIM_PINS_H

#include <atomic>

struct SimPins {
    std::atomic<bool> v1{false};
    std::atomic<bool> b1{false};
    std::atomic<bool> v2{false};
    std::atomic<bool> r1{false};
    std::atomic<bool> s11{false};
    std::atomic<bool> s12{false};
    std::atomic<bool> s21{false};
    std::atomic<bool> s22{false};
    std::atomic<bool> s31{false};
    std::atomic<bool> s32{false};
    std::atomic<float> st1{25.0f};
    std::atomic<bool> btn_reset{false};
    std::atomic<bool> led_sm1_err{false};
    std::atomic<bool> led_sm2_err{false};
    std::atomic<bool> led_sm3_err{false};
    std::atomic<bool> led_sm4_err{false};
};

extern SimPins sim_pins;

#endif
