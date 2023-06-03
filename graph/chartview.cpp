#include "chartview.h"
#include<QDebug>
#include<QGraphicsView>
#include <QLegendMarker>
#include <QQueue>
#include <QtCore/QtMath>
#include<QApplication>
#include<QCategoryAxis>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

#include <iostream>
#include <sys/sysinfo.h>
//多线程处理库
#include <pthread.h>
//网络监控部分
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/time.h>
using namespace std;

ChartView::ChartView(QWidget *parent)
    : QChartView(parent)
    , m_tooltip(0)
    , m_isTouching(false)
    , m_isBuoyShow(true)
    , m_x(0)
    , m_max(80)
{
    m_series=new QLineSeries;
    m_chart=new QChart;
    m_chart->setBackgroundVisible(false);

    m_series=new QLineSeries;
    m_series->append(-1, 0);
    m_series->setName("ss");
    m_chart->addSeries(m_series);
    m_chart->createDefaultAxes();

    m_axisX=new QValueAxis;
    m_axisX->setRange(0, m_max);
    m_axisX->setLineVisible(false);
    m_axisX->setTickCount(11);     //标记的个数
    m_axisX->setMinorTickCount(5); //次标记的个数
    m_axisX->setLabelsVisible(false);

    m_axisY=new QValueAxis;
    m_axisY->setRange(0,100);
    m_axisY->setGridLineVisible(true);
    m_axisY->setTickCount(6);
    m_axisY->setMinorTickCount(2);
    

    m_chart->setAxisX(m_axisX,m_series);
    m_chart->setAxisY(m_axisY,m_series);

    m_chart->legend()->hide();

    this->setChart(m_chart);
    this->setRenderHint(QPainter::Antialiasing);
    // this->setRubberBand(QChartView::RectangleRubberBand);

    // m_timer.setInterval(1000);
    // m_timer.start();
    // connect(&m_timer,SIGNAL(timeout()),this,SLOT(handleTimeout()));

    connectMarkers();

    //浮框
    connect(m_series, SIGNAL(hovered(QPointF, bool)), this, SLOT(tooltip(QPointF,bool)));

    this->setMouseTracking(true);

    type = 0;
    unit = '%';
}

qreal ChartView::getYValue(QPointF p1, QPointF p2, qreal x)
{
    qreal y=(p2.y()-p1.y())/(p2.x()-p1.x())*(x-p1.x())+p1.y();
    return y;

}

bool ChartView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {

        m_isTouching = true;

        chart()->setAnimationOptions(QChart::NoAnimation);
    }
    return QChartView::viewportEvent(event);
}

void ChartView::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
         m_chart->resize(event->size());
    }
    QChartView::resizeEvent(event);
}



void ChartView::connectMarkers()
{
    foreach (QLegendMarker* marker, m_chart->legend()->markers()) {
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }

}

void ChartView::handleMarkerClicked()
{

    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
    Q_ASSERT(marker);

    switch (marker->type())

    {
        case QLegendMarker::LegendMarkerTypeXY:
        {

        marker->setVisible(true);

        qreal alpha;

        QColor color;
        QPen pen = marker->pen();
        color = pen.color();

        if(color.alphaF()==1.0){
            //未点击
            alpha=0.2;

            QFont font=marker->font();
            font.setBold(true);
            marker->setFont(font);

        }else {
            //已点击
            alpha=1.0;

            QFont font=marker->font();
            font.setBold(false);
            marker->setFont(font);
        }

        QList<QAbstractSeries *> seriesList=m_chart->series();

        for(int i=0;i<seriesList.size();i++){

           QLineSeries* series=(QLineSeries*)seriesList.at(i);
           if(series!=marker->series()){
               QPen seriesPen=series->pen();
               QColor color=seriesPen.color();
               color.setAlphaF(alpha);
               seriesPen.setColor(color);
               series->setPen(seriesPen);


           }
        }

        color.setAlphaF(alpha);
        pen.setColor(color);
        marker->setPen(pen);



        break;
        }
    default:
        {
        qDebug() << "Unknown marker type";
        break;
        }
    }


}

void ChartView::tooltip(QPointF point, bool state)
{
    if (m_tooltip == 0)
        m_tooltip = new Callout(m_chart);

    if (state) {
        m_tooltip->setText(QString("时刻: %1s \n使用情况: %2 %3 ").arg((int)point.x()).arg(point.y()).arg(unit));//.arg(unit)
        // if(type)
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}

void ChartView::handleTimeout(int y)
{

    QVector<QPointF> points = m_series->pointsVector();

    if(y>100&&y<1000&&type<1){
        type = 1;
        m_chart->axisY()->setRange(0,1000);
    }else if(y>1000&&type<2){
        type = 2;
        m_chart->axisY()->setRange(0,5);
        unit = "MB";
        QVector<QPointF> points_old = m_series->pointsVector();
        points =QVector<QPointF>();
        while(!points_old.empty()){
            QPointF point = (points_old.front());
            points_old.pop_front();
            points.append(QPointF(point.x(), (float)point.y()/1024));
        }
    }

    if(type==2){
        points.append(QPointF(m_x, (float)y/1024));
    }
    else 
        points.append(QPointF(m_x, y));

    if (points.size() > m_max) { //达到限值
        points.pop_front();
    }

    m_series->replace(points);
    m_chart->axisX()->setRange(m_x - (m_max - 1), m_x);

    m_x++;
}
void ChartView::setKbUnit()
{
    unit = "KB";
}
