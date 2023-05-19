#include "CpuInfoHandler.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

CpuInfoHandler::CpuInfoHandler() {
    try {
        cpu_name = getCpuName();
    } catch (QString e) {
        cpu_name = e;
    }
}

QString CpuInfoHandler::getCpuName() {
    QFile file("/proc/cpuinfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw "无法获取CPU信息";
    }

    QTextStream ins(&file);
    do {
        auto tmp_str        = ins.readLine();
        auto pos            = tmp_str.indexOf("model name");
        auto model_name_len = QString("model name\\t:").length();
        if (pos != -1) {
            pos += model_name_len;
            return QString(tmp_str.mid(pos, tmp_str.length() - model_name_len));
        }
    } while (!ins.atEnd());
    return QString("无法获取CPU名称");
}
