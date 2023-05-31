#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include "network.h"
#include <QTimer>
#include <QWidget>
#include <memory>

namespace Ui {
    class networkwidget;
}

class networkwidget : public QWidget
{
    Q_OBJECT

public:

    QTimer timer;
    double time_rate = 1.5;



private:
    Ui::networkwidget *ui;
};

#endif // CPUINFOWIDGET_H
