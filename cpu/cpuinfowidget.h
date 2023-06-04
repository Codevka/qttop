#ifndef CPUINFOWIDGET_H
#define CPUINFOWIDGET_H

#include <QLabel>
#include <QTimer>
#include <QWidget>
#include "CpuInfoHandler.h"
#include <memory>

namespace Ui {
class CpuInfoWidget;
}

class CpuInfoWidget : public QWidget
{
    Q_OBJECT

public:
    std::shared_ptr<CpuInfoHandler> cpu_info_handler;

    QTimer timer;
    double time_rate = 1.0;

    QVector<QLabel *> labels;

public:
    explicit CpuInfoWidget(QWidget *parent = nullptr);
    ~CpuInfoWidget();

public slots:
    void timer_update_cpu_hz(void);
    void timer_update_cpu_collect(void);
    void timer_update_cpu_graph(void);
    void timer_update_core_percent(void);

private:
    Ui::CpuInfoWidget *ui;
};

#endif // CPUINFOWIDGET_H
