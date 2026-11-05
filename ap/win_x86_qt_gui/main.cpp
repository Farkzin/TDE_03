// Inicializa a aplicação gráfica e executa o comportamento do sistema

#include "mainwindow.h"
#include "behaviorthread.h"

#include <QApplication>

// Cria a interface, inicia a thread de comportamento e executa o loop da aplicação
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    BehaviorThread bthread;

    QObject::connect(&a, &QApplication::aboutToQuit, [&bthread](){
        bthread.requestInterruption();
        bthread.wait();
    });

    w.show();
    bthread.start();
    return a.exec();
}
