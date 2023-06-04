#ifndef CHARTVIEW_H
#define CHARTVIEW_H

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
#include "callout.h"

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QWidget *parent = 0);

    qreal getYValue(QPointF p1, QPointF p2, qreal x);

protected:
    bool viewportEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event);

public slots:
    void connectMarkers();
    void handleMarkerClicked();

    void tooltip(QPointF point, bool state);
    void handleTimeout(int y);
    void setKbUnit();

private:
    Callout *m_tooltip;
    QChart *m_chart;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
    QTimer m_timer;
    int m_x;
    int m_max;
    bool m_isTouching;
    QString unit;
    int type;

public:
    bool m_isBuoyShow;
    QLineSeries *m_series;
};

#endif // CHARTVIEW_H
