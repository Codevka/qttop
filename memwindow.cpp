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

mem *meminfo;
MemWindow::MemWindow(QWidget *parent) : QDialog(parent),
                                        ui(new Ui::MemWindow)
{
    ui->setupUi(this);
    meminfo = new mem();
    meminfo->mem_free = -1;
    meminfo->mem_total = -1;
    meminfo->mem_buffers = -1;
    meminfo->mem_cached = -1;
    meminfo->mem_used = -1.0;
    meminfo->maxLength = 10;
    for (int i = 0; i < meminfo->maxLength; ++i) {
            meminfo->mem_useds[i] = 0;
        }
    meminfo->nowLength = -1;
    pthread_t thread_mem_id;
    pthread_create(&thread_mem_id, NULL, MemWindow::thread_mem, meminfo);
}

MemWindow::~MemWindow()
{
    delete ui;
}
int GetSysMemInfo(mem *memInfo)
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

void print_array(float arr[],int n)
{
    for (size_t i = 0; i < n; i++) {
        std::cout << arr[i] << ' ';
    }
}

void *MemWindow::thread_mem(void *arg)
{
    mem *memInfo = reinterpret_cast<mem *>(arg);
    printf("%s\n", __FUNCTION__);
    while (1)
    {
        int ret = GetSysMemInfo(memInfo);
        if (ret == -1)
        {
            std::cerr << "get meminfo error" << std::endl;
        }
        else
        {
            meminfo->nowLength++;
            if(meminfo->nowLength<meminfo->maxLength){
                meminfo->mem_useds[meminfo->nowLength] = memInfo->mem_used;
            }
            else
            {
                int i;
                for (i = 0; i < meminfo->maxLength - 1; ++i)
                {
                    meminfo->mem_useds[i] = meminfo->mem_useds[i++];
                }
                meminfo->mem_useds[meminfo->maxLength-1] = memInfo->mem_used;
            }
                    cout
                << "mem_free: " << memInfo->mem_free << "\n"
                << endl;
            cout << "mem_total: " << memInfo->mem_total << "\n"
                 << endl;
            cout << "mem_buffers: " << memInfo->mem_buffers << "\n"
                 << endl;
            cout << "mem_cached: " << memInfo->mem_cached << "\n"
                 << endl;
            cout << "mem_used: " << memInfo->mem_used << "\n"
                 << endl;
            print_array(memInfo->mem_useds, memInfo->maxLength);
            cout << "\n"
                 << endl;
        }

        sleep(3);
    }
}