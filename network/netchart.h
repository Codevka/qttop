#ifndef NETCHART_H
#define NETCHART_H

#include <QApplication>
#include <QChart>
#include <QChartView>
#include <QGraphicsLineItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QTimer>
#include <QValueAxis>
#include <QWheelEvent>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtWidgets/QGraphicsScene>

QT_CHARTS_USE_NAMESPACE

class netchart : public QChartView
{
    Q_OBJECT

public:
    netchart(QWidget *parent = 0);
    virtual ~netchart() {}

    QLineSeries *r_series;
    QLineSeries *s_series;
    void handle(long long y_s, long long y_r);

private:
    QChart *m_chart;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
    QTimer m_timer;
    int m_x;
    int m_max;
    bool m_isTouching;
    long init = 1;
    long last_init = 1;
    qreal ymax = 1;
};

#endif // NETCHART_H
