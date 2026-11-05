#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define TEMPO_V1_FECHADA            3000  // Tempo que a válvula V1 permanece fechada

#define TEMPO_B1_DESLIGADA          5000  // Tempo que a bomba B1 permanece desligada

#define TEMPO_B1_AGUARDA_AGUA       2000  // Tempo de espera pela presença de água

#define TEMP_SETPOINT               50.0f  // Temperatura desejada para o sistema

#define TEMP_HISTERESE              1.0f   // Variação permitida em torno do setpoint

#define TEMP_LIGA                   (TEMP_SETPOINT - TEMP_HISTERESE)  // Temperatura para ligar o aquecimento

#define TEMP_DESLIGA                (TEMP_SETPOINT + TEMP_HISTERESE)  // Temperatura para desligar o aquecimento

#define TEMP_MIN_VALIDA             0.0f   // Menor temperatura considerada válida
#define TEMP_MAX_VALIDA             100.0f // Maior temperatura considerada válida

#endif
