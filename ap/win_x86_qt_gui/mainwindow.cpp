// Implementa a janela principal e a atualização da interface gráfica da simulação

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sim_pins.h"
#include "../common/sm1.h"
#include "../common/sm2.h"
#include "../common/sm3.h"
#include "../common/sm4.h"

#include <algorithm>
#include <cmath>

// Inicializa os componentes visuais, os controles e os temporizadores da aplicação
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->toolButton_v1->setColor(QColor(60,200,90));
    ui->toolButton_b1->setColor(QColor(60,200,90));
    ui->toolButton_v2->setColor(QColor(60,200,90));
    ui->toolButton_r1->setColor(QColor(255,80,40));
    ui->toolButton_s11->setColor(QColor(30,110,230));
    ui->toolButton_s12->setColor(QColor(30,110,230));
    ui->toolButton_s21->setColor(QColor(30,110,230));
    ui->toolButton_s22->setColor(QColor(30,110,230));
    ui->toolButton_s31->setColor(QColor(30,110,230));
    ui->toolButton_s32->setColor(QColor(30,110,230));
    ui->toolButton_overflow1->setColor(QColor(255,140,0));
    ui->toolButton_overflow2->setColor(QColor(255,140,0));
    ui->toolButton_pump_dry->setColor(QColor(255,140,0));
    ui->toolButton_heater_dry->setColor(QColor(255,140,0));

    for(cedaTank *t : {ui->toolButton_t1, ui->toolButton_t2, ui->toolButton_t3}){
        t->ShowDeger1 = true;
        t->Deger1Suffix = "%";
    }

    connect(ui->checkBox_dreno2, &QCheckBox::toggled, this, [this](bool on){ plant_.set_drain2(on); });
    connect(ui->checkBox_dreno3, &QCheckBox::toggled, this, [this](bool on){ plant_.set_drain3(on); });

    connect(ui->pushButton_cenario, &QPushButton::clicked, this, &MainWindow::carregar_cenario);

    connect(ui->pushButton_reset, &QPushButton::pressed,  this, [](){ sim_pins.btn_reset = true;  });
    connect(ui->pushButton_reset, &QPushButton::released, this, [](){ sim_pins.btn_reset = false; });

    publish_sensors();

    connect(&timer_update_ui, &QTimer::timeout, this, &MainWindow::update_ui);
    timer_update_ui.start(33);

    physics_clock.start();
    connect(&timer_physical_process, &QTimer::timeout, this, &MainWindow::physical_process);
    timer_physical_process.start(10);
}

// Libera os recursos utilizados pela janela principal
MainWindow::~MainWindow()
{
    delete ui;
}

// Carrega o cenário selecionado e atualiza as condições iniciais da planta
void MainWindow::carregar_cenario()
{
    struct Cenario { double t2, t3, temp; bool dreno3; };
    static const Cenario cenarios[] = {
        { 100,   0, 25, false },
        {  50,   0, 25, false },
        {   0, 100, 25, false },
        {  10,  89, 25, false },
        {  50,  15, 50, true  },
    };

    int i = ui->comboBox_cenario->currentIndex();
    if(i < 0 || i > 4) return;
    const Cenario &c = cenarios[i];

    Plant::Config cfg;
    cfg.initial2_pct   = c.t2;
    cfg.initial3_pct   = c.t3;
    cfg.initial_temp_c = c.temp;
    plant_ = Plant(cfg);

    ui->checkBox_dreno2->setChecked(false);
    ui->checkBox_dreno3->setChecked(c.dreno3);
    ui->checkBox_sem_abastecimento->setChecked(true);
    plant_.set_drain2(ui->checkBox_dreno2->isChecked());
    plant_.set_drain3(ui->checkBox_dreno3->isChecked());

    publish_sensors();
}

// Publica na simulação os estados dos sensores e as falhas selecionadas na interface
void MainWindow::publish_sensors()
{
    bool s11 = plant_.s11(), s12 = plant_.s12();
    bool s21 = plant_.s21(), s22 = plant_.s22();
    bool s31 = plant_.s31(), s32 = plant_.s32();
    float st1 = (float)plant_.temperature_c();

    if(ui->checkBox_falha_sm1->isChecked()){ s12 = true; s11 = false; }
    if(ui->checkBox_falha_sm2->isChecked()){ s22 = true; s21 = false; }
    if(ui->checkBox_falha_sm3->isChecked()){ s32 = true; s31 = false; }
    if(ui->checkBox_falha_sm4->isChecked()){ st1 = 120.0f; }

    sim_pins.s11 = s11;
    sim_pins.s12 = s12;
    sim_pins.s21 = s21;
    sim_pins.s22 = s22;
    sim_pins.s31 = s31;
    sim_pins.s32 = s32;
    sim_pins.st1 = st1;
}

// Executa a simulação física e atualiza os sensores da planta
void MainWindow::physical_process()
{
    double dt = physics_clock.restart() / 1000.0;
    if(dt > 0.1) dt = 0.1;

    bool abastecimento = !ui->checkBox_sem_abastecimento->isChecked();

    plant_.step(dt, sim_pins.v1 && abastecimento, sim_pins.b1 && abastecimento, sim_pins.v2, sim_pins.r1);

    publish_sensors();
}

// Atualiza os indicadores visuais, níveis, temperatura, atuadores e estados das máquinas
void MainWindow::update_ui()
{
    ui->toolButton_t1->setValue((int)std::lround(plant_.level1_pct()));
    ui->toolButton_t2->setValue((int)std::lround(plant_.level2_pct()));
    ui->toolButton_t3->setValue((int)std::lround(plant_.level3_pct()));

    bool v1 = sim_pins.v1, b1 = sim_pins.b1, v2 = sim_pins.v2, r1 = sim_pins.r1;
    ui->toolButton_v1->setPower(v1);
    ui->toolButton_b1->setPower(b1);
    ui->toolButton_v2->setPower(v2);
    ui->toolButton_r1->setPower(r1);
    bool abastecimento = !ui->checkBox_sem_abastecimento->isChecked();
    ui->pipes->setV1(v1 && abastecimento);
    ui->pipes->setB1(b1 && abastecimento);
    ui->pipes->setV2(plant_.v2_position(), plant_.v2_flow_lps() != 0.0);
    ui->pipes->setDrain2(plant_.drain2() && plant_.volume2_l() > 0);
    ui->pipes->setDrain3(plant_.drain3() && plant_.volume3_l() > 0);

    ui->toolButton_s11->setPower(sim_pins.s11);
    ui->toolButton_s12->setPower(sim_pins.s12);
    ui->toolButton_s21->setPower(sim_pins.s21);
    ui->toolButton_s22->setPower(sim_pins.s22);
    ui->toolButton_s31->setPower(sim_pins.s31);
    ui->toolButton_s32->setPower(sim_pins.s32);

    float temp = sim_pins.st1;
    ui->label_temp->setText(QString("Temperatura de T3: %1 %2C")
                                .arg(temp, 0, 'f', 1)
                                .arg(QChar(0x00B0)));
    ui->progressBar_temp->setValue(std::clamp((int)std::lround(temp), 0, 100));
    QString cor = "#1e6ee6";
    if(temp > 51.0f)       cor = "#dc3c3c";
    else if(temp >= 49.0f) cor = "#3cc85a";
    if(cor != cor_barra){
        cor_barra = cor;
        ui->progressBar_temp->setStyleSheet(QString("QProgressBar::chunk{background-color:%1;}").arg(cor));
    }

    ui->toolButton_overflow1->setPower(plant_.overflow1());
    ui->toolButton_overflow2->setPower(plant_.overflow2());
    ui->toolButton_pump_dry->setPower(plant_.pump_dry());
    ui->toolButton_heater_dry->setPower(plant_.heater_dry());
    ui->toolButton_led_sm1_err->setPower(sim_pins.led_sm1_err);
    ui->toolButton_led_sm2_err->setPower(sim_pins.led_sm2_err);
    ui->toolButton_led_sm3_err->setPower(sim_pins.led_sm3_err);
    ui->toolButton_led_sm4_err->setPower(sim_pins.led_sm4_err);

    ui->label_sm1->setText(QString("SM1 (V1): %1").arg(sm1_state_name()));
    ui->label_sm2->setText(QString("SM2 (B1): %1").arg(sm2_state_name()));
    ui->label_sm3->setText(QString("SM3 (V2): %1").arg(sm3_state_name()));
    ui->label_sm4->setText(QString("SM4 (R1): %1").arg(sm4_state_name()));
}
