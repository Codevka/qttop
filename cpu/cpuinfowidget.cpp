#include "CpuInfoHandler.h"
#include "cpuinfowidget.h"
#include "ui_cpuinfowidget.h"

#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

CpuInfoWidget::CpuInfoWidget(QWidget *parent) : QWidget(parent), ui(new Ui::CpuInfoWidget) {
    ui->setupUi(this);
    this->cpu_info_handler = CpuInfoHandler::getInstance();
    {
        ui->label_cpu_name->setText(this->cpu_info_handler->cpu_name);
        ui->label_cpu_hz->setText(QString("基准频率：%1").arg(this->cpu_info_handler->cpu_hz));
        ui->label_cpu_percent->setText(QString("利用率：%1.%2\%")
                                           .arg(this->cpu_info_handler->cur_cpu.cpu_percent / 10)
                                           .arg(this->cpu_info_handler->cur_cpu.cpu_percent % 10));

        QString cpu_load_avg = QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[0], 'f', 2) + " "
                               + QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[1], 'f', 2) + " "
                               + QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[2], 'f', 2);
        ui->label_cpu_load_avg->setText(QString("平均负载（1, 5, 15分钟内）：%1").arg(cpu_load_avg));
    }

    // Grid layout for each core
    this->labels = QVector<QLabel *>();
    for (uint32_t i = 0; i < this->cpu_info_handler->core_num; i++) {
        QLabel *label = new QLabel(this);
        this->labels.push_back(label);
        label->setText(QString("CPU%1: %2\%").arg(i).arg(this->cpu_info_handler->cur_cpu.core_percent[i].last()));
        label->setAlignment(Qt::AlignCenter);
        ui->gridLayout->addWidget(label, i / 4, i % 4);
    }

    this->timer.start(1000 * time_rate);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_cpu_hz()));
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_cpu_collect()));
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_cpu_graph()));
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_core_percent()));
}
CpuInfoWidget::~CpuInfoWidget() {
    delete ui;
}

void CpuInfoWidget::timer_update_cpu_hz() {
    this->cpu_info_handler->updateCpuHz();
    ui->label_cpu_hz->setText(QString("基准频率：%1").arg(this->cpu_info_handler->cpu_hz));
}

void CpuInfoWidget::timer_update_cpu_collect() {
    this->cpu_info_handler->collectCpuInfo();
    QString cpu_load_avg = QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[0], 'f', 2) + "  "
                           + QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[1], 'f', 2) + "  "
                           + QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[2], 'f', 2);
    ui->label_cpu_load_avg->setText(QString("平均负载（1、5、15分钟内）：%1").arg(cpu_load_avg));
    ui->label_cpu_percent->setText(QString("利用率：%1.%2\%")
                                       .arg(this->cpu_info_handler->cur_cpu.cpu_percent / 10)
                                       .arg(this->cpu_info_handler->cur_cpu.cpu_percent % 10));
}

void CpuInfoWidget::timer_update_cpu_graph() {
    ui->cpu_graphicsView->handleTimeout(this->cpu_info_handler->cur_cpu.cpu_percent / 10);
}

void CpuInfoWidget::timer_update_core_percent() {
    for (uint32_t i = 0; i < this->cpu_info_handler->core_num; i++) {
        this->labels[i]->setText(
            QString("CPU%1: %2\%").arg(i).arg(this->cpu_info_handler->cur_cpu.core_percent[i].last()));
    }
}
