// Implementa as funções de cronômetro do sistema

#include "chronometer.h"
#include "hal.h"

// Inicia o cronômetro com o tempo definido em milissegundos passado como parâmetro
void Chronometer::start(uint32_t delta_ms){
    chrono = now() + delta_ms;
}

// Verifica se o tempo definido pelo cronômetro já terminou
bool Chronometer::is_finished(){
    return (int32_t)(now() - chrono) >= 0;
}
