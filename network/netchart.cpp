#include "netchart.h"
#include<QDebug>
#include<inttypes.h>
netchart::netchart(QWidget *parent)
   : QChartView(parent)
   ,m_x(0)
   ,m_max(80)
{
    m_chart=new QChart;
    r_series=new QSplineSeries;
    s_series=new QSplineSeries;
    r_series->append(-1, 0);
    r_series->setName("r");
    s_series->append(-1, 0);
    s_series->setName("s");
    r_series->setPen(QPen(Qt::blue,2.5,Qt::SolidLine));
    s_series->setPen(QPen(Qt::red,2.5,Qt::SolidLine));

    m_axisX=new QValueAxis;
    m_axisX->setRange(0, m_max);
    m_axisX->setLineVisible(false);
    m_axisX->setTickCount(11);     //标记的个数
    m_axisX->setMinorTickCount(5); //次标记的个数
    m_axisX->setLabelsVisible(false);

    m_axisY=new QValueAxis;

    m_axisY->setRange(0,ymax);
    m_axisY->setGridLineVisible(true);
    m_axisY->setTickCount(6);
    m_axisY->setMinorTickCount(2);


    m_chart->addSeries(r_series);
    m_chart->addSeries(s_series);
    m_chart->createDefaultAxes();
    m_chart->setAxisX(m_axisX,r_series);
    m_chart->setAxisY(m_axisY,r_series);
    m_chart->setAxisX(m_axisX,s_series);
    m_chart->setAxisY(m_axisY,s_series);

    m_chart->legend()->hide();

    this->setChart(m_chart);
    this->setRenderHint(QPainter::Antialiasing);

    connect(r_series, &QSplineSeries::pointAdded, [=](){
        int y_max=0;
        for(int i=0;i<r_series->count();i++){
            if(r_series->at(i).y()>y_max)
                y_max=r_series->at(i).y();
            if(s_series->at(i).y()>y_max)
                y_max=s_series->at(i).y();
        }
        if(y_max<1)
            y_max=1;
        m_axisY->setRange(0,y_max*1.1);
    });
}

void netchart::handle(long long  y_s,long long y_r)
{   s_series->append(m_x,y_s/1024/1024);
    r_series->append(m_x,y_r/1024/1024);
    if(r_series->count()>m_max){
        r_series->remove(0);
        s_series->remove(0);
         }
    m_chart->axisX()->setRange(m_x - (m_max - 1), m_x);
    m_x++;
}
