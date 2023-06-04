#ifndef MEMWINDOW_H
#define MEMWINDOW_H

#include <QTimer>
#include <QTimer>
#include <QWidget>
#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MemWindow;
}

typedef struct memI
{
    int mem_free;
    int mem_total;
    int mem_buffers;
    int mem_cached;
    float mem_used;
    float mem_useds[10];
    int maxLength;
    int nowLength;
    /* data */
} mem;

class MemWindow : public QWidget
{
    Q_OBJECT

public:
    QTimer timer;
    double time_rate = 1.5;

public:
    explicit MemWindow(QWidget *parent = nullptr);
    ~MemWindow();

private slots:
    void thread_mem(void);
    int timer_update_mem(void);
    void print_array(float arr[], int n);
    void showChart(int x, int y);

private:
    Ui::MemWindow *ui;
    QLineSeries *m_series;
    QChart *m_chart;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
    QTimer m_timer;
    int m_x;
    int m_max;
};

#endif // MEMWINDOW_H
