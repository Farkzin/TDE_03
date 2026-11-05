#include <Arduino.h>
#include "../ap/common/behavior.h"
#include "../ap/common/chronometer.h"
#include "../ap/common/hal.h"

#define SERIAL_BAUD        9600 // Baud rate para comunicação serial
#define INTERVALO_LOG_MS   500 // Intervalo em milissegundos entre cada mensagem de status enviada ao Serial Monitor

static Chronometer log_chronometer; // Cronômetro usado para controlar o intervalo de log no Serial Monitor

// Imprime o status atual dos sensores e máquinas de estados no Serial Monitor
static void imprimir_status(){
    Serial.print("st1=");  Serial.print(get_st1(), 1); Serial.print("C"); // Temperatura do tanque 3
    
    // Estados dos sensores dos tanques
    Serial.print(" | s11="); Serial.print(get_s11());
    Serial.print(" s12=");   Serial.print(get_s12());
    Serial.print(" s21=");   Serial.print(get_s21());
    Serial.print(" s22=");   Serial.print(get_s22());
    Serial.print(" s31=");   Serial.print(get_s31());
    Serial.print(" s32=");   Serial.print(get_s32());

    // Estados das máquinas de estados
    Serial.print(" | "); Serial.print(sm1_state_name());
    Serial.print(" ");   Serial.print(sm2_state_name());
    Serial.print(" ");   Serial.print(sm3_state_name());
    Serial.print(" ");   Serial.println(sm4_state_name());
}

// Inicia a comunicação serial, o hardware, a máquina de estados e o cronômetro de log
void setup(){
    Serial.begin(SERIAL_BAUD);

    behavior_init();

    log_chronometer.start(INTERVALO_LOG_MS);
}

// Roda um passo de cada máquina de estados e imprime o status a cada INTERVALO_LOG_MS ms
void loop(){
    behavior_step();

    if(log_chronometer.is_finished()){
        imprimir_status();
        log_chronometer.start(INTERVALO_LOG_MS);
    }
}

// Inicializa o Arduino, chama o setup() uma vez e depois o loop() para sempre
int main(void){
    init();

    setup();
    while(1){
        loop();
    }

    return 0;
}
