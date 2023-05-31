#ifndef PROCWIDGET_H
#define PROCWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QStandardItemModel>
#include <vector>
#include "process.h"

namespace Ui {
class procwidget;
}

class procwidget : public QWidget
{
    Q_OBJECT

public:
    explicit procwidget(QWidget *parent = nullptr);
    ~procwidget();

public slots:
    void timer_update_proc(void);

private slots:
    void on_tableView_customContextMenuRequested(const QPoint &pos);

private:
    Ui::procwidget *ui;
    QStandardItemModel *model;

    QTimer timer;
    double time_rate = 3;

    std::vector<Proc::ProcessInfo> processes;
};

#endif // PROCWIDGET_H
