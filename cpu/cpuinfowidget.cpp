#include "cpuinfowidget.h"
#include "CpuInfoHandler.h"
#include "ui_cpuinfowidget.h"

#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

QString get_core_color(double val)
{
    // color inspired by Vuetify
    if (val <= 50) {
        return QString("QLabel { color: #009688; }");
    } else if (val <= 80) {
        return QString("QLabel { color: #FFA000; }");
    } else {
        return QString("QLabel { color: #C62828; }");
    }
}

CpuInfoWidget::CpuInfoWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CpuInfoWidget)
{
    ui->setupUi(this);
    this->cpu_info_handler = CpuInfoHandler::getInstance();
    {
        ui->label_cpu_name->setText(this->cpu_info_handler->cpu_name);
        ui->label_cpu_hz->setText(QString("%1").arg(this->cpu_info_handler->cpu_hz));
        ui->label_cpu_percent->setText(QString("%1.%2\%")
                                           .arg(this->cpu_info_handler->cur_cpu.cpu_percent / 10)
                                           .arg(this->cpu_info_handler->cur_cpu.cpu_percent % 10));
        ui->label_load_avg_1->setText(
            QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[0], 'f', 2));
        ui->label_load_avg_5->setText(
            QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[1], 'f', 2));
        ui->label_load_avg_10->setText(
            QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[2], 'f', 2));
    }

    // Grid layout for each core
    auto core_grid_layout = new QGridLayout;
    ui->core_frame->setLayout(core_grid_layout);
    this->labels = QVector<QLabel *>();
    for (uint32_t i = 0; i < this->cpu_info_handler->core_num; i++) {
        QLabel *label_name = new QLabel(this);
        QLabel *label_value = new QLabel(this);
        this->labels.push_back(label_name);
        this->labels.push_back(label_value);
        label_name->setText(QString("CPU%1: ").arg(i));
        label_value->setText(
            QString("%2\%").arg(this->cpu_info_handler->cur_cpu.core_percent[i].last()));

        core_grid_layout->addWidget(label_name, i / 4, (i % 4) * 4);
        core_grid_layout->addWidget(label_value, i / 4, (i % 4) * 4 + 1);

        if (this->cpu_info_handler->core_num >= 4) {
            QLabel *empty_label = new QLabel(this);
            core_grid_layout->addWidget(empty_label, i / 4, (i % 4) * 4 + 2);
            core_grid_layout->addWidget(empty_label, i / 4, (i % 4) * 4 + 3);
        }

        label_name->setStyleSheet("QLabel { color: dimgrey; }");
        label_value->setStyleSheet(
            get_core_color(this->cpu_info_handler->cur_cpu.core_percent[i].last()));
    }

    this->timer.start(1000 * time_rate);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_cpu_hz()));
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_cpu_collect()));
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_cpu_graph()));
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_core_percent()));
}
CpuInfoWidget::~CpuInfoWidget()
{
    delete ui;
}

void CpuInfoWidget::timer_update_cpu_hz()
{
    this->cpu_info_handler->updateCpuHz();
    ui->label_cpu_hz->setText(QString("基准频率：%1").arg(this->cpu_info_handler->cpu_hz));
}

void CpuInfoWidget::timer_update_cpu_collect()
{
    this->cpu_info_handler->collectCpuInfo();
    ui->label_cpu_hz->setText(QString("%1").arg(this->cpu_info_handler->cpu_hz));
    ui->label_load_avg_1->setText(
        QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[0], 'f', 2));
    ui->label_load_avg_5->setText(
        QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[1], 'f', 2));
    ui->label_load_avg_10->setText(
        QString::number(this->cpu_info_handler->cur_cpu.load_avg_1_5_15[2], 'f', 2));
    ui->label_cpu_percent->setText(QString("%1.%2\%")
                                       .arg(this->cpu_info_handler->cur_cpu.cpu_percent / 10)
                                       .arg(this->cpu_info_handler->cur_cpu.cpu_percent % 10));
}

void CpuInfoWidget::timer_update_cpu_graph()
{
    ui->cpu_graphicsView->handleTimeout(this->cpu_info_handler->cur_cpu.cpu_percent / 10);
}

void CpuInfoWidget::timer_update_core_percent()
{
    for (uint32_t i = 0; i < this->cpu_info_handler->core_num; i++) {
        this->labels[i * 2]->setText(QString("CPU%1: ").arg(i));
        this->labels[i * 2 + 1]->setText(
            QString("%2\%").arg(this->cpu_info_handler->cur_cpu.core_percent[i].last()));
        this->labels[i * 2 + 1]->setStyleSheet(
            get_core_color(this->cpu_info_handler->cur_cpu.core_percent[i].last()));
    }
}
