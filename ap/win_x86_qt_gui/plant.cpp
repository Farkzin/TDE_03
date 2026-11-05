// Implementa a simulação física dos tanques, fluxos, sensores e temperatura

#include "plant.h"
#include <algorithm>

// Inicializa a planta com a configuração padrão
Plant::Plant() : Plant(Config()) {}

// Inicializa volumes, temperatura e estados dos sensores da planta
Plant::Plant(const Config &c) : cfg(c)
{
    vol1 = cfg.cap1_l * cfg.initial1_pct / 100.0;
    vol2 = cfg.cap2_l * cfg.initial2_pct / 100.0;
    vol3 = cap3_l()   * cfg.initial3_pct / 100.0;
    temp_c = cfg.initial_temp_c;

    auto setup = [&](Sensor &s, double pct){
        s.on_pct  = pct;
        s.off_pct = pct - cfg.hysteresis_pct;
    };
    setup(sens_s11, cfg.s11_pct);
    setup(sens_s12, cfg.s12_pct);
    setup(sens_s21, cfg.s21_pct);
    setup(sens_s22, cfg.s22_pct);
    setup(sens_s31, cfg.s31_pct);
    setup(sens_s32, cfg.s32_pct);

    sens_s11.state = level1_pct() >= cfg.s11_pct;
    sens_s12.state = level1_pct() >= cfg.s12_pct;
    sens_s21.state = level2_pct() >= cfg.s21_pct;
    sens_s22.state = level2_pct() >= cfg.s22_pct;
    sens_s31.state = level3_pct() >= cfg.s31_pct;
    sens_s32.state = level3_pct() >= cfg.s32_pct;
}

// Avança a simulação considerando os atuadores ativos no ciclo
void Plant::step(double dt, bool v1_open, bool b1_on, bool v2_open, bool r1_on)
{
    if(dt <= 0) return;

    bool ovf1 = false, ovf2 = false, dry = false, hdry = false;

    if(v1_open){
        double in = cfg.v1_flow_lps * dt;
        double space = cfg.cap1_l - vol1;
        if(in > space){
            total_lost += in - space;
            in = space;
            ovf1 = true;
        }
        vol1 += in;
        total_supplied += in;
    }

    if(b1_on){
        double want = cfg.b1_flow_lps * dt;
        double moved = std::min(want, vol1);
        if(moved < want) dry = true;
        vol1 -= moved;
        vol2 += moved;
        if(vol2 > cfg.cap2_l){
            total_lost += vol2 - cfg.cap2_l;
            vol2 = cfg.cap2_l;
            ovf2 = true;
        }
    }

    update_v2(dt, v2_open);

    if(drain2_on){
        double use = std::min(cfg.drain2_lps * dt, vol2);
        vol2 -= use;
        total_consumed += use;
    }
    if(drain3_on){
        double use = std::min(cfg.drain3_lps * dt, vol3);
        vol3 -= use;
        total_consumed += use;
    }

    if(r1_on){
        temp_c += cfg.heat_cps * dt;
        if(vol3 < 0.001) hdry = true;
    } else {
        temp_c = std::max(cfg.ambient_c, temp_c - cfg.cool_cps * dt);
    }

    auto hold = [&](double &h, bool event){
        if(event) h = cfg.alarm_hold_s;
        else      h = std::max(0.0, h - dt);
    };
    hold(hold_overflow1, ovf1);
    hold(hold_overflow2, ovf2);
    hold(hold_pump_dry,  dry);
    hold(hold_heater_dry, hdry);

    update_sensors();
}

// Atualiza a posição da válvula V2 e calcula o fluxo entre os tanques
void Plant::update_v2(double dt, bool v2_open)
{
    double target = v2_open ? 1.0 : 0.0;
    double mv = dt / cfg.v2_time_s;
    if(v2_pos < target)      v2_pos = std::min(target, v2_pos + mv);
    else if(v2_pos > target) v2_pos = std::max(target, v2_pos - mv);

    v2_flow = 0.0;
    if(v2_pos < 1.0) return;

    double h2 = level2_pct();
    double h3 = level3_pct() / 2.0;
    double to_equilibrium = (h2 - h3) * cfg.cap2_l / 300.0;

    double max_move = cfg.v2_flow_lps * dt;
    double move = std::max(-max_move, std::min(max_move, to_equilibrium));

    if(move > 0){
        move = std::min(move, std::min(vol2, cap3_l() - vol3));
    } else {
        move = std::max(move, -std::min(vol3, cfg.cap2_l - vol2));
    }

    vol2 -= move;
    vol3 += move;
    v2_flow = move / dt;
}

// Atualiza os estados dos sensores conforme os níveis atuais dos tanques
void Plant::update_sensors()
{
    sens_s11.update(level1_pct());
    sens_s12.update(level1_pct());
    sens_s21.update(level2_pct());
    sens_s22.update(level2_pct());
    sens_s31.update(level3_pct());
    sens_s32.update(level3_pct());
}
