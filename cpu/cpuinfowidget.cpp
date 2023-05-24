#include "CpuInfoHandler.h"
#include "cpuinfowidget.h"
#include "ui_cpuinfowidget.h"

#include <QDebug>
#include <QTimer>

CpuInfoWidget::CpuInfoWidget(QWidget *parent) : QWidget(parent), ui(new Ui::CpuInfoWidget) {
    ui->setupUi(this);
    this->cpu_info_handler = CpuInfoHandler::getInstance();
    ui->label_cpu_name->setText(this->cpu_info_handler->cpu_name);
    ui->label_cpu_hz->setText(this->cpu_info_handler->cpu_hz);

    QString cpu_load_avg = QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[0], 'f', 2) + " "
                           + QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[1], 'f', 2) + " "
                           + QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[2], 'f', 2);
    ui->label_cpu_load_avg->setText(cpu_load_avg);

    this->timer.start(1000 * time_rate);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_cpu_hz()));
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_cpu_collect()));
}

CpuInfoWidget::~CpuInfoWidget() {
    delete ui;
}

void CpuInfoWidget::timer_update_cpu_hz() {
    this->cpu_info_handler->updateCpuHz();
    ui->label_cpu_hz->setText(this->cpu_info_handler->cpu_hz);
}

void CpuInfoWidget::timer_update_cpu_collect() {
    this->cpu_info_handler->collectCpuInfo();
    QString cpu_load_avg = QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[0], 'f', 2) + " "
                           + QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[1], 'f', 2) + " "
                           + QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[2], 'f', 2);
    ui->label_cpu_load_avg->setText(cpu_load_avg);
}
