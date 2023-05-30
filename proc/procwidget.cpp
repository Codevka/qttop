#include "procwidget.h"
#include "ui_procwidget.h"
#include "process.h"
#include <QStandardItemModel>
#include <QMenu>
#include <QAction>
#include <iostream>
#include <signal.h>


procwidget::procwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::procwidget)
{
    ui->setupUi(this);

    model = new QStandardItemModel(this);
    model->setColumnCount(5);

    QStringList headers;
    headers << "PID" << "进程名" << "CPU占用率" << "内存" << "功耗";
    model->setHorizontalHeaderLabels(headers);

    Shared::init();

    timer_update_proc();

    ui->tableView->setModel(model);

    this->timer.start(1000 * time_rate);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_update_proc()));
}

procwidget::~procwidget()
{
    delete ui;
}

void procwidget::timer_update_proc() {
    processes = Proc::get_processes(time_rate);
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

void procwidget::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->tableView->indexAt(pos);
    if (index.isValid()) {
        int pid = processes[index.row()].pid;
        std::cout << pid << '\n';
        QMenu menu;
        QAction *stopProc = menu.addAction("挂起进程");
        QAction *termProc = menu.addAction("结束进程");
        QAction *killProc = menu.addAction("强制退出");
        connect(stopProc, &QAction::triggered, [=](){
            Proc::send_signal(pid, SIGSTOP);
        });
        connect(termProc, &QAction::triggered, [=](){
            Proc::send_signal(pid, SIGTERM);
        });
        connect(killProc, &QAction::triggered, [=](){
            Proc::send_signal(pid, SIGKILL);
        });
        menu.exec(QCursor::pos());
    }
}
