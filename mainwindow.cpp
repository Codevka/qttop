#include "mainwindow.h"
#include "cpu/cpuinfowidget.h"
#include "mem/memwindow.h"
#include "proc/procwidget.h"
#include "ui_mainwindow.h"

#include <QTabBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->tabBar()->setDocumentMode(true);
    ui->tabWidget->tabBar()->setExpanding(true);

    auto cpuLayout     = new QVBoxLayout;
    auto cpuInfoWidget = new CpuInfoWidget;
    cpuLayout->addWidget(cpuInfoWidget);
    ui->cpu_tab->setLayout(cpuLayout);

    auto memLayout     = new QVBoxLayout;
    auto memWindow = new MemWindow;
    memLayout->addWidget(memWindow);
    ui->memory_tab->setLayout(memLayout);

    auto procLayout     = new QVBoxLayout;
    auto proc_widget = new procwidget;
    procLayout->addWidget(proc_widget);
    ui->process_tab->setLayout(procLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}
