#ifndef PROCWIDGET_H
#define PROCWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QStandardItemModel>

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

private:
    Ui::procwidget *ui;
    QStandardItemModel *model;
    QTimer timer;
    double time_rate = 1;
};

#endif // PROCWIDGET_H
