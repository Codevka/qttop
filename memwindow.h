#ifndef MEMWINDOW_H
#define MEMWINDOW_H

#include <QDialog>

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

class MemWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MemWindow(QWidget *parent = nullptr);
    ~MemWindow();

private slots:
    static void *thread_mem(void *arg);

private:
    Ui::MemWindow *ui;

};

#endif // MEMWINDOW_H
