#include "netwidget.h"
#include "ui_netwidget.h"
#include "network.h"
#include<QString>

netwidget::netwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::netwidget)
{
    ui->setupUi(this);

    netinfo ni;
    getnetinfo(ni);
    ui->comboBox->addItems(ni.netname);
    ui->label_6->setText(ni.ipadd[0]);
    this->timer.start(1000*time_rate);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(update_network()));
    connect(ui->comboBox,SIGNAL(currentTextChanged(QString,ni.ipadd[1])),this,SLOT(on_comboBox_currentTextChanged(QString)));
}

netwidget::~netwidget()
{
    delete ui;
}
void netwidget::update_network() {
    netflowinfo nfi;
    QString name = ui->comboBox->currentText();
    strncpy(nfi.netname, name.toStdString().c_str(), sizeof(nfi.netname));
    getnetflowinfo(nfi);
    QString rv = QString::number((nfi.recvbytes-this->last_recvbytes)/(1024*1.5));
    ui->label_5->setText(rv);
    QString sv = QString::number((nfi.sendbytes-this->last_sendbytes)/(1024*1.5));

    ui->label_up->setText(sv);
    this->last_recvbytes = nfi.recvbytes;
    this->last_sendbytes = nfi.sendbytes;

}

void netwidget::on_comboBox_currentTextChanged(QString ni)
{
    ui->label_6->setText(ni);
}
