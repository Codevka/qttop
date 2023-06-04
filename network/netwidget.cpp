#include "netwidget.h"
#include "ui_netwidget.h"
#include "network.h"
#include<QString>
#include "netchart.h"
netwidget::netwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::netwidget)
{
    ui->setupUi(this);

    getnetinfo(ni);
    ni.netname.removeOne("lo");
    ui->comboBox->addItems(ni.netname);
    ui->label_6->setText(ni.ipadd[0]);
    this->timer.start(1000*time_rate);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(update_network()));
    connect(ui->comboBox,SIGNAL(currentTextChanged()),this,SLOT(on_comboBox_currentTextChanged()));
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
    QString rv_unit="B/s";
    QString sv_unit="B/s";
    long long rv = nfi.recvbytes-this->last_recvbytes;
    long long sv = nfi.sendbytes-this->last_sendbytes;
    if(rv<0){
        rv=0;
    }
    if(sv<0){
        sv=0;
    }
    ui->graphicsView->handle(sv,rv);
    if(rv>1024){
        rv_unit = "KB/s";
        rv = rv/1024;
    }
    if(rv>1024){
        rv_unit = "MB/s";
        rv = rv/1024;
    }
    ui->label_5->setText(QString::number(rv,'f',2)+rv_unit);

    if(sv>1024){
        sv_unit = "KB/s";
        sv = sv/1024;
    }
    if(sv>1024){
        sv_unit = "MB/s";
        sv = sv/1024;
    }
    ui->label_up->setText(QString::number(sv,'f',2)+sv_unit);

    this->last_recvbytes = nfi.recvbytes;
    this->last_sendbytes = nfi.sendbytes;



}

void netwidget::on_comboBox_currentTextChanged()
{
    ui->label_6->setText(ni.ipadd[ui->comboBox->currentIndex()]);
}
