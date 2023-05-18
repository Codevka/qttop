#ifndef CPUINFOWIDGET_H
#define CPUINFOWIDGET_H

#include <QWidget>

namespace Ui {
    class CpuInfoWidget;
}

class CpuInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CpuInfoWidget(QWidget *parent = nullptr);
    ~CpuInfoWidget();

private:
    Ui::CpuInfoWidget *ui;
};

#endif // CPUINFOWIDGET_H
