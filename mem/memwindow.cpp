#include "memwindow.h"
#include "ui_memwindow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

mem *memInfo;
MemWindow::MemWindow(QWidget *parent) : QWidget(parent),ui(new Ui::MemWindow),m_x(0),m_max(50)
{
    ui->setupUi(this);
    memInfo = new mem();
    memInfo->mem_free = -1;
    memInfo->mem_total = -1;
    memInfo->mem_buffers = -1;
    memInfo->mem_cached = -1;
    memInfo->mem_used = -1.0;
    memInfo->maxLength = 10;
    for (int i = 0; i < memInfo->maxLength; ++i) {
            memInfo->mem_useds[i] = 0;
        }
    memInfo->nowLength = -1;

    this->timer.start(1000 * time_rate);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(thread_mem()));
    // connect(&this->timer,SIGNAL(timeout()),this,SLOT(ChartView::handleTimeout()));
    // QVector<QPointF> points=m_series->pointsVector();
    // points.append(QPointF(m_x,y));
    // ui->graphicsView->m_series->pointsVector().append(QPointF(0, 10));
    // ui->graphicsView->m_series->pointsVector().append(QPointF(1, 10));
    // ui->graphicsView->m_series->pointsVector().append(QPointF(2, 10));
    // pthread_t thread_mem_id;
    // pthread_create(&thread_mem_id, NULL, MemWindow::thread_mem, memInfo);
}

MemWindow::~MemWindow()
{
    delete ui;
}
// int GetSysMemInfo(mem *memInfo)
int MemWindow::timer_update_mem()
{ //获取系统当前可用内存
    char name[20];

    FILE *fp;
    char buf1[128], buf2[128], buf3[128], buf4[128], buf5[128];
    int buff_len = 128;
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        std::cerr << "GetSysMemInfo() error! file not exist" << std::endl;
        return -1;
    }
    if (NULL == fgets(buf1, buff_len, fp) ||
        NULL == fgets(buf2, buff_len, fp) ||
        NULL == fgets(buf3, buff_len, fp) ||
        NULL == fgets(buf4, buff_len, fp) ||
        NULL == fgets(buf5, buff_len, fp))
    {
        std::cerr << "GetSysMemInfo() error! fail to read!" << std::endl;
        fclose(fp);
        return -1;
    }
    fclose(fp);
    sscanf(buf1, "%s%d", name, &(memInfo->mem_total));
    sscanf(buf2, "%s%d", name, &(memInfo->mem_free));
    sscanf(buf4, "%s%d", name, &(memInfo->mem_buffers));
    sscanf(buf5, "%s%d", name, &(memInfo->mem_cached));
    memInfo->mem_used = (memInfo->mem_total - memInfo->mem_free) * (float)1.0 / memInfo->mem_total;

    return 0;
}

void MemWindow::print_array(float arr[],int n)
{
    for (size_t i = 0; i < n; i++) {
        std::cout << arr[i] << ' ';
    }
}

enum Unit { USE_KB, USE_MB, USE_GB };

QString gen_mem_str(int num)
{
    Unit u = USE_KB;
    int val1 = num, val2 = 0;
    if (num >= 1024 * 1024) {
        u = USE_GB;
        val1 = num / (1024 * 1024);
        val2 = (num * 10 / (1024 * 1024)) % 10;
    } else if (num >= 1024) {
        u = USE_MB;
        val1 = num / 1024;
        val2 = (num * 10 / 1024) % 10;
    }
    switch (u) {
    case USE_KB:
        return QString("%1.%2 KB").arg(val1).arg(val2);
    case USE_MB:
        return QString("%1.%2 MB").arg(val1).arg(val2);
    case USE_GB:
        return QString("%1.%2 GB").arg(val1).arg(val2);
    default:
        return QString("Error");
    }
}

void MemWindow::thread_mem()
{
    // mem *memInfo = reinterpret_cast<mem *>(arg);
    // printf("%s\n", __FUNCTION__);
    // while (1)
    // {
        int ret = MemWindow::timer_update_mem();
        if (ret == -1)
        {
            std::cerr << "get meminfo error" << std::endl;
        }
        else
        {
            memInfo->nowLength++;
            // if(memInfo->nowLength<memInfo->maxLength){
            //     memInfo->mem_useds[memInfo->nowLength] = memInfo->mem_used;
            // }
            // else
            // {
                int i;
                for (i = 0; i < memInfo->maxLength - 1; ++i)
                {
                    memInfo->mem_useds[i] = memInfo->mem_useds[i++];
                }
                memInfo->mem_useds[memInfo->maxLength-1] = memInfo->mem_used;
            // }
                // MemWindow::showChart(memInfo->nowLength, (int)memInfo->mem_used);
                cout << "空闲内存量: " << memInfo->mem_free << "kb\n" << endl;
                cout << "内存总量: " << memInfo->mem_total << "kb\n" << endl;
                cout << "块设备占用内存: " << memInfo->mem_buffers << "kb\n" << endl;
                cout << "文件页: " << memInfo->mem_cached << "\n" << endl;
                cout << "内存使用量: " << memInfo->mem_used << "kb\n" << endl;
                MemWindow::print_array(memInfo->mem_useds, memInfo->maxLength);
                cout << "\n" << endl;
                ui->label_mem_free_name->setText(gen_mem_str(memInfo->mem_free));
                ui->label_mem_total_name->setText(gen_mem_str(memInfo->mem_total));
                ui->label_mem_buffers_name->setText(gen_mem_str(memInfo->mem_buffers));
                ui->label_mem_cached_name->setText(gen_mem_str(memInfo->mem_cached));
                ui->label_mem_used_name->setText(QString::number(memInfo->mem_used * 100, 'f', 1)
                                                 + "%");
                ui->graphicsView->handleTimeout((memInfo->mem_used*100));
        }

    //     sleep(3);
    // }
}
//  https://mirrors.tuna.tsinghua.edu.cn/qt/online/qtsdkrepository/linux_x64/desktop/qt5_5128/
//
void MemWindow::showChart(int m_x,int y){
    if(ui->graphicsView->isVisible())
    {//在可视的情况下刷新数据
        // QVector<QPointF> points=m_series->pointsVector();
        // points.append(QPointF(m_x,y));

        // if(points.size()>m_max){//达到限值
        //     points.pop_front();
        // }
        // m_series->replace(points);

        // if(m_x>60)
        // {
        //     m_chart->axisX()->setRange(m_x-59,m_x);
        // }
    }
}
