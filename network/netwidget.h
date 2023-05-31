#ifndef NETWIDGET_H
#define NETWIDGET_H
#include "network.h"
#include <QWidget>
#include <QTimer>

namespace Ui {
class netwidget;
}

class netwidget : public QWidget
{
    Q_OBJECT

public:
    explicit netwidget(QWidget *parent = nullptr);
    ~netwidget();
    QTimer timer;
    double time_rate = 1.5;
    double last_recvbytes=0;
    double last_sendbytes=0;
public slots:
    void update_network(void);
private slots:
    void on_comboBox_currentTextChanged(QString ni);

private:
    Ui::netwidget *ui;
};

#endif // NETWIDGET_H
