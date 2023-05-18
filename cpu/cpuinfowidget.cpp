#include "cpuinfowidget.h"
#include "ui_cpuinfowidget.h"

#include "CpuInfoHandler.h"

CpuInfoWidget::CpuInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CpuInfoWidget)
{
    ui->setupUi(this);
    auto    cpu_info_handler = new CpuInfoHandler;
    QString cpu_name;
    try {
        cpu_name = cpu_info_handler->getCpuName();
    } catch (QString e) {
        cpu_name = e;
    }
    ui->label->setText(cpu_name);
    // FIXME: There maybe some bug with QT Designer.
    ui->label->setAlignment(Qt::AlignCenter);
}

CpuInfoWidget::~CpuInfoWidget()
{
    delete ui;
}
