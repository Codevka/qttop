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
#include"callout.h"
#include<QChart>
#include<QTimer>
#include <QValueAxis>


QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QWidget *parent = 0);

    qreal getYValue(QPointF p1,QPointF p2,qreal x);


protected:
    bool viewportEvent(QEvent *event)override;
    void resizeEvent(QResizeEvent *event);

public slots:
    void connectMarkers();
    void handleMarkerClicked();

    void tooltip(QPointF point, bool state);
    void handleTimeout(int y);

private:
    Callout* m_tooltip;
    QChart* m_chart;
    QValueAxis* m_axisX;
    QValueAxis* m_axisY;
    QTimer m_timer;
    int m_x;
    int m_max;
    bool m_isTouching;


public:
    bool m_isBuoyShow;
    QLineSeries* m_series;

};

#endif // CHARTVIEW_H
