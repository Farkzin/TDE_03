// Executa o comportamento do sistema em uma thread separada

#include "behaviorthread.h"
#include "../common/behavior.h"

BehaviorThread::BehaviorThread() {}

void BehaviorThread::run(){
    behavior_init();
    while(!isInterruptionRequested()){
        behavior_step();
        QThread::msleep(1);
    }
}
