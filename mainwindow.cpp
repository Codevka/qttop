#include "mainwindow.h"
#include "cpu/cpuinfowidget.h"
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
}

MainWindow::~MainWindow()
{
    delete ui;
}
