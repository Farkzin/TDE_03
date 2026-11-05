#include "../common/behavior.h"
#include "../common/hal.h"
#include "../win_x86_qt_gui/plant.h"
#include "../win_x86_qt_gui/sim_pins.h"
#include <cstdio>
#include <cstring>

void advance_ms(uint32_t);

static int fails = 0;
#define CHECK(c, msg) do{ if(!(c)){ printf("  [FALHA] %s\n", msg); fails++; } else printf("  [ok]    %s\n", msg);}while(0)

struct Faults { bool t1=false, t2=false, t3=false, temp=false; };

struct Stats {
    double t=0, above80=0, minL2=100, maxL1=0, maxL2=0;
    bool dry=false, ovf1=false, ovf2=false, hdry=false;
    double tmin=1e9, tmax=-1e9;
    int r1_sem_agua=0;
    double v2_max_pos=0;
    double first_flow_t=-1;
};

static void publish(Plant &p, const Faults &f = Faults()){
    sim_pins.s11 = f.t1 ? false : p.s11();  sim_pins.s12 = f.t1 ? true : p.s12();
    sim_pins.s21 = f.t2 ? false : p.s21();  sim_pins.s22 = f.t2 ? true : p.s22();
    sim_pins.s31 = f.t3 ? false : p.s31();  sim_pins.s32 = f.t3 ? true : p.s32();
    sim_pins.st1 = f.temp ? 120.0f : (float)p.temperature_c();
}

static Stats run(Plant &p, double seconds, const Faults &f = Faults(), bool supply = true){
    Stats s;
    for(int i=0;i<(int)(seconds*100);i++){
        publish(p,f);
        behavior_step();
        p.step(0.01, sim_pins.v1 && supply, sim_pins.b1 && supply, sim_pins.v2, sim_pins.r1);
        advance_ms(10);
        s.t += 0.01;
        if(p.level2_pct()>=80) s.above80 += 0.01;
        if(p.level2_pct()<s.minL2) s.minL2 = p.level2_pct();
        if(p.level1_pct()>s.maxL1) s.maxL1 = p.level1_pct();
        if(p.level2_pct()>s.maxL2) s.maxL2 = p.level2_pct();
        s.dry |= p.pump_dry(); s.ovf1 |= p.overflow1(); s.ovf2 |= p.overflow2(); s.hdry |= p.heater_dry();
        if(p.temperature_c()<s.tmin) s.tmin = p.temperature_c();
        if(p.temperature_c()>s.tmax) s.tmax = p.temperature_c();
        if(sim_pins.r1 && !p.s31()) s.r1_sem_agua++;
        if(p.v2_position()>s.v2_max_pos) s.v2_max_pos = p.v2_position();
        if(s.first_flow_t<0 && p.v2_flow_lps()!=0) s.first_flow_t = s.t;
    }
    return s;
}

static bool near(double a, double b, double tol){ return a>b-tol && a<b+tol; }

static Plant::Config cen(double t2, double t3, double temp){
    Plant::Config c; c.initial2_pct=t2; c.initial3_pct=t3; c.initial_temp_c=temp; return c;
}

int main(){
    printf("== Cenario 1: operacao normal (consumo frio ligado, 300 s)\n");
    { Plant p; p.set_drain2(true); publish(p); behavior_init();
      Stats s = run(p, 300);
      printf("   T2 acima de 80%% em %.1f%% do tempo; min T2=%.1f%%; max T1=%.1f%% max T2=%.1f%%\n",
             100*s.above80/s.t, s.minL2, s.maxL1, s.maxL2);
      CHECK(!s.dry, "bomba nunca funcionou a seco");
      CHECK(!s.ovf1 && !s.ovf2, "nenhum transbordamento em T1/T2");
      CHECK(s.above80/s.t > 0.85, "T2 permanece cheia (>=80%) em mais de 85% do tempo");
      CHECK(p.lost_l() < 1e-6, "nenhuma agua perdida");
      double bal = p.supplied_l() + 0.30*1000 + 0.60*500 - p.volume1_l() - p.volume2_l() - p.volume3_l() - p.consumed_l() - p.lost_l();
      CHECK(bal > -1e-6 && bal < 1e-6, "conservacao de massa na simulacao (T1+T2+T3)");
      CHECK(p.level3_pct() > 85, "boiler T3 foi enchido por V2");
      CHECK(!s.hdry, "R1 nunca ligou com T3 vazio");
    }
    printf("== Cenario 2: consumo frio muito alto (80 L/s > vazao da bomba 50 L/s), 300 s\n");
    { Plant::Config c; c.drain2_lps = 80; Plant p(c); p.set_drain2(true); publish(p); behavior_init();
      Stats s = run(p, 300);
      printf("   min T2=%.1f%%\n", s.minL2);
      CHECK(!s.dry, "protecao contra funcionamento a seco: B1 nunca bombeou com T1 vazia");
      CHECK(!s.ovf1 && !s.ovf2, "sem transbordamento");
    }
    printf("== Cenario 3: falha de sensor em T2 (S22=1, S21=0)\n");
    { Plant p; publish(p); behavior_init();
      run(p, 5);
      Faults f; f.t2 = true;
      run(p, 5, f);
      CHECK(strcmp(sm2_state_name(),"sm2_err")==0, "SM2 foi para sm2_err");
      CHECK(!sim_pins.b1 && sim_pins.led_sm2_err, "bomba desligada e LED de erro aceso");
      run(p, 5);
      CHECK(strcmp(sm2_state_name(),"sm2_err")==0, "falha continua travada sem reset");
      sim_pins.btn_reset = true; run(p, 0.1); sim_pins.btn_reset = false; run(p, 1);
      CHECK(strcmp(sm2_state_name(),"sm2_err")!=0 && !sim_pins.led_sm2_err, "reset manual recupera SM2");
    }
    printf("== Cenario 4: falha de sensor em T1 (S12=1, S11=0)\n");
    { Plant p; publish(p); behavior_init();
      Faults f; f.t1 = true;
      run(p, 1, f);
      CHECK(strcmp(sm1_state_name(),"sm1_err")==0, "SM1 foi para sm1_err");
      CHECK(!sim_pins.v1 && sim_pins.led_sm1_err, "V1 fechada e LED de erro aceso");
    }
    printf("== Cenario 5: partida com T1 vazia e T2 vazia\n");
    { Plant::Config c; c.initial1_pct=0; c.initial2_pct=0; Plant p(c); publish(p); behavior_init();
      Stats s = run(p, 120);
      CHECK(!s.dry, "sem funcionamento a seco na partida");
      CHECK(p.level2_pct() > 50, "sistema enche T2 a partir do zero");
    }

    printf("== Cenario A (spec): t2=100%%, t3=0%%, 25C - enchimento completo\n");
    { Plant p(cen(100,0,25)); publish(p); behavior_init();
      Stats s = run(p, 25, Faults(), false);
      CHECK(s.first_flow_t > 14.9, "V2 leva 15 s para abrir: nao ha fluxo antes disso");
      Stats s2 = run(p, 120, Faults(), false);
      printf("   t2=%.2f%% t3=%.2f%% temp=%.2f (min %.2f max %.2f)\n", p.level2_pct(), p.level3_pct(), p.temperature_c(), s2.tmin, s2.tmax);
      CHECK(near(p.level2_pct(), 77.5, 0.5), "t2 ~ 77,5%");
      CHECK(near(p.level3_pct(), 90.0, 0.5), "t3 = 90%");
      CHECK(strcmp(sm3_state_name(),"sm3_v2_fechada")==0, "SM3 fechou V2 quando S32 acendeu");
      Stats s3 = run(p, 60, Faults(), false);
      CHECK(s3.tmin > 48.9 && s3.tmax < 51.1, "temperatura oscila entre 49 e 51 C");
      CHECK(!s.hdry && !s2.hdry && !s3.hdry, "sem aquecimento a seco");
    }
    printf("== Cenario B (spec): t2=50%%, t3=0%%, 25C - equilibrio parcial\n");
    { Plant p(cen(50,0,25)); publish(p); behavior_init();
      run(p, 120, Faults(), false);
      Stats s = run(p, 60, Faults(), false);
      printf("   t2=%.2f%% t3=%.2f%%\n", p.level2_pct(), p.level3_pct());
      CHECK(near(p.level2_pct(), 33.3, 0.5), "t2 ~ 33,3%");
      CHECK(near(p.level3_pct(), 66.6, 0.5), "t3 ~ 66,6%");
      CHECK(s.tmin > 48.9 && s.tmax < 51.1, "temperatura oscila entre 49 e 51 C");
    }
    printf("== Cenario C (spec): t2=0%%, t3=100%%, 25C - protecao contra refluxo\n");
    { Plant p(cen(0,100,25)); publish(p); behavior_init();
      Stats s = run(p, 60, Faults(), false);
      printf("   t2=%.2f%% t3=%.2f%% v2 pos max=%.2f\n", p.level2_pct(), p.level3_pct(), s.v2_max_pos);
      CHECK(s.v2_max_pos == 0.0, "V2 permaneceu fechada");
      CHECK(near(p.level2_pct(), 0.0, 0.01) && near(p.level3_pct(), 100.0, 0.01), "niveis inalterados");
      Stats s2 = run(p, 30, Faults(), false);
      CHECK(s2.tmin > 48.9 && s2.tmax < 51.1, "R1 ligou (ha agua em T3) e a temperatura oscila entre 49 e 51 C");
    }
    printf("== Cenario D (spec): t2=10%%, t3=89%%, 25C - decisao com informacao limitada\n");
    { Plant p(cen(10,89,25)); publish(p); behavior_init();
      Stats s = run(p, 60, Faults(), false);
      printf("   t2=%.2f%% t3=%.2f%%\n", p.level2_pct(), p.level3_pct());
      CHECK(s.v2_max_pos == 1.0, "V2 abriu (SM3 nao consegue garantir que t2 < t3)");
      CHECK(near(p.level2_pct(), 21.5, 0.5), "t2 ~ 21,5%");
      CHECK(near(p.level3_pct(), 43.0, 0.5), "t3 ~ 43%");
      Stats s2 = run(p, 30, Faults(), false);
      CHECK(s2.tmin > 48.9 && s2.tmax < 51.1, "temperatura oscila entre 49 e 51 C");
    }
    printf("== Cenario E (spec): t2=50%%, t3=15%%, 50C, dreno de t3 ligado\n");
    { Plant p(cen(50,15,50)); p.set_drain3(true); publish(p); behavior_init();
      Stats s = run(p, 120, Faults(), false);
      printf("   t2=%.2f%% t3=%.2f%% temp=%.2f r1=%d\n", p.level2_pct(), p.level3_pct(), p.temperature_c(), (int)sim_pins.r1);
      CHECK(p.volume3_l() < 0.5, "t3 ficou vazio");
      CHECK(!sim_pins.r1, "R1 desligada no fim");
      CHECK(!s.hdry, "nenhum aquecimento a seco");
      CHECK(p.temperature_c() < 50.0, "temperatura diminuindo");
    }
    printf("== Cenario E2: R1 LIGADA aquecendo e T3 esvaziando (prova da protecao)\n");
    { Plant p(cen(0,40,25)); p.set_drain3(true); publish(p); behavior_init();
      Stats s = run(p, 20, Faults(), false);
      printf("   passos com R1 ligada e S31 apagado: %d\n", s.r1_sem_agua);
      CHECK(s.r1_sem_agua <= 3, "R1 desliga imediatamente quando S31 apaga");
      CHECK(!sim_pins.r1 && !s.hdry, "R1 desligada e sem alarme de aquecimento a seco");
    }
    printf("== Cenario F: falha de sensor em T3 (S32=1, S31=0)\n");
    { Plant p(cen(100,0,25)); publish(p); behavior_init();
      Faults f; f.t3 = true;
      run(p, 1, f, false);
      CHECK(strcmp(sm3_state_name(),"sm3_err")==0, "SM3 foi para sm3_err");
      CHECK(!sim_pins.v2 && sim_pins.led_sm3_err, "V2 fechada e LED de erro aceso");
      run(p, 2, Faults(), false);
      CHECK(strcmp(sm3_state_name(),"sm3_err")==0, "falha continua travada sem reset");
      sim_pins.btn_reset = true; run(p, 0.1, Faults(), false); sim_pins.btn_reset = false; run(p, 1, Faults(), false);
      CHECK(strcmp(sm3_state_name(),"sm3_err")!=0 && !sim_pins.led_sm3_err, "reset manual recupera SM3");
    }
    printf("== Cenario G: temperatura fora da faixa (st1 = 120 C)\n");
    { Plant p(cen(0,100,25)); publish(p); behavior_init();
      run(p, 3, Faults(), false);
      CHECK(sim_pins.r1, "antes da falha R1 estava ligada (T3 cheio e fria)");
      Faults f; f.temp = true;
      run(p, 1, f, false);
      CHECK(strcmp(sm4_state_name(),"sm4_err")==0, "SM4 foi para sm4_err");
      CHECK(!sim_pins.r1 && sim_pins.led_sm4_err, "R1 desligada e LED de erro aceso");
      run(p, 2, Faults(), false);
      CHECK(strcmp(sm4_state_name(),"sm4_err")==0, "falha continua travada sem reset");
      sim_pins.btn_reset = true; run(p, 0.1, Faults(), false); sim_pins.btn_reset = false; run(p, 1, Faults(), false);
      CHECK(strcmp(sm4_state_name(),"sm4_err")!=0 && !sim_pins.led_sm4_err, "reset manual recupera SM4");
    }
    printf("\n%s (%d falha(s))\n", fails? "REPROVADO":"TODOS OS TESTES PASSARAM", fails);
    return fails?1:0;
}
