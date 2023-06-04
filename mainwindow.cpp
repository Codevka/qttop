#include "mainwindow.h"
#include "cpu/cpuinfowidget.h"
#include "mem/memwindow.h"
#include "network/netwidget.h"
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
    ui->tabWidget->setStyleSheet("QTabWidget#tabwidget { background-color: rgb(243, 243, 243); } \
                                  QTabWidget::tar-bar { alignment: center; } \
                                  QTabWidget::pane { border: none; } \
                                  QTabBar::tab { background-color: rgb(243, 243, 243); padding: 8px; } \
                                  QTabBar::tab:first { border-top-left-radius: 8px; border-bottom-left-radius: 8px; } \
                                  QTabBar::tab:last { border-top-right-radius: 8px; border-bottom-right-radius: 8px; } \
                                  QTabBar::tab:selected { background-color: #039BE5; color: white; } \
                                  QTabBar::tab:hover:!selected { background-color: #4FC3F7; color: #FAFAFA; } \
                                 ");

    auto cpuLayout = new QVBoxLayout;
    auto cpuInfoWidget = new CpuInfoWidget;
    cpuLayout->addWidget(cpuInfoWidget);

    auto networklayout = new QVBoxLayout;
    auto networkwidget = new netwidget;
    networklayout->addWidget(networkwidget);

    ui->cpu_tab->setLayout(cpuLayout);
    ui->network_tab->setLayout(networklayout);

    auto memLayout = new QVBoxLayout;
    auto memWindow = new MemWindow;
    memLayout->addWidget(memWindow);
    ui->memory_tab->setLayout(memLayout);

    auto procLayout = new QVBoxLayout;
    auto proc_widget = new procwidget;
    procLayout->addWidget(proc_widget);
    ui->process_tab->setLayout(procLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}
