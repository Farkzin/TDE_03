// Declara a janela principal e os recursos de controle da simulação

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QString>
#include "plant.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTimer timer_update_ui;
    QTimer timer_physical_process;

    Plant &plant() { return plant_; }

private:
    Ui::MainWindow *ui;
    Plant plant_;
    QElapsedTimer physics_clock;
    QString cor_barra;

    void publish_sensors();

public slots:
    void update_ui();
    void physical_process();

private slots:
    void carregar_cenario();
};
#endif
