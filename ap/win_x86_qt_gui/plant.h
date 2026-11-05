// Declara a planta simulada e suas configurações, sensores e atuadores

#ifndef PLANT_H
#define PLANT_H

class Plant {
public:
    // Define os parâmetros físicos e as condições iniciais da planta
    struct Config {
        double cap1_l          = 1000.0;
        double cap2_l          =  500.0;
        double v1_flow_lps     =  100.0;
        double b1_flow_lps     =   50.0;
        double v2_flow_lps     =    5.0;
        double v2_time_s       =   15.0;
        double drain2_lps      =    5.0;
        double drain3_lps      =    8.0;
        double s11_pct         =   10.0;
        double s12_pct         =   90.0;
        double s21_pct         =   10.0;
        double s22_pct         =   90.0;
        double s31_pct         =   10.0;
        double s32_pct         =   90.0;
        double hysteresis_pct  =    1.0;
        double alarm_hold_s    =    1.5;
        double initial1_pct    =   30.0;
        double initial2_pct    =   60.0;
        double initial3_pct    =    0.0;
        double initial_temp_c  =   25.0;
        double ambient_c       =   25.0;
        double heat_cps        =    2.0;
        double cool_cps        =    0.5;
    };

    Plant();
    explicit Plant(const Config &cfg);

    // Avança a simulação considerando os atuadores ativos
    void step(double dt_s, bool v1_open, bool b1_on, bool v2_open, bool r1_on);

    // Controla e consulta os drenos dos tanques
    void set_drain2(bool on) { drain2_on = on; }
    void set_drain3(bool on) { drain3_on = on; }
    bool drain2() const { return drain2_on; }
    bool drain3() const { return drain3_on; }

    // Consulta os níveis e volumes atuais dos tanques
    double level1_pct() const { return 100.0 * vol1 / cfg.cap1_l; }
    double level2_pct() const { return 100.0 * vol2 / cfg.cap2_l; }
    double level3_pct() const { return 100.0 * vol3 / cap3_l(); }
    double volume1_l()  const { return vol1; }
    double volume2_l()  const { return vol2; }
    double volume3_l()  const { return vol3; }
    double cap3_l()     const { return cfg.cap2_l / 4.0; }

    // Consulta os estados dos sensores de nível
    bool s11() const { return sens_s11.state; }
    bool s12() const { return sens_s12.state; }
    bool s21() const { return sens_s21.state; }
    bool s22() const { return sens_s22.state; }
    bool s31() const { return sens_s31.state; }
    bool s32() const { return sens_s32.state; }

    // Consulta a temperatura e o estado da válvula V2
    double temperature_c() const { return temp_c; }
    double v2_position()   const { return v2_pos; }
    double v2_flow_lps()   const { return v2_flow; }

    // Consulta as condições de alarme e os volumes acumulados
    bool overflow1() const { return hold_overflow1 > 0; }
    bool overflow2() const { return hold_overflow2 > 0; }
    bool pump_dry()  const { return hold_pump_dry  > 0; }
    bool heater_dry() const { return hold_heater_dry > 0; }

    double lost_l()     const { return total_lost; }
    double consumed_l() const { return total_consumed; }
    double supplied_l() const { return total_supplied; }

private:
    // Representa um sensor de nível com histerese
    struct Sensor {
        double on_pct  = 0;
        double off_pct = 0;
        bool   state   = false;
        void update(double level_pct){
            if(!state && level_pct >= on_pct)  state = true;
            else if(state && level_pct < off_pct) state = false;
        }
    };

    // Atualiza a válvula V2 e os estados dos sensores
    void update_v2(double dt_s, bool v2_open);
    void update_sensors();

    Config cfg;
    double vol1 = 0, vol2 = 0, vol3 = 0;
    double temp_c = 25.0;
    double v2_pos = 0.0;
    double v2_flow = 0.0;
    bool drain2_on = false, drain3_on = false;
    Sensor sens_s11, sens_s12, sens_s21, sens_s22, sens_s31, sens_s32;
    double hold_overflow1 = 0, hold_overflow2 = 0, hold_pump_dry = 0, hold_heater_dry = 0;
    double total_lost = 0, total_consumed = 0, total_supplied = 0;
};

#endif
