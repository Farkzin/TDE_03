// Declara a classe responsável pelo controle de intervalos de tempo

#ifndef CHRONOMETER_H
#define CHRONOMETER_H

#include <stdint.h>

class Chronometer{
private:
    uint32_t chrono = 0;

public:
    void start(uint32_t delta_ms);
    bool is_finished();
};

#endif
