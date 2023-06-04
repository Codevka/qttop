#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include<QChartView>
#include<QGraphicsView>
#include<QMouseEvent>
#include<QWheelEvent>
#include<QChart>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include<QGraphicsLineItem>
#include<QChart>
#include<QTimer>
#include <QValueAxis>



QT_CHARTS_USE_NAMESPACE


class netchart :public QChartView
{
public:
    netchart(QWidget *parent = 0);
    QLineSeries * r_series;
    QLineSeries * s_series;
    void handle(long long  y_s,long long  y_r);

private:
    QChart* m_chart;
    QValueAxis* m_axisX;
    QValueAxis* m_axisY;
    QTimer m_timer;
    int m_x;
    int m_max;
    bool m_isTouching;
    long init=1;
    long last_init=1;
    qreal ymax=1;
};



#endif // NETCHART_H
