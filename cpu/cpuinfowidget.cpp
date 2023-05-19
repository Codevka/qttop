#include "cpuinfowidget.h"
#include "ui_cpuinfowidget.h"
#include "CpuInfoHandler.h"

#include <QDebug>
#include <QTimer>

CpuInfoWidget::CpuInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CpuInfoWidget)
{
    ui->setupUi(this);
    this->cpu_info_handler = CpuInfoHandler::getInstance();
    ui->label_cpu_name->setText(this->cpu_info_handler->cpu_name);
    // FIXME: There maybe some bug with QT Designer.
    ui->label_cpu_name->setAlignment(Qt::AlignCenter);
    ui->label_cpu_hz->setText(this->cpu_info_handler->cpu_hz);
    ui->label_cpu_hz->setAlignment(Qt::AlignCenter);

    this->timer.start(1000 * time_rate);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_cpu_hz()));
}

CpuInfoWidget::~CpuInfoWidget()
{
    delete ui;
}

void CpuInfoWidget::timer_update_cpu_hz() {
    this->cpu_info_handler->updateCpuHz();
    ui->label_cpu_hz->setText(this->cpu_info_handler->cpu_hz);
}
