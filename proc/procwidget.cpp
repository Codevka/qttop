#include "procwidget.h"
#include "ui_procwidget.h"
#include "process.h"
#include <QStandardItemModel>


procwidget::procwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::procwidget)
{
    ui->setupUi(this);

    model = new QStandardItemModel(this);
    model->setColumnCount(5);

    QStringList headers;
    headers << "PID" << "name" << "CPU" << "mem" << "power";
    model->setHorizontalHeaderLabels(headers);

    Shared::init();

    ui->tableView->setModel(model);

    this->timer.start(1000 * time_rate);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_proc()));
}

procwidget::~procwidget()
{
    delete ui;
}

void procwidget::timer_update_proc() {
    auto processes = Proc::get_processes(time_rate);
    for (size_t i = 0; i < processes.size(); i++)
    {
        const auto process = processes[i];
        QString cpu; cpu.sprintf("%.1lf%%", process.cpu_p);
        QString mem; mem.sprintf("%.1lfMB", process.memory * 1.0 / (1 << 20));
        QString power; power.sprintf("%.1lfmW", process.power);
        model->setItem(i, 0, new QStandardItem(QString::number(process.pid)));
        model->setItem(i, 1, new QStandardItem(QString::fromStdString(process.name)));
        model->setItem(i, 2, new QStandardItem(cpu));
        model->setItem(i, 3, new QStandardItem(mem));
        model->setItem(i, 4, new QStandardItem(power));
    }
}
