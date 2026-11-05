// Declara a thread responsável pela execução do comportamento do sistema

#ifndef BEHAVIORTHREAD_H
#define BEHAVIORTHREAD_H

#include <QThread>

class BehaviorThread : public QThread
{
public:
    BehaviorThread();

    void run() override;
};

#endif
